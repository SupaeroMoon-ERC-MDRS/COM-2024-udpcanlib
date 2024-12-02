#pragma once
#include <thread>
#include <cstring>

#include "net.hpp"
#include "can.hpp"
#include "message_definitions.hpp"

#define PUSH_MSG(ctype, member)if(std::is_same<T,ctype>::value){res = member.access([this](ctype* msg){internal::CanMsgBytes canmsg(member.getId(), {});std::map<std::string, std::any> data = {};msg->saveTo(data);internal::Bitarray arr({});database.encode(canmsg.id, data, arr);canmsg.all_bytes.insert(canmsg.all_bytes.cbegin(), arr.get().cbegin(), arr.get().cend());udp.push(canmsg);});}

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
