#include "can.hpp"

using namespace udpcan::internal;

CanMessageDesc::CanMessageDesc():signals({}),id(0),message_length(0){

}

CanMessageDesc::~CanMessageDesc(){
    signals.clear();
}

uint32_t CanMessageDesc::parse(std::ifstream& in, const uint64_t eof){
    uint32_t tmp;
    uint32_t res;    
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, tmp))
    id = (uint8_t)tmp;
    
    CAN_E_FW_IF_ERR(readNextString(in, eof, name))
    
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
        CAN_E_FW_IF_ERR(desc.parse(in, msg_eof))
        
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
    
    uint32_t res;
    CAN_E_FW_IF_ERR(readNextString(in, eof, name))

    uint32_t start;
    uint32_t length;
    uint32_t endian;
    std::string sign;

    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, start))
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, length))
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, endian))
    CAN_E_FW_IF_ERR(readNextString(in, eof, sign))
    CAN_E_FW_IF_ERR(readNextFloating(in, eof, scale))
    CAN_E_FW_IF_ERR(readNextFloating(in, eof, offset))

    shift = start;
    // create mask, at first lets not mess with endian and sign
    return CAN_E_SUCCESS;
}