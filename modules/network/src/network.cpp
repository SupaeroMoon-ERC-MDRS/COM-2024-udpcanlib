#include "network.hpp"

using namespace udpcan::internal;

UDP::UDP(){

}

UDP::~UDP(){
    shutdown();
}

uint32_t UDP::recv(){
    return net.recv();
}

uint32_t UDP::getPackets(std::vector<RecvPacket>& packets){
    return net.getPackets(packets);
}