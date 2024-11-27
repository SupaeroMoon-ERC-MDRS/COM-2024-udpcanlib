#include "can.hpp"

using namespace udpcan::internal;

CanDatabase::~CanDatabase(){
    messages.clear();
}

uint32_t CanDatabase::read(const std::string& fn, size_t& end, std::ifstream& in) const{
    try{
        in = std::ifstream(fn, std::ios::in | std::ios::binary);

        if(!in.is_open()){
            return CAN_E_FILE_OPEN;
        }

        if(!in.good()){
            in.close();
            return CAN_E_FILE_FS_MALFORM;
        }

        in.seekg(std::ios::end);
        end = in.tellg();
        in.seekg(std::ios::beg);
        return CAN_E_SUCCESS;
    }
    catch(...){
        return CAN_E_FILE_READ_ERRNO;
    }
}

uint32_t CanDatabase::parse(const std::string& fn){
    size_t eof;
    std::ifstream in;

    uint32_t res = read(fn, eof, in);
    if(res != CAN_E_SUCCESS){
        return res;
    }


    in.close();
}