#include "can.hpp"

using namespace udpcan::internal;

CanDatabase::CanDatabase():dbc_version(0),messages({}){

}

CanDatabase::~CanDatabase(){
    messages.clear();
}

uint32_t CanDatabase::read(const std::string& fn, uint64_t& end, std::ifstream& in) const{
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

    if(beg >= end){
        return CAN_E_FILE_NO_DBC_VERSION;
    }

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

    uint32_t res = read(fn, eof, in);
    if(res != CAN_E_SUCCESS){
        return res;
    }

    std::string version_str;
    version_str.resize(eof);
    in.getline(&version_str[0], eof, '\n');
    res = validateDBCVersion(std::string(version_str.data()));
    if(res != CAN_E_SUCCESS){
        return res;
    }
    version_str.clear();

    

    in.close();
    return CAN_E_SUCCESS;
}