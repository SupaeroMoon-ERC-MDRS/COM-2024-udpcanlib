#include "can.hpp"
#include "message_definitions.hpp"

using namespace udpcan::internal;

int32_t main(){
    CanDatabase db;
    uint32_t res = db.parse("C:\\Users\\Lenovo\\Desktop\\COM-2024-DBC\\comms.dbc");
    std::map<std::string, std::any> out = {};
    Bitarray in({0x00,0x00,0x07,0xAC,0x3F,0x07,0x2F,0xAF,0x0F});
    res = db.decode(in, out);

    udpcan::MessageWrapper<udpcan::RemoteControl> rem(15);
    uint8_t i = rem.getId();

    return 0;
};