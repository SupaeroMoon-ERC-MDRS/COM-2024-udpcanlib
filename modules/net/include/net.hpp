#pragma once
#include <cstdint>
#include <vector>

namespace Net{
    struct CanMsgBytes{
        uint8_t id;
        std::vector<uint8_t> bytes;
    };


    class UDP{
        private:
            bool initialized;
            bool need_reset;
            std::vector<uint8_t> buf;
            std::vector<CanMsgBytes> messages;

            uint32_t readBuf();
            uint32_t readMsg();

        public:
            UDP();
            ~UDP();

            uint32_t init();
            uint32_t reset();
            uint32_t close();

            bool isInitialized(){
                return initialized;
            }

            bool needReset(){
                return need_reset;
            }

            uint32_t recv();
            uint32_t getMessages(std::vector<CanMsgBytes>& messages);
            uint32_t push(const CanMsgBytes& message);
            uint32_t push(const std::vector<CanMsgBytes>& messages);
            uint32_t flush();

    };
};