#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <functional>

#include "definitions.h"

namespace udpcan{

    namespace internal{
        struct HasSerDes{
            virtual uint32_t updateFrom(const std::map<std::string, std::any>& data) = 0;
            virtual uint32_t saveTo(std::map<std::string, std::any>& data) = 0;
        };
    };

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
    struct RemoteControl : public internal::HasSerDes{
        bool l_top;
        bool l_bottom;
        bool l_right;
        bool l_left;    
        bool r_top;
        bool r_bottom;
        bool r_right;
        bool r_left;
        bool l_shoulder;
        bool r_shoulder;
        bool e_stop;
        uint8_t left_trigger;
        uint8_t right_trigger;
        uint8_t thumb_left_x;
        uint8_t thumb_left_y;
        uint8_t thumb_right_x;
        uint8_t thumb_right_y;

        uint32_t updateFrom(const std::map<std::string, std::any>& data){
            try{
                l_top = (bool)std::any_cast<uint8_t>(data.at("l_top"));
                l_bottom = (bool)std::any_cast<uint8_t>(data.at("l_bottom"));
                l_right = (bool)std::any_cast<uint8_t>(data.at("l_right"));
                l_left = (bool)std::any_cast<uint8_t>(data.at("l_left"));
                r_top = (bool)std::any_cast<uint8_t>(data.at("r_top"));
                r_bottom = (bool)std::any_cast<uint8_t>(data.at("r_bottom"));
                r_right = (bool)std::any_cast<uint8_t>(data.at("r_right"));
                r_left = (bool)std::any_cast<uint8_t>(data.at("r_left"));
                l_shoulder = (bool)std::any_cast<uint8_t>(data.at("l_shoulder"));
                r_shoulder = (bool)std::any_cast<uint8_t>(data.at("r_shoulder"));
                e_stop = (bool)std::any_cast<uint8_t>(data.at("e_stop"));
                left_trigger = std::any_cast<uint8_t>(data.at("left_trigger"));
                right_trigger = std::any_cast<uint8_t>(data.at("right_trigger"));
                thumb_left_x = std::any_cast<uint8_t>(data.at("thumb_left_x"));
                thumb_left_y = std::any_cast<uint8_t>(data.at("thumb_left_y"));
                thumb_right_x = std::any_cast<uint8_t>(data.at("thumb_right_x"));
                thumb_right_y = std::any_cast<uint8_t>(data.at("thumb_right_y"));
            }
            catch(...){
                return CAN_E_I_KEYERR;
            }
            return CAN_E_SUCCESS;
        }

        uint32_t saveTo(std::map<std::string, std::any>& data){
            data.clear();
            data["l_top"] = l_top;
            data["l_bottom"] = l_bottom;
            data["l_right"] = l_right;
            data["l_left"] = l_left;
            data["r_top"] = r_top;
            data["r_bottom"] = r_bottom;
            data["r_right"] = r_right;
            data["r_left"] = r_left;
            data["l_shoulder"] = l_shoulder;
            data["r_shoulder"] = r_shoulder;
            data["e_stop"] = e_stop;
            data["left_trigger"] = left_trigger;
            data["right_trigger"] = right_trigger;
            data["thumb_left_x"] = thumb_left_x;
            data["thumb_left_y"] = thumb_left_y;
            data["thumb_right_x"] = thumb_right_x;
            data["thumb_right_y"] = thumb_right_y;
            return CAN_E_SUCCESS;
        }


    };
    #pragma pack(pop)
};