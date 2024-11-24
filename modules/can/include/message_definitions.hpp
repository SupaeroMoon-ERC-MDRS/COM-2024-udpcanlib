#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <functional>

#define CAN_INVALID_ID (uint8_t)0;

#define CAN_E_SUCCESS (uint32_t)0;

#define CAN_E_NOTUPDATED (uint32_t)1;

#define CAN_E_WARNING_MAX = (uint32_t)1023;
#define CAN_E_WRAPPER_NOT_INITIALIZED (uint32_t)1024;

namespace udpcan{
    namespace internal{

        struct Marshalable{
            std::vector<uint8_t> toBytes() {return {};}
            static Marshalable fromBytes(const std::vector<uint8_t>& bytes) {return Marshalable();}
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
            uint32_t access(std::function<void(T*)>& accessor){
                std::unique_lock(mtx);

                if(!updated){return CAN_E_NOTUPDATED;}
                if(id == CAN_INVALID_ID){return CAN_E_WRAPPER_NOT_INITIALIZED;}

                accessor(message);
                updated = false;
                return CAN_E_SUCCESS;
            }

            uint32_t update(std::function<void(T*)>& accessor){
                std::unique_lock(mtx);

                if(id == CAN_INVALID_ID){return CAN_E_WRAPPER_NOT_INITIALIZED;}

                accessor(message);
                updated = true;
                return CAN_E_SUCCESS;
            }

            uint32_t getId(uint8_t& id);
            uint32_t hasUpdate(bool& f);

    };

    MessageWrapper<internal::Marshalable>;

    #pragma pack(push,1)
    struct RemoteControl : public internal::Marshalable{
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
        uint8_t left_trigger : 8;
        uint8_t right_trigger : 8;
        uint8_t thumb_left_x : 8;
        uint8_t thumb_left_y : 8;
        uint8_t thumb_right_x : 8;
        uint8_t thumb_right_y : 8;
    };
    #pragma pack(pop)
};