#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <string.h>

#include "net.hpp"

#define REMOTE_IP "10.188.69.148"
#define REMOTE_PORT 12122u

namespace udpcan{
    namespace internal{

        struct CanMsgBytes{
            uint8_t id;
            std::vector<uint8_t> all_bytes;

            CanMsgBytes(const uint8_t id, const std::vector<uint8_t> all_bytes);
            ~CanMsgBytes();
        };

        class UDP{
            private:
                Net net;

                std::vector<std::pair<uint8_t, uint32_t>> expect_can_ids;

                sockaddr_in remote_addr;

                std::vector<CanMsgBytes> in_messages;

                uint32_t readPackets(const std::vector<RecvPacket>& packets);

            public:
                UDP();
                ~UDP();

                inline uint32_t init(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& can_ids, const uint16_t port){
                    expect_can_ids = can_ids;

                    remote_addr.sin_family = AF_INET;
                    remote_addr.sin_port = htons(REMOTE_PORT);
                    remote_addr.sin_addr.s_addr = inet_addr(REMOTE_IP);

                    return net.init(dbc_version, port, NodeType::ROVER);
                }
                inline uint32_t reset(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& can_ids, const uint16_t port){
                    expect_can_ids = can_ids;

                    return net.reset(dbc_version, port, NodeType::ROVER);
                }
                inline uint32_t shutdown(){
                    in_messages.clear();
                    return net.shutdown();
                }

                inline bool isInitialized(){
                    return net.isInitialized();
                }

                inline bool needReset(){
                    return net.needReset();
                }

                uint32_t recv();
                uint32_t getMessages(std::vector<CanMsgBytes>& messages);
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
                    return net.sendConn(REMOTE_PORT);
                }
                inline uint32_t disconnectRemote(){
                    return net.sendConn(REMOTE_PORT);
                }
        };
    };
};