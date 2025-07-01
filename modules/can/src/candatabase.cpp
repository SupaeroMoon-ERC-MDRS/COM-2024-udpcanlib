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

// TODO check if we really need this
std::vector<std::pair<uint8_t, uint32_t>> CanDatabase::getMessageSizes() const{
    std::vector<std::pair<uint8_t, uint32_t>> ret;
    ret.reserve(messages.size());

    for(auto it : messages){
        ret.push_back({it.first, it.second.message_length});
    }
    return ret;
}

uint32_t CanDatabase::decode(const std::vector<uint8_t>& message_all, std::vector<std::pair<uint8_t, std::map<std::string, std::any>>>& out) const{
    uint32_t pos = 0;
    out.clear();

    while(pos + 2u < message_all.size()){ // 1 msg id +1 min msg size = 2u        
        uint8_t msg_id = message_all[pos];
        if(messages.find(msg_id) == messages.cend()){
            return CAN_E_UNKNOWN_MSG_ID;
        }
        pos += 1;

        uint32_t msg_size_min = messages.at(msg_id).message_length;
        if(pos + msg_size_min > message_all.size()){
            return CAN_E_PARTIAL_MSG;
        }

        uint32_t msg_size;
        std::map<std::string, std::any> msg_out;
        messages.at(msg_id).decode(std::vector<uint8_t>(message_all.cbegin() + pos, message_all.cend()), msg_out, msg_size);
        out.push_back({msg_id, msg_out});
        pos += msg_size;
    }
    return CAN_E_SUCCESS;
}

uint32_t CanDatabase::encode(const uint8_t id, const std::map<std::string, std::any>& in, std::vector<uint8_t>& all_out) const {
    std::set<std::string> msg_keys;
    std::set<std::string> in_keys;

    std::cout << "1" << std::endl;
    messages.at(id).getSignalNames(msg_keys);
    std::cout << "2" << std::endl;
    for(const std::pair<std::string, std::any> p : in){
        in_keys.insert(p.first);
    }

    if(msg_keys == in_keys){
        uint32_t res;
        std::cout << "3" << std::endl;
        CAN_E_FW_IF_ERR(messages.at(id).encode(in, all_out, dbc_version));
        std::cout << "4" << std::endl;
        return res;
    }
    else{
        return CAN_E_I_SIGNAMES;
    }
}

std::vector<std::string> CanDatabase::getSignalNames() const {
    std::vector<std::string> ret;

    for(const std::pair<uint8_t, CanMessageDesc> msg : messages){
        std::set<std::string> msg_keys;
        msg.second.getSignalNames(msg_keys);
        std::copy(msg_keys.cbegin(), msg_keys.cend(), std::back_inserter(ret));
    }

    return ret;
}

std::map<std::string, ENumType> CanDatabase::getSignalTypes() const {
    std::map<std::string, ENumType> ret;

    for(const std::pair<uint8_t, CanMessageDesc> msg : messages){
        for(const std::pair<std::string, ENumType> sig : msg.second.getSignalTypes()){
            ret[sig.first] = sig.second;
        }
    }

    return ret;
}