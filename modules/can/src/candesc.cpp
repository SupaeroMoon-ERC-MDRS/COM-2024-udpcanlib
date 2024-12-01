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

uint32_t CanMessageDesc::decode(const Bitarray& message_payload_bits, std::map<std::string, int64_t>& out_int, std::map<std::string, float>& out_float) const{
    uint32_t res = CAN_E_SUCCESS;
    for(const std::pair<std::string, CanSignalDesc> sig : signals){
        if(sig.second.num_type64_id == ENumType64::FLOAT){
            float v = 0;
            CAN_E_FW_IF_ERR(sig.second.decode(message_payload_bits, v))
            out_float[sig.first] = v;
        }
        else if(sig.second.num_type64_id == ENumType64::UINT){
            uint64_t v = 0;
            CAN_E_FW_IF_ERR(sig.second.decode(message_payload_bits, v))
            out_int[sig.first] = (int64_t)v;
        }
        else if(sig.second.num_type64_id == ENumType64::INT){
            int64_t v = 0;
            CAN_E_FW_IF_ERR(sig.second.decode(message_payload_bits, v))
            out_int[sig.first] = v;
        }
    }
    return res;
}

//////////////////////////////////////////////////////////////////////

CanSignalDesc::CanSignalDesc()
:mask(Bitarray({})),shift(0),scale(0),offset(0),num_type64_id(ENumType64::UINT),int_type_id(EIntType::U32),name(""){

}

CanSignalDesc::CanSignalDesc(const uint32_t message_length)
:mask(Bitarray(message_length)),shift(0),scale(0),offset(0),num_type64_id(ENumType64::UINT),int_type_id(EIntType::U32),name(""){

}

CanSignalDesc::~CanSignalDesc(){
    name.clear();
}

ENumType64 CanSignalDesc::determineNumType64(const std::string& sign, const float scale, const float offset){
    if(std::abs(scale - int32_t(scale)) > 1e-5 || std::abs(offset - int32_t(offset)) > 1e-5){
        return ENumType64::FLOAT;
    }
    if(scale < 0 || offset < 0){
        return ENumType64::INT;
    }
    return ENumType64::UINT;
}

EIntType CanSignalDesc::determineIntType(const std::string& sign, const uint32_t length){
    bool u = sign == "+";
    uint32_t reqlen = std::ceil(length / 8.f) * 8;
    if(reqlen == 8){
        return u ? EIntType::U8 : EIntType::I8;
    }
    else if(reqlen == 16){
        return u ? EIntType::U16 : EIntType::I16;
    }
    else if(reqlen == 32){
        return u ? EIntType::U32 : EIntType::I32;
    }
    else{
        return u ? EIntType::U64 : EIntType::I64;
    }
}

uint32_t CanSignalDesc::parse(std::ifstream& in, const uint64_t eof){
    in.seekg((uint64_t)in.tellg() + 3u);
    
    uint32_t res;
    CAN_E_FW_IF_ERR(readNextString(in, eof, name))

    uint32_t length;
    uint32_t endian;
    std::string sign;

    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, shift))
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, length))
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, endian))
    CAN_E_FW_IF_ERR(readNextString(in, eof, sign))
    CAN_E_FW_IF_ERR(readNextFloating(in, eof, scale))
    CAN_E_FW_IF_ERR(readNextFloating(in, eof, offset))

    if(shift > mask.size() * 8u) return CAN_E_SIGNAL_POS_OB;
    if(shift + length > mask.size() * 8u) return CAN_E_SIGNAL_POS_OB;

    mask.set(shift, length);

    num_type64_id = determineNumType64(sign, scale, offset);
    int_type_id = determineIntType(sign, length);
    return CAN_E_SUCCESS;
}

template<typename NumType64>
uint32_t CanSignalDesc::decode(const Bitarray& message_payload_bits, NumType64& out) const{
    Bitarray part = (message_payload_bits & mask) >> shift;
    switch (int_type_id){
        case EIntType::U8:
            out = (NumType64)part.as<uint8_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;
        
        case EIntType::U16:
            out = (NumType64)part.as<uint16_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::U32:
            out = (NumType64)part.as<uint32_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::U64:
            out = (NumType64)part.as<uint64_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::I8:
            out = (NumType64)part.as<int8_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::I16:
            out = (NumType64)part.as<int16_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::I32:
            out = (NumType64)part.as<int32_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;

        case EIntType::I64:
            out = (NumType64)part.as<int64_t>() * (NumType64)scale + (NumType64)offset;
            return CAN_E_SUCCESS;
    }
    return CAN_E_SUCCESS;
}