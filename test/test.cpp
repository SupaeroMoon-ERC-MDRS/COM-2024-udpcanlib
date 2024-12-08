#include "udpcan.hpp"
#include <iostream>

int32_t main(){
    udpcan::NetworkHandler nh;
    uint32_t res;
    res = nh.parse("/home/davidgmolnar/Documents/COM-2024/COM-2024-DBC/comms.dbc");
    if(res != 0){
        std::cout << "Failed to parse, error code was " << res << std::endl;
        return -1;
    }

    res = nh.init();
    if(res != 0){
        std::cout << "Failed to init, error code was " << res << std::endl;
        return -1;
    }

    res = nh.start();
    if(res != 0){
        std::cout << "Failed to start thread, error code was " << res << std::endl;
        return -1;
    }

    udpcan::MessageWrapper<udpcan::RemoteControl>* remotemsg = nh.get<udpcan::RemoteControl>();

    bool e_stop = false;
    while(!e_stop){
        res = remotemsg->access([&e_stop](const udpcan::RemoteControl& remote_view){
            e_stop = remote_view.e_stop;

            std::cout << "====================\n";
            std::cout << "LB: " << remote_view.l_bottom << "\n";
            std::cout << "LT: " << remote_view.l_top << "\n";
            std::cout << "LR: " << remote_view.l_right << "\n";
            std::cout << "LL: " << remote_view.l_left << "\n";
            std::cout << "RB: " << remote_view.r_bottom << "\n";
            std::cout << "RT: " << remote_view.r_top << "\n";
            std::cout << "RR: " << remote_view.r_right << "\n";
            std::cout << "RL: " << remote_view.r_left << "\n";
            std::cout << "LS: " << remote_view.l_shoulder << "\n";
            std::cout << "RS: " << remote_view.r_shoulder << "\n";
            std::cout << "LTrigger: " << (uint16_t)remote_view.left_trigger << "\n"; // 0 not pressed 255 full pressed
            std::cout << "RTrigger: " << (uint16_t)remote_view.right_trigger << "\n"; // 0 not pressed 255 full pressed
            std::cout << "ThumbLX: " << (uint16_t)remote_view.thumb_left_x << "\n"; // left < right
            std::cout << "ThumbLY: " << (uint16_t)remote_view.thumb_left_y << "\n"; // top < bottom
            std::cout << "ThumbRX: " << (uint16_t)remote_view.thumb_right_x << "\n"; // left < right
            std::cout << "ThumbRY: " << (uint16_t)remote_view.thumb_right_y; // top < bottom
            std::cout << std::endl;
        });

        if(res != CAN_E_SUCCESS){
            //std::cout << "Failed to access RemoteControl, error code was " << res << std::endl;
        }
    }
    
    nh.stop();
    return 0;
}