#include "net.hpp"

using namespace udpcan::internal;

CanMsgBytes::CanMsgBytes(const uint8_t id, const std::vector<uint8_t> bytes):id(id),bytes(bytes){

}

CanMsgBytes::~CanMsgBytes(){
    bytes.clear();
}

UDP::UDP():initialized(false),need_reset(false),expect_dbc_version(0){

}

UDP::~UDP(){
    buf.clear();
    outbuf.clear();
    messages.clear();
}