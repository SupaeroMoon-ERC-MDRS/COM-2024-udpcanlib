#include "can.hpp"
#include "message_definitions.hpp"

using namespace udpcan::internal;

void basic_test(){
    CanDatabase db;
    uint32_t res = db.parse("C:\\Users\\Lenovo\\Desktop\\COM-2024\\COM-2024-DBC\\comms.dbc");
    std::map<std::string, std::any> out = {};
    std::vector<uint8_t> in({0x0F,0xAC,0x06,0x07,0x2F,0xAF,0x0F,0x2F,0xAF});
    res = db.decode(in, out);

    std::vector<uint8_t> enc({});
    res = db.encode(0x0F, out, enc);

    udpcan::MessageWrapper<udpcan::RemoteControl> rem(15);
    uint8_t i = rem.getId();
}

void vector_test(){
    CanDatabase db;
    uint32_t res = db.parse("..\\test\\test.dbc");

    std::map<std::string, std::any> in = {};
    in["AsigA"] = (uint8_t)10;
    in["AsigB"] = (uint8_t)12;
    in["AsigC"] = (uint8_t)14;
    in["AsigD"] = (uint8_t)16;

    std::vector<uint8_t> asige = {0, 10, 1, 9, 2, 8};
    in["AsigE"] = asige;

    std::vector<int32_t> asigf = {-10, 10, 1, -1, 2, -2};
    in["AsigF"] = asigf;

    std::map<std::string, std::any> in2 = {};
    in2["BsigA"] = (uint8_t)20;
    in2["BsigB"] = (uint8_t)22;
    in2["BsigC"] = (uint8_t)24;
    in2["BsigD"] = (uint8_t)26;

    std::vector<uint8_t> bsige = {10, 1, 9, 2, 8, 0};
    in2["BsigE"] = bsige;

    std::vector<int32_t> bsigf = {10, 1, -1, 2, -2, -10};
    in2["BsigF"] = bsigf;

    std::vector<uint8_t> enc;
    std::vector<uint8_t> enc2;
    res = db.encode(0x0F, in, enc);
    res = db.encode(0x0E, in2, enc2);
    std::copy(enc2.cbegin(), enc2.cend(), std::back_inserter(enc));

    std::map<std::string, std::any> out = {};
    res = db.decode(enc, out);

    uint8_t asiga_dec = std::any_cast<uint8_t>(out["AsigA"]);
    uint8_t asigb_dec = std::any_cast<uint8_t>(out["AsigB"]);
    uint8_t asigc_dec = std::any_cast<uint8_t>(out["AsigC"]);
    uint8_t asigd_dec = std::any_cast<uint8_t>(out["AsigD"]);
    std::vector<uint8_t> asige_dec = std::any_cast<std::vector<uint8_t>>(out["AsigE"]);
    std::vector<int32_t> asigf_dec = std::any_cast<std::vector<int32_t>>(out["AsigF"]);
    uint8_t bsiga_dec = std::any_cast<uint8_t>(out["BsigA"]);
    uint8_t bsigb_dec = std::any_cast<uint8_t>(out["BsigB"]);
    uint8_t bsigc_dec = std::any_cast<uint8_t>(out["BsigC"]);
    uint8_t bsigd_dec = std::any_cast<uint8_t>(out["BsigD"]);
    std::vector<uint8_t> bsige_dec = std::any_cast<std::vector<uint8_t>>(out["BsigE"]);
    std::vector<int32_t> bsigf_dec = std::any_cast<std::vector<int32_t>>(out["BsigF"]);
    int a = 0;
}

int32_t main(){
    //basic_test();
    vector_test();
    return 0;
};