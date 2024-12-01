#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <functional>

#include "definitions.h"

namespace udpcan{

    template<typename T>
    struct MessageWrapper{
        private:
            T* message;
            bool updated;
            uint8_t id;
            std::mutex mtx;

        public:
            uint32_t access(const std::function<void(const T*)>& accessor){
                std::unique_lock lk(mtx);

                if(!updated) return CAN_E_NOTUPDATED;
                if(id == CAN_INVALID_ID) return CAN_E_WRAPPER_NOT_INITIALIZED;

                accessor(message);
                updated = false;
                return CAN_E_SUCCESS;
            }

            uint32_t update(const std::function<void(T*)>& accessor){
                std::unique_lock lk(mtx);

                if(id == CAN_INVALID_ID) return CAN_E_WRAPPER_NOT_INITIALIZED;

                accessor(message);
                updated = true;
                return CAN_E_SUCCESS;
            }

            uint32_t getId(uint8_t& i){
                std::unique_lock lk(mtx);
                if(id == CAN_INVALID_ID) return CAN_E_WRAPPER_NOT_INITIALIZED;
                i = id;
            }

            uint8_t getId(){
                std::unique_lock lk(mtx);
                return id;
            }

            uint32_t hasUpdate(bool& f){
                std::unique_lock lk(mtx);
                if(id == CAN_INVALID_ID) return CAN_E_WRAPPER_NOT_INITIALIZED;
                f = updated;
            }

            uint32_t clearUpdate(){
                std::unique_lock lk(mtx);
                if(id == CAN_INVALID_ID) return CAN_E_WRAPPER_NOT_INITIALIZED;
                updated = false;
            }

    };

    #pragma pack(push,1)
    struct RemoteControl{
        bool l_top : 1;
        bool l_bottom : 1;
        bool l_right : 1;
        bool l_left : 1;    
        bool r_top : 1;
        bool r_bottom : 1;
        bool r_right : 1;
        bool r_left : 1;
        bool l_shoulder : 1;
        bool r_shoulder : 1;
        bool e_stop : 1;
        uint8_t left_trigger : 8;
        uint8_t right_trigger : 8;
        uint8_t thumb_left_x : 8;
        uint8_t thumb_left_y : 8;
        uint8_t thumb_right_x : 8;
        uint8_t thumb_right_y : 8;
    };
    #pragma pack(pop)
};