#include "can.hpp"

using namespace udpcan::internal;

int32_t main(){
    Bitarray arr({0x80, 0xFF, 0x07, 0x0F});

    Bitarray shift = arr >> 2;

    int8_t a = arr.as<int8_t>();
    int32_t b = arr.as<int32_t>();

    return 0;
};