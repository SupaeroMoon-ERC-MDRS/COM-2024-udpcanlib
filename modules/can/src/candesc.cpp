#include "can.hpp"

using namespace udpcan::internal;

CanMessageDesc::CanMessageDesc():signals({}),id(0),message_length(0){

}

CanMessageDesc::~CanMessageDesc(){
    signals.clear();
}

//////////////////////////////////////////////////////////////////////

CanSignalDesc::CanSignalDesc(const uint32_t message_length):mask(Bitarray(message_length)),shift(0),scale(0),offset(0),name(""){

}

CanSignalDesc::~CanSignalDesc(){
    name.clear();
}