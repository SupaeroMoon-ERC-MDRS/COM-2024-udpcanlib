#include "can.hpp"

using namespace udpcan::internal;

int32_t main(){
    CanDatabase db;
    uint32_t res = db.parse("/home/davidgmolnar/Documents/COM-2024/COM-2024-DBC/comms.dbc");
    return 0;
};