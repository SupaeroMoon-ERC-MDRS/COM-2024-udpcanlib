#include "net.hpp"

using namespace udpcan::internal;

CanMsgBytes::CanMsgBytes(const uint8_t id, const std::vector<uint8_t> all_bytes):id(id),all_bytes(all_bytes){

}

CanMsgBytes::~CanMsgBytes(){
    all_bytes.clear();
}

UDP::UDP():initialized(false),need_reset(false),remote_connected(false),expect_dbc_version(0){

}

UDP::~UDP(){
    shutdown();
}

uint32_t UDP::init(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& can_ids, const uint16_t port){
    expect_dbc_version = dbc_version;
    expect_can_ids = can_ids;

    socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket_fd < 0){
        return NET_E_SOCK_FAIL_ASSIGN;
    }
    int broadcastEnable = 1;
    if(setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0){
        return NET_E_SOCK_FAIL_ASSIGN;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socket_fd, (const sockaddr *)&addr, sizeof(sockaddr_in)) < 0){
        return NET_E_SOCK_FAIL_BIND;
    }

    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) return NET_E_SOCK_FAIL_FCNTL;
    flags = flags | O_NONBLOCK;
    if(fcntl(socket_fd, F_SETFL, flags) != 0) return NET_E_SOCK_FAIL_FCNTL;

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(12122u);
    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);

    remote_addr_bytes.resize(sizeof(sockaddr_in));
    memcpy(remote_addr_bytes.data(), &remote_addr, sizeof(sockaddr_in));

    initialized = true;

    return NET_E_SUCCESS;
}

uint32_t UDP::shutdown(){
    disconnectRemote();

    if(close(socket_fd) != 0){
        return NET_E_CANT_CLOSE;
    }

    buf.clear();
    outbuf.clear();
    in_messages.clear();

    // DONT CLEAR connections

    initialized = false;
    return NET_E_SUCCESS;
}

uint32_t UDP::reset(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& can_ids, const uint16_t port){
    shutdown();
    uint32_t res = init(dbc_version, can_ids, port);
    need_reset = false;
    return res;
}

uint32_t UDP::recv(){
    uint32_t res = readBuf();
    if(res != NET_E_SUCCESS){
        return res;
    }
    return readMsg();
}

uint32_t UDP::readBuf(){
    bool stop = false;
    while(!stop){
        sockaddr_in addr;
        socklen_t addr_len = sizeof(sockaddr_in);
        std::vector<uint8_t> buffer(1024u, 0);
        int64_t nread;

        {
            std::lock_guard lk(sock_mtx);
            nread = recvfrom(socket_fd, buffer.data(), 1024, 0, (sockaddr *)&addr, &addr_len);
        }

        if(nread > 0){
            buf.emplace_back().addr = addr;
            buf.back().buf.resize(nread);
            std::copy(buffer.cbegin(), buffer.cbegin() + nread, buf.back().buf.begin());
        }
        else if(errno == EAGAIN){
            stop = true;
        }
        else{
            need_reset = true;
            return NET_E_SOCK_FAIL_RECV_ERRNO;
        }
    }
    
    return NET_E_SUCCESS;
}

uint32_t UDP::readMsg(){
    for(const RecvPacket& pack : buf){
        uint32_t pos = 0;
        while(pos + 3u <= pack.buf.size()){
            if(*(uint16_t*)(pack.buf.data() + pos) != expect_dbc_version){
                pos++;
                continue;
            }
            pos += 2;

            uint8_t msg_id = pack.buf[pos];
            pos++;
            if(msg_id <= 2){  // 0 = add me as conn, 1 = ack of conn, 2 = remove me as conn
                pos++;
                std::vector<uint8_t> search(sizeof(sockaddr_in));
                memcpy(search.data(), &pack.addr, sizeof(sockaddr_in));

                auto it = std::find_if(connections.begin(), connections.end(), [&search](sockaddr_in& conn){
                    std::vector<uint8_t> arr(sizeof(sockaddr_in));
                    memcpy(arr.data(), &conn, sizeof(sockaddr_in));
                    return search == arr;
                });
                if(it == connections.end() && msg_id == 0){
                    connections.push_back(pack.addr);
                }
                else if(it != connections.end() && msg_id == 2){
                    connections.erase(it);
                }
                else if(it == connections.end() && msg_id == 2){
                    if(search == remote_addr_bytes){
                        remote_connected = false;
                    }
                }
                else if(msg_id == 1){
                    if(pack.addr.sin_port == htons(12122u)){
                        remote_connected = true;
                        remote_addr_bytes = search;
                        remote_addr.sin_addr = pack.addr.sin_addr;
                    }
                }
                continue;
            }

            auto it = std::find_if(expect_can_ids.cbegin(), expect_can_ids.cend(), [&msg_id](const std::pair<uint8_t, uint32_t>& id){
                return id.first == msg_id;
            });
            
            if(it == expect_can_ids.cend()){
                continue;
            }

            if(pos + it->second <= pack.buf.size()){
                in_messages.emplace_back(msg_id, std::vector<uint8_t>(pack.buf.data() + pos - 3, pack.buf.data() + pos + it->second));
                pos += it->second;
            }
            else{
                return NET_E_PARTIAL_MSG;
            }
        }
    }
    buf.clear();
    return NET_E_SUCCESS;
}

uint32_t UDP::getMessages(std::vector<CanMsgBytes>& messages){
    if(in_messages.empty()){
        return NET_E_NO_UDPATE;
    }
    messages = in_messages;
    in_messages.clear();
    return NET_E_SUCCESS;
}

uint32_t UDP::push(const std::vector<uint8_t>& message){
    std::lock_guard lk(outbuf_mtx);
    std::copy(message.cbegin(), message.cend(), std::back_inserter(outbuf));
    return NET_E_SUCCESS;
}

uint32_t UDP::flush(){
    for(const sockaddr_in addr : connections){
        uint64_t sent;
        sent = sendto(socket_fd, outbuf.data(), outbuf.size(), 0, (const sockaddr *)&addr, sizeof(addr));

        if(sent == outbuf.size()){
            continue;
        }
        else if(sent < 0){
            need_reset = true;  // TODO maybe just mark connection to be removed
            return NET_E_SOCK_FAIL_SEND_ERRNO;
        }
        else{
            return NET_E_PARTIAL_MSG;
        }
    }
    outbuf.clear();

    return NET_E_SUCCESS;
}

uint32_t UDP::tryConnectRemote(){
    if(remote_connected){
        return NET_E_SUCCESS;
    }

    std::vector<uint8_t> buf(3, 0);
    *(uint16_t*)(buf.data()) = expect_dbc_version;

    {
        std::lock_guard lk(sock_mtx);
        uint32_t res = sendto(socket_fd, buf.data(), 3, 0, (const sockaddr *)&remote_addr, sizeof(remote_addr));
        if(res < 3){
            return NET_E_PARTIAL_MSG;
        }
    }

    return NET_E_SUCCESS;
}

uint32_t UDP::disconnectRemote(){
    if(!remote_connected){
        return NET_E_SUCCESS;
    }

    std::vector<uint8_t> buf(3, 2);
    *(uint16_t*)(buf.data()) = expect_dbc_version;

    {
        std::lock_guard lk(sock_mtx);
        uint32_t res = sendto(socket_fd, buf.data(), 3, 0, (const sockaddr *)&remote_addr, sizeof(remote_addr));
        if(res < 3){
            return NET_E_PARTIAL_MSG;
        }
    }

    return NET_E_SUCCESS;
}