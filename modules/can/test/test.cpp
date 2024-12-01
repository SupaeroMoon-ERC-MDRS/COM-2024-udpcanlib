#include "can.hpp"

using namespace udpcan::internal;

int32_t main(){
    CanDatabase db;
    uint32_t res = db.parse("/home/davidgmolnar/Documents/COM-2024/COM-2024-DBC/comms.dbc");
    std::map<std::string, int64_t> out_int = {};
    std::map<std::string, float> out_float = {};
    Bitarray in({0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF});
    res = db.decode(in, out_int, out_float);
    return 0;
};