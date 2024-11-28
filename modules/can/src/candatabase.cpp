#include "can.hpp"

using namespace udpcan::internal;

CanDatabase::CanDatabase():dbc_version(0),messages({}){

}

CanDatabase::~CanDatabase(){
    messages.clear();
}

uint32_t CanDatabase::validateDBCVersion(const std::string& v){
    if(std::string(v.cbegin(), v.cbegin() + 7) != std::string("VERSION")){
        return CAN_E_FILE_NO_DBC_VERSION;
    }

    uint32_t beg = 7;
    uint32_t end = v.size();

    while(beg < end){
        if(!std::isdigit(v[beg])){
            beg++;
        }
        else if(!std::isdigit(v[end - 1])){
            end--;
        }
        else{
            break;
        }
    }

    if(beg >= end) return CAN_E_FILE_NO_DBC_VERSION;

    try{
        dbc_version = std::stoi(std::string(v.cbegin() + beg, v.cbegin() + end));
        return CAN_E_SUCCESS;
    }
    catch(...){
        return CAN_E_FILE_DBC_VERSION_ERRNO;
    }
}

uint32_t CanDatabase::parse(const std::string& fn){
    uint64_t eof;
    std::ifstream in;

    uint32_t res;
    CAN_E_FW_IF_ERR(openRead(fn, eof, in))
    
    std::string version_str = readUntil(in, eof, '\n');
    CAN_E_FW_IF_ERR(validateDBCVersion(std::string(version_str.data())))
    version_str.clear();

    while(true){
        res = seekUntil(in, eof, "BO_");
        if(res != CAN_E_SUCCESS){
            if(messages.empty()) return CAN_E_FILE_UNEXPECTED_EOF;
            return CAN_E_SUCCESS;
        }

        CanMessageDesc desc;
        CAN_E_FW_IF_ERR(desc.parse(in, eof))
        messages[desc.id] = desc;
    }

    in.close();
    return CAN_E_SUCCESS;
}