#pragma once
#include <cstdint>
#include <vector>

namespace udpcan{
    namespace internal{

        struct CanMsgBytes{
            uint8_t id;
            std::vector<uint8_t> bytes;

            CanMsgBytes(const uint8_t id, const std::vector<uint8_t> bytes);
            ~CanMsgBytes();
        };

        class UDP{
            private:
                bool initialized;
                bool need_reset;
                uint16_t expect_dbc_version;
                std::vector<uint8_t> buf;
                std::vector<uint8_t> outbuf;
                std::vector<CanMsgBytes> messages;

                uint32_t readBuf();
                uint32_t readMsg();

            public:
                UDP();
                ~UDP();

                uint32_t init(const uint16_t dbc_version);
                uint32_t reset(const uint16_t dbc_version);
                uint32_t close();

                bool isInitialized(){
                    return initialized;
                }

                bool needReset(){
                    return need_reset;
                }

                uint32_t recv(); // thrsafety
                uint32_t getMessages(std::vector<CanMsgBytes>& messages); // thrsafety
                uint32_t push(const CanMsgBytes message); // thrsafety
                uint32_t push(const std::vector<CanMsgBytes>& messages); // thrsafety
                uint32_t flush(); // thrsafety
        };
    };
};