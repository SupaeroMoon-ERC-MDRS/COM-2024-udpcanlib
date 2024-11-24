#include "net.hpp"
#include "can.hpp"

namespace udpcan{

    class NetworkHandler{
        private:
            // TODO storage for latest received messages
            MessageWrapper<RemoteControl> remote_msg;

            internal::CanDatabase database;
            internal::UDP udp;

        public:
            NetworkHandler();
            ~NetworkHandler();

            uint32_t parse(const std::string& fn) {
                return database.parse(fn);
            }

            uint32_t init(){
                return udp.init();
            }
            uint32_t reset(){
                return udp.reset();
            }
            uint32_t close(){
                return udp.close();
            }

            uint32_t start(); // thread

            template<typename T>
            MessageWrapper<T>* get(){
                if(std::is_same<T,RemoteControl>::value){
                    return &remote_msg;
                }
            }

            template<typename T>
            uint32_t push(){
                if(std::is_same<T,RemoteControl>::value){
                    // set updated to false
                    // udp.push(internal::CanMsgBytes of remotemsg)
                }
            }

            uint32_t flush(){
                udp.flush();
            }
    };
};
