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
    CAN_E_FW_IF_ERR(readNextNumeric(in, eof, message_length))

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

void CanMessageDesc::getSignalNames(std::set<std::string>& in) const {
    for(const std::pair<std::string, CanSignalDesc> p : signals){
        in.insert(p.first);
    }
}

uint32_t CanMessageDesc::decode(const Bitarray& message_payload_bits, std::map<std::string, std::any>& out) const{
    uint32_t res = CAN_E_SUCCESS;
    for(const std::pair<std::string, CanSignalDesc> sig : signals){
        DECODE_SIG(float, NF32)
        DECODE_SIG(uint8_t, NU8)
        DECODE_SIG(uint16_t, NU16)
        DECODE_SIG(uint32_t, NU32)
        DECODE_SIG(uint64_t, NU64)
        DECODE_SIG(int8_t, NI8)
        DECODE_SIG(int16_t, NI16)
        DECODE_SIG(int32_t, NI32)
        DECODE_SIG(int64_t, NI64)
    }
    return res;
}

uint32_t CanMessageDesc::encode(const std::map<std::string, std::any>& in, Bitarray& out, const uint16_t version) const{
    Bitarray msg = Bitarray(message_length);
    for(const std::pair<std::string, std::any> p : in){
        Bitarray sig = Bitarray({});
        uint32_t res = CAN_E_I_NO_SUCH_MSG;

        if(signals.at(p.first).num_type_id == ENumType::NU8){
            res = signals.at(p.first).encode<uint8_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NU16){
            res = signals.at(p.first).encode<uint16_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NU32){
            res = signals.at(p.first).encode<uint32_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NU64){
            res = signals.at(p.first).encode<uint64_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NI8){
            res = signals.at(p.first).encode<int8_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NI16){
            res = signals.at(p.first).encode<int16_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NI32){
            res = signals.at(p.first).encode<int32_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NI64){
            res = signals.at(p.first).encode<int64_t>(p.second, sig);
        }
        else if(signals.at(p.first).num_type_id == ENumType::NF32){
            res = signals.at(p.first).encode<float>(p.second, sig);
        }

        if(res != CAN_E_SUCCESS){
            return res;
        }

        msg |= sig;
    }

    std::vector<uint8_t> pack(message_length + 1, 0);
    std::copy(msg.cbegin(), msg.cend(), pack.begin() + 1);
    pack[0] = id;
    out = Bitarray(pack);
    return CAN_E_SUCCESS;
}

//////////////////////////////////////////////////////////////////////

CanSignalDesc::CanSignalDesc()
:mask(Bitarray({})),shift(0),scale(0),offset(0),num_type_id(ENumType::NU8),int_type_id(EIntType::U32),name(""){

}

CanSignalDesc::CanSignalDesc(const uint32_t message_length)
:mask(Bitarray(message_length)),shift(0),scale(0),offset(0),num_type_id(ENumType::NU8),int_type_id(EIntType::U32),name(""){

}

CanSignalDesc::~CanSignalDesc(){
    name.clear();
}

ENumType CanSignalDesc::determineNumType(const std::string& sign, const uint32_t length, const float scale, const float offset){
    if(std::abs(scale - int32_t(scale)) > 1e-5 || std::abs(offset - int32_t(offset)) > 1e-5){
        return ENumType::NF32;
    }

    if(sign == "+"){
        int64_t critical_1 = (int64_t)(std::pow(2, length) * scale + offset);
        int64_t critical_2 = (int64_t)(offset);

        uint32_t bitreq = std::max(std::log2(critical_1), std::log2(critical_2));
        uint32_t reqlen = std::ceil(bitreq / 8.f) * 8;
        
        bool neg = scale < 0 || offset < 0;

        if(reqlen == 8){
            return neg ? ENumType::NI8 : ENumType::NU8;
        }
        else if(reqlen == 16){
            return neg ? ENumType::NI16 : ENumType::NU16;
        }
        else if(reqlen == 32){
            return neg ? ENumType::NI32 : ENumType::NU32;
        }
        else{
            return neg ? ENumType::NI64 : ENumType::NU64;
        }
    }
    else{
        throw std::logic_error("not yet");
    }
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

    int_type_id = determineIntType(sign, length);
    num_type_id = determineNumType(sign, length, scale, offset);
    return CAN_E_SUCCESS;
}

template<typename NumType>
uint32_t CanSignalDesc::decode(const Bitarray& message_payload_bits, NumType& out) const{
    Bitarray part = (message_payload_bits & mask) >> shift;
    switch (int_type_id){
        case EIntType::U8:
            out = (NumType)part.as<uint8_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;
        
        case EIntType::U16:
            out = (NumType)part.as<uint16_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::U32:
            out = (NumType)part.as<uint32_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::U64:
            out = (NumType)part.as<uint64_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::I8:
            out = (NumType)part.as<int8_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::I16:
            out = (NumType)part.as<int16_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::I32:
            out = (NumType)part.as<int32_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;

        case EIntType::I64:
            out = (NumType)part.as<int64_t>() * (NumType)scale + (NumType)offset;
            return CAN_E_SUCCESS;
    }
    return CAN_E_SUCCESS;
}

template<typename NumType>
uint32_t CanSignalDesc::encode(const std::any num, Bitarray& out) const {
    NumType value = (std::any_cast<NumType>(num) - (NumType)offset) / (NumType)scale;
    switch (int_type_id){
        case EIntType::U8:
            out = Bitarray((uint8_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::U16:
            out = Bitarray((uint16_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::U32:
            out = Bitarray((uint32_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::U64:
            out = Bitarray((uint64_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::I8:
            out = Bitarray((int8_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::I16:
            out = Bitarray((int16_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::I32:
            out = Bitarray((int32_t)std::round(value), mask.size()) << shift;
            break;
        case EIntType::I64:
            out = Bitarray((int64_t)std::round(value), mask.size()) << shift;
            break;
        }
    return CAN_E_SUCCESS;
}