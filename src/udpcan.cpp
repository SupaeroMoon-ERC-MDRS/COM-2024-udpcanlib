#include "udpcan.hpp"

using namespace udpcan;

uint32_t NetworkHandler::parse(const std::string& fn){
    return database.parse(fn);
}

uint32_t NetworkHandler::init(){
    return udp.init(database.dbc_version);
}

uint32_t NetworkHandler::reset(){
    return udp.reset(database.dbc_version);
}

uint32_t NetworkHandler::close(){
    return udp.close();
}

template<typename T>
MessageWrapper<T>* NetworkHandler::get(){
    if(std::is_same<T,RemoteControl>::value){
        return &remote_msg;
    }
}

template<typename T>
uint32_t NetworkHandler::push(){
    uint32_t res = CAN_E_I_NO_SUCH_MSG;
    PUSH_MSG(RemoteControl, remote_msg)
    return res;
}

uint32_t NetworkHandler::flush(){
    return udp.flush();
}

uint32_t NetworkHandler::start(){
    if(thr.joinable()){
		return CAN_E_THREAD_ALREADY_RUNNING;
	}

    stop_thr = false;
    thr = std::thread([this](){thread();});
    
	if(thr.joinable()){
		return CAN_E_SUCCESS;
	}
	else{
		return CAN_E_THREAD_LAUNCH_FAIL;
	}
}

uint32_t NetworkHandler::stop(){
	stop_thr = true;
	{
		std::unique_lock<std::mutex> lk(thr_mtx);
        thr.join();
	}
    return CAN_E_SUCCESS;
}

void NetworkHandler::thread(){
    while (!stop_thr){
		std::unique_lock<std::mutex> lk(thr_mtx);

        std::vector<internal::CanMsgBytes> msgs;
        if(udp.isInitialized() && !udp.needReset()){
            udp.recv();
            udp.getMessages(msgs);
        }
        else{
            udp.reset(database.dbc_version);
            continue;
        }

        for(internal::CanMsgBytes msg : msgs){
            std::map<std::string, std::any> out = {};
            uint32_t res = database.decode(msg.all_bytes, out);
            if(res != CAN_E_SUCCESS){
                continue;
            }

            if(msg.id == remote_msg.getId()){
                res = remote_msg.update([&out](RemoteControl* remote){
                    remote->updateFrom(out);
                });
            }
        }
    }
}