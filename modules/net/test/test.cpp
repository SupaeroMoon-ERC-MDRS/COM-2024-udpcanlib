#include "net.hpp"

int32_t main(){
    udpcan::internal::UDP udp;
    udp.init(0, {{15,8}}, 8000);
    while(true){
        if(udp.isInitialized() && !udp.needReset()){
            udp.recv();
            udp.push({72, 101, 108, 108, 111, 33, 0});
            udp.flush();
        }
        else{
            udp.reset(0, {{15,8}}, 8000);
        }
    }
    return 0;
}