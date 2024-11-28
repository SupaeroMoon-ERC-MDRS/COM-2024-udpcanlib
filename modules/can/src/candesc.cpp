#include "can.hpp"

using namespace udpcan::internal;

CanMessageDesc::CanMessageDesc():signals({}),id(0),message_length(0){

}

CanMessageDesc::~CanMessageDesc(){
    signals.clear();
}

uint32_t CanMessageDesc::parse(std::ifstream& in, const uint64_t eof){
    uint32_t tmp;
    uint32_t res = readNextNumeric(in, eof, tmp);
    if(res != CAN_E_SUCCESS) return res;
    id = (uint8_t)tmp;

    res = readNextString(in, eof, name);
    if(res != CAN_E_SUCCESS) return res;
    
    res = readNextNumeric(in, eof, message_length);
    if(res != CAN_E_SUCCESS) return res;

    uint64_t pos = in.tellg();
    uint64_t msg_eof = eof;
    res = seekUntil(in, eof, "BO_");
    if(res == CAN_E_SUCCESS){
        msg_eof = in.tellg();
    }
    in.seekg(pos);

    while(true){
        res = seekUntil(in, msg_eof, "SG_");
        if(res != CAN_E_SUCCESS){
            if(signals.empty()) return CAN_E_FILE_UNEXPECTED_EOF;
            return CAN_E_SUCCESS;
        }

        CanSignalDesc desc(message_length);
        res = desc.parse(in, msg_eof);
        if(res != CAN_E_SUCCESS) return res;
        signals[desc.name] = desc;
    }
}

//////////////////////////////////////////////////////////////////////

CanSignalDesc::CanSignalDesc():mask(Bitarray({})),shift(0),scale(0),offset(0),name(""){

}

CanSignalDesc::CanSignalDesc(const uint32_t message_length):mask(Bitarray(message_length)),shift(0),scale(0),offset(0),name(""){

}

CanSignalDesc::~CanSignalDesc(){
    name.clear();
}


uint32_t CanSignalDesc::parse(std::ifstream& in, const uint64_t eof){
    in.seekg((uint64_t)in.tellg() + 3u);
    
    uint32_t res = readNextString(in, eof, name);
    if(res != CAN_E_SUCCESS) return res;

    uint32_t start;
    uint32_t length;
    uint32_t endian;
    std::string sign;

    res = readNextNumeric(in, eof, start);
    if(res != CAN_E_SUCCESS) return res;

    res = readNextNumeric(in, eof, length);
    if(res != CAN_E_SUCCESS) return res;

    res = readNextNumeric(in, eof, endian);
    if(res != CAN_E_SUCCESS) return res;

    res = readNextString(in, eof, sign);
    if(res != CAN_E_SUCCESS) return res;

    res = readNextFloating(in, eof, scale);
    if(res != CAN_E_SUCCESS) return res;

    res = readNextFloating(in, eof, offset);
    if(res != CAN_E_SUCCESS) return res;

    shift = start;
    // create mask, at first lets not mess with endian and sign
    return CAN_E_SUCCESS;
}