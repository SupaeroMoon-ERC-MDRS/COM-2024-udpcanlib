#include "can.hpp"

using namespace udpcan::internal;

CanDatabase::CanDatabase():messages({}),dbc_version(0){

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

uint32_t CanDatabase::decode(const Bitarray& message_all_bits, std::map<std::string, int64_t>& out_int, std::map<std::string, float>& out_float) const{
    std::vector<uint8_t> bytes = message_all_bits.get();
    uint32_t pos = 0;
    out_int.clear();
    out_float.clear();

    while(pos + 4u < bytes.size()){ // 2 dbc version 1 msg id +1 min msg size = 4u
        uint16_t msg_version = *(uint16_t*)(bytes.data() + pos);
        if(msg_version != dbc_version){
            return CAN_E_WRONG_DBC_VERSION;
        }
        pos += 2;
        
        uint8_t msg_id = bytes[pos];
        if(messages.find(msg_id) == messages.cend()){
            return CAN_E_UNKNOWN_MSG_ID;
        }
        pos += 1;

        uint32_t msg_size = messages.at(msg_id).message_length;
        if(pos + msg_size < bytes.size()){
            return CAN_E_PARTIAL_MSG;
        }

        Bitarray msg_payload = Bitarray(std::vector<uint8_t>(bytes.cbegin() + pos, bytes.cbegin() + pos + msg_size));
        messages.at(msg_id).decode(msg_payload, out_int, out_float);
        pos += msg_size;
    }
    return CAN_E_SUCCESS;
}