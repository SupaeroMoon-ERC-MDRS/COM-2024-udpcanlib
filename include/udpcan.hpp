#pragma once
#include <thread>
#include <cstring>
#include <cstdio>
#include <iostream>

#include "network.hpp"
#include "can.hpp"
#include "message_definitions.hpp"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

#define UDPCAN_PORT 12121u
//#define PUSH_MSG(ctype, member)if(std::is_same<T,ctype>::value){res = member.access([this](const ctype& msg){internal::CanMsgBytes canmsg(member.getId(), {});std::map<std::string, std::any> data = {};msg.saveTo(data);std::vector<uint8_t> arr = {};database.encode(canmsg.id, data, arr);canmsg.all_bytes.insert(canmsg.all_bytes.cbegin(), arr.cbegin(), arr.cend());udp.push(canmsg.all_bytes);});}
#define PUSH_MSG(ctype, member)if(std::is_same<T,ctype>::value){res = member.access([this](const ctype& msg){;std::map<std::string, std::any> data = {};msg.saveTo(data);std::vector<uint8_t> arr = {};database.encode(member.getId(), data, arr);udp.push(arr);});}

namespace udpcan{

    class NetworkHandler{
        private:
            MessageWrapper<RemoteControl> remote_msg;
            MessageWrapper<RaspiState> raspi_state;
            MessageWrapper<NavOdometry> nav_odometry;

            internal::CanDatabase database;
            internal::UDP udp;

		    std::thread thr;
            std::mutex thr_mtx;
            bool stop_thr;
            std::string subnet = "192.168.43.";

            void thread();
            std::string getWlanIp();

        public:
            NetworkHandler();
            ~NetworkHandler(){};

            void setSubnet(const std::string sn){subnet = sn;}

            uint32_t parse(const std::string& fn);

            uint32_t init(const int32_t type);
            uint32_t reset();
            uint32_t close();

            uint32_t start();
            uint32_t stop();

            template<typename T>
            MessageWrapper<T>* get(){
                if constexpr (std::is_same<T,RemoteControl>::value){
                    return &remote_msg;
                }
                if constexpr (std::is_same<T,RaspiState>::value){
                    return &raspi_state;
                }
                if constexpr (std::is_same<T,NavOdometry>::value){
                    return &nav_odometry;
                }
            }

            template<typename T>
            uint32_t push(){
                uint32_t res = CAN_E_I_NO_SUCH_MSG;
                PUSH_MSG(RemoteControl, remote_msg)
                PUSH_MSG(RaspiState, raspi_state)
                PUSH_MSG(NavOdometry, nav_odometry)
                return res;
            }

            uint32_t flush();
    };
};
