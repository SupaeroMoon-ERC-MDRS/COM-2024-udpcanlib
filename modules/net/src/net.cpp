#include "net.hpp"

using namespace udpcan::internal;

CanMsgBytes::CanMsgBytes(const uint8_t id, const std::vector<uint8_t> all_bytes):id(id),all_bytes(all_bytes){

}

CanMsgBytes::~CanMsgBytes(){
    all_bytes.clear();
}

UDP::UDP():initialized(false),need_reset(false),expect_dbc_version(0){

}

UDP::~UDP(){
    buf.clear();
    outbuf.clear();
    messages.clear();
}