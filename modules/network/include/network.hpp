#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <string.h>

#include "net.hpp"

#define REMOTE_IP "255.255.255.255" // 255.255.255.255 "10.188.69.148"
#define REMOTE_PORT 12122u

namespace udpcan{
    namespace internal{

        class UDP{
            private:
                std::string ip;
                Net net;
                sockaddr_in remote_addr;
                NodeType type;

            public:
                UDP();
                ~UDP();

                inline uint32_t init(const uint16_t dbc_version, const std::string ip, const uint16_t port, const NodeType node){
                    type = node;
                    this->ip = ip;
                    remote_addr.sin_family = AF_INET;
                    remote_addr.sin_port = htons(REMOTE_PORT);
                    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);

                    return net.init(dbc_version, ip, port, node);
                }

                inline uint32_t reset(const uint16_t dbc_version, const uint16_t port){
                    return net.reset(dbc_version, ip, port, type);
                }

                inline uint32_t shutdown(){
                    return net.shutdown();
                }

                inline bool isInitialized(){
                    return net.isInitialized();
                }

                inline bool needReset(){
                    return net.needReset();
                }

                uint32_t recv();
                uint32_t getPackets(std::vector<RecvPacket>& packets);

                inline uint32_t push(const std::vector<uint8_t>& message){
                    return net.push(message);
                }

                inline uint32_t flush(){
                    return net.flush();
                }

                inline uint32_t tryConnectRemote(){
                    if(net.hasPublishers(NodeType::REMOTE)){
                        return NET_E_SUCCESS;
                    }
                    return net.sendConn("", REMOTE_PORT) + net.sendConn(ip, REMOTE_PORT); // for webcontrol
                }

                inline uint32_t disconnectRemote(){
                    return net.sendDisc(NodeType::REMOTE);
                }
        };
    };
};