#include "network.hpp"

using namespace udpcan::internal;

CanMsgBytes::CanMsgBytes(const uint8_t id, const std::vector<uint8_t> all_bytes):id(id),all_bytes(all_bytes){

}

CanMsgBytes::~CanMsgBytes(){
    all_bytes.clear();
}

UDP::UDP(){

}

UDP::~UDP(){
    shutdown();
}

uint32_t UDP::recv(){
    uint32_t res = net.recv();
    if(res != NET_E_SUCCESS){
        return res;
    }

    std::vector<RecvPacket> p;
    net.getPackets(p);
    return readPackets(p);
}

uint32_t UDP::readPackets(const std::vector<RecvPacket>& packets){
    for(const RecvPacket& pack : packets){
        uint32_t pos = 0;
        while(pos + 3u <= pack.buf.size()){

            uint8_t msg_id = pack.buf[pos];
            pos++;

            auto it = std::find_if(expect_can_ids.cbegin(), expect_can_ids.cend(), [&msg_id](const std::pair<uint8_t, uint32_t>& id){
                return id.first == msg_id;
            });
            
            if(it == expect_can_ids.cend()){
                continue;
            }

            if(pos + it->second <= pack.buf.size()){
                in_messages.emplace_back(msg_id, std::vector<uint8_t>(pack.buf.data() + pos - 1, pack.buf.data() + pos + it->second));
                pos += it->second;
            }
            else{
                return NET_E_PARTIAL_MSG;
            }
        }
    }
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