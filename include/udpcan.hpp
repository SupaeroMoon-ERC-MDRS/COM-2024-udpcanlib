#pragma once
#include <thread>
#include <cstring>

#include "net.hpp"
#include "can.hpp"
#include "message_definitions.hpp"

#define UDPCAN_PORT 12121u
#define PUSH_MSG(ctype, member)if(std::is_same<T,ctype>::value){res = member.access([this](const ctype& msg){internal::CanMsgBytes canmsg(member.getId(), {});std::map<std::string, std::any> data = {};msg.saveTo(data);internal::Bitarray arr({});database.encode(canmsg.id, data, arr);canmsg.all_bytes.insert(canmsg.all_bytes.cbegin(), arr.get().cbegin(), arr.get().cend());udp.push(canmsg.all_bytes);});}

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
            NetworkHandler();
            ~NetworkHandler(){};

            uint32_t parse(const std::string& fn);

            uint32_t init();
            uint32_t reset();
            uint32_t close();

            uint32_t start();
            uint32_t stop();

            template<typename T>
            MessageWrapper<T>* get(){
                if(std::is_same<T,RemoteControl>::value){
                    return &remote_msg;
                }
            }

            template<typename T>
            uint32_t push(){
                uint32_t res = CAN_E_I_NO_SUCH_MSG;
                PUSH_MSG(RemoteControl, remote_msg)
                /*if(std::is_same<T,RemoteControl>::value){
                    res = remote_msg.access([this](const RemoteControl& msg){
                        //internal::CanMsgBytes canmsg(remote_msg.getId(), {});
                        //std::map<std::string, std::any> data = {};
                        //msg.saveTo(data);internal::Bitarray arr({});
                        //database.encode(canmsg.id, data, arr);
                        //canmsg.all_bytes.insert(canmsg.all_bytes.cbegin(), arr.get().cbegin(), arr.get().cend());
                        //udp.push(canmsg.all_bytes);
                    });
                }*/

                return res;
            }

            uint32_t flush();
    };
};
