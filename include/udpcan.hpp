#include <thread>

#include "net.hpp"
#include "can.hpp"
#include "message_definitions.hpp"

#define BURN_IN true

namespace udpcan{

    class NetworkHandler{
        private:
            // TODO storage for latest received messages
            MessageWrapper<RemoteControl> remote_msg;

            internal::CanDatabase database;
            internal::UDP udp;

		    std::thread thr;
            std::mutex thr_mtx;
            bool stop_thr;

            void thread();

        public:
            NetworkHandler(){};
            ~NetworkHandler(){};

            uint32_t parse(const std::string& fn);

            uint32_t init();
            uint32_t reset();
            uint32_t close();

            uint32_t start();
            uint32_t stop();

            template<typename T>
            MessageWrapper<T>* get();

            template<typename T>
            uint32_t push();

            uint32_t flush();
    };
};
