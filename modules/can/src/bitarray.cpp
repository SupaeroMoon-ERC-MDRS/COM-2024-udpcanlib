#include "can.hpp"

using namespace udpcan::internal;

Bitarray::Bitarray(const std::vector<uint8_t>& init){
    buf = init; 
}

Bitarray::Bitarray(const uint32_t size){
    buf.resize(size); 
}

Bitarray::~Bitarray(){
    buf.clear();
}

Bitarray Bitarray::operator&(const Bitarray& rhs) const{
    // assume same length
    Bitarray res({});

    std::transform(buf.cbegin(), buf.cend(), rhs.buf.cbegin(), std::back_inserter(res.buf), [](const uint8_t& a, const uint8_t& b){
        return a & b;
    });

    return res;
}

Bitarray& Bitarray::operator&=(const Bitarray& rhs){
    // assume same length

    std::transform(buf.begin(), buf.end(), rhs.buf.cbegin(), buf.begin(), [](const uint8_t& a, const uint8_t& b){
        return a & b;
    });

    return *this;
}

Bitarray Bitarray::operator|(const Bitarray& rhs) const{
    // assume same length
    Bitarray res({});

    std::transform(buf.cbegin(), buf.cend(), rhs.buf.cbegin(), std::back_inserter(res.buf), [](const uint8_t& a, const uint8_t& b){
        return a | b;
    });

    return res;
}

Bitarray& Bitarray::operator|=(const Bitarray& rhs){
    // assume same length

    std::transform(buf.begin(), buf.end(), rhs.buf.cbegin(), buf.begin(), [](const uint8_t& a, const uint8_t& b){
        return a | b;
    });

    return *this;
}


Bitarray Bitarray::operator>>(const uint32_t rhs) const{
    uint32_t full = rhs / 8u;
    uint32_t part = rhs % 8u;

    Bitarray res({});
    res.buf.resize(buf.size(), 0);

    for(uint32_t pos = full; pos < buf.size(); pos++){
        res.buf[pos - full] = buf[pos];
    }

    for(uint32_t i = 0; i < buf.size(); i++){
        res.buf[i] >>= part;
        if(i + 1 < buf.size()){
            res.buf[i] |= (res.buf[i + 1] & uint8_t(std::pow(2u, part) - 1)) << (8 - part);
        }
    }

    return res;
}

Bitarray& Bitarray::operator>>=(const uint32_t rhs){
    uint32_t full = rhs / 8u;
    uint32_t part = rhs % 8u;

    for(uint32_t pos = full; pos < buf.size(); pos++){
        buf[pos - full] = buf[pos];
    }

    for(uint32_t i = 0; i < buf.size(); i++){
        buf[i] >>= part;
        if(i + 1 < buf.size()){
            buf[i] |= (buf[i + 1] & uint8_t(std::pow(2u, part) - 1)) << (8 - part);
        }
    }

    return *this;
}

std::vector<uint8_t> Bitarray::get() const {
    return buf;
}

template<>
uint8_t Bitarray::as() const {
    return buf[0];
}

template<>
uint16_t Bitarray::as() const {
    return *(uint16_t*)&buf[0];
}

template<>
uint32_t Bitarray::as() const {
    return *(uint32_t*)&buf[0];
}

template<>
uint64_t Bitarray::as() const {
    return *(uint64_t*)&buf[0];
}

template<>
int8_t Bitarray::as() const {
    return (int8_t)buf[0];
}

template<>
int16_t Bitarray::as() const {
    return *(int16_t*)&buf[0];
}

template<>
int32_t Bitarray::as() const {
    return *(int32_t*)&buf[0];
}

template<>
int64_t Bitarray::as() const {
    return *(int64_t*)&buf[0];
}

