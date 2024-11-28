#include "can.hpp"

bool udpcan::internal::isValidString(const char c){
    return std::isalpha(c) || c == '_' || c == '+' || c == '-';
}

uint32_t udpcan::internal::openRead(const std::string& fn, uint64_t& end, std::ifstream& in){
    try{
        in = std::ifstream(fn, std::ios::ate | std::ios::binary);

        if(!in.is_open()){
            return CAN_E_FILE_OPEN_ERRNO;
        }

        if(!in.good()){
            in.close();
            return CAN_E_FILE_FS_MALFORM;
        }

        end = in.tellg();
        in.seekg(std::ios::beg);
        return CAN_E_SUCCESS;
    }
    catch(...){
        return CAN_E_FILE_READ_ERRNO;
    }
}

std::string udpcan::internal::readUntil(std::ifstream& in, const uint64_t eof, const char delim){
    uint64_t start = in.tellg();
    uint64_t pos = start;

    while (pos < eof){
        in.seekg(pos);
        if(delim == in.peek()){
            break;
        }
        pos++;
    }
    in.seekg(start);

    std::string res;
    res.resize(pos - in.tellg());
    in.read(&res[0], res.size());
    in.seekg(pos + 1);
    return res;
}

uint32_t udpcan::internal::seekUntil(std::ifstream& in, const uint64_t eof, const std::string& pattern){
    uint64_t start = in.tellg();
    uint64_t pos = start;

    bool match = false;
    while(pos + pattern.size() < eof){
        match = true;
        for(uint32_t i = 0; i < pattern.size(); i++){
            in.seekg(pos + i);
            match = in.peek() == pattern[i];
            if(!match) break;
        }
        if(match){
            in.seekg(pos);
            break;
        }
        pos++;
    }

    if(pos + pattern.size() >= eof) return CAN_E_I_EOF;
    
    return CAN_E_SUCCESS;
}

uint32_t udpcan::internal::readNextNumeric(std::ifstream& in, const uint64_t eof, uint32_t& value){
    uint64_t start = in.tellg();
    uint64_t pos = start;

    bool beg = false;
    bool end = false;

    while(pos < eof){
        in.seekg(pos);
        if(!beg && std::isdigit(in.peek())){
            beg = true;
            start = pos;
        }
        else if(beg && !std::isdigit(in.peek()) && !end){
            end = true;
            break;
        }
        pos++;
    }

    if(!beg && !end) return CAN_E_I_EOF;
    
    try{
        std::string num;
        num.resize(pos - start);
        in.seekg(start);
        in.read(&num[0], num.size());
        value = std::stoi(num);
        return CAN_E_SUCCESS;
    }
    catch(...){
        return CAN_E_FILE_NUM_READ_ERRNO;
    }
}

uint32_t udpcan::internal::readNextFloating(std::ifstream& in, const uint64_t eof, float& value){
    uint64_t start = in.tellg();
    uint64_t pos = start;

    bool beg = false;
    bool end = false;

    while(pos < eof){
        in.seekg(pos);
        if((std::isdigit(in.peek()) || in.peek() == '.') && !beg){
            beg = true;
            start = pos;
        }
        else if(beg && !std::isdigit(in.peek()) && in.peek() != '.' && !end){
            end = true;
            break;
        }
        pos++;
    }

    if(!beg && !end) return CAN_E_I_EOF;
    
    try{
        std::string num;
        num.resize(pos - start);
        in.seekg(start);
        in.read(&num[0], num.size());
        value = std::stof(num);
        return CAN_E_SUCCESS;
    }
    catch(...){
        return CAN_E_FILE_NUM_READ_ERRNO;
    }
}

uint32_t udpcan::internal::readNextString(std::ifstream& in, const uint64_t eof, std::string& value){
    uint64_t start = in.tellg();
    uint64_t pos = start;

    bool beg = false;
    bool end = false;

    while(pos < eof){
        in.seekg(pos);
        if(!beg && isValidString(in.peek())){
            beg = true;
            start = pos;
        }
        else if(beg && !isValidString(in.peek()) && !end){
            end = true;
            break;
        }
        pos++;
    }

    if(!beg && !end) return CAN_E_I_EOF;
    
    value.resize(pos - start);
    in.seekg(start);
    in.read(&value[0], value.size());
    return CAN_E_SUCCESS;
}