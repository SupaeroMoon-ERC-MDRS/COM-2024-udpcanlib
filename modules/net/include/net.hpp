#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <mutex>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "defines.h"

#define REMOTE_IP "10.193.91.243"

namespace udpcan{
    namespace internal{

        struct CanMsgBytes{
            uint8_t id;
            std::vector<uint8_t> all_bytes;

            CanMsgBytes(const uint8_t id, const std::vector<uint8_t> all_bytes);
            ~CanMsgBytes();
        };

        struct RecvPacket{
            sockaddr_in addr;
            std::vector<uint8_t> buf;

            RecvPacket(){};
            ~RecvPacket(){};
        };

        class UDP{
            private:
                bool initialized;
                bool need_reset;

                bool remote_connected;
                uint16_t expect_dbc_version;
                std::vector<std::pair<uint8_t, uint32_t>> expect_can_ids;

                int32_t socket_fd;
                std::vector<sockaddr_in> connections;
                sockaddr_in remote_addr;
                std::vector<uint8_t> remote_addr_bytes;

                std::vector<RecvPacket> buf;
                std::vector<uint8_t> outbuf;
                std::vector<CanMsgBytes> in_messages;

                std::mutex sock_mtx;
                std::mutex outbuf_mtx;

                uint32_t readBuf();
                uint32_t readMsg();

            public:
                UDP();
                ~UDP();

                uint32_t init(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& expect_can_ids, const uint16_t port);
                uint32_t reset(const uint16_t dbc_version, const std::vector<std::pair<uint8_t, uint32_t>>& expect_can_ids, const uint16_t port);
                uint32_t shutdown();

                bool isInitialized(){
                    return initialized;
                }

                bool needReset(){
                    return need_reset;
                }

                uint32_t recv(); // thrsafety
                uint32_t getMessages(std::vector<CanMsgBytes>& messages); // thrsafety
                uint32_t push(const std::vector<uint8_t>& message); // thrsafety
                uint32_t flush(); // thrsafety
                uint32_t tryConnectRemote(); // thrsafety
                uint32_t disconnectRemote(); // thrsafety
        };
    };
};