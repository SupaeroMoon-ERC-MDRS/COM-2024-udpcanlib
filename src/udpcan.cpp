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
    if(std::is_same<T,RemoteControl>::value){
        #if BURN_IN
        res = remote_msg.access([this](RemoteControl* msg){
            internal::CanMsgBytes canmsg(remote_msg.getId(), {});
            canmsg.bytes.resize(sizeof(RemoteControl));
            std::memcpy(canmsg.bytes.data(), msg, sizeof(RemoteControl));
            udp.push(canmsg);
        });
        #else
        #endif
    }
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
            #if BURN_IN
            if(msg.id == remote_msg.getId()){
                uint32_t res = remote_msg.update([&msg](RemoteControl* remote){
                    std::memcpy(remote, msg.bytes.data(), sizeof(RemoteControl));
                });
            }
            #else
            std::map<std::string, int64_t> out_int = {};
            std::map<std::string, float> out_float = {};
            msg.bytes.insert(msg.bytes.begin(), database.dbc_version & 0xFF);
            msg.bytes.insert(msg.bytes.begin(), (database.dbc_version >> 8) & 0xFF);
            msg.bytes.insert(msg.bytes.begin(), msg.id);
            uint32_t res = database.decode(msg.bytes, out_int, out_float);
            if(res != CAN_E_SUCCESS){
                continue;
            }

            if(msg.id == remote_msg.getId()){
                res = remote_msg.update([&out_float, &out_int](RemoteControl* remote){
                    remote->l_top = out_float["l_top"];
                    remote->l_bottom = out_float["l_bottom"];
                    remote->l_right = out_float["l_right"];
                    remote->l_left = out_float["l_left"];
                    remote->r_top = out_float["r_top"];
                    remote->r_bottom = out_float["r_bottom"];
                    remote->r_right = out_float["r_right"];
                    remote->r_left = out_float["r_left"];
                    remote->l_shoulder = out_float["l_shoulder"];
                    remote->r_shoulder = out_float["r_shoulder"];
                    remote->e_stop = out_float["e_stop"];
                    remote->left_trigger = out_float["left_trigger"];
                    remote->right_trigger = out_float["right_trigger"];
                    remote->thumb_left_x = out_float["thumb_left_x"];
                    remote->thumb_left_y = out_float["thumb_left_y"];
                    remote->thumb_right_x = out_float["thumb_right_x"];
                    remote->thumb_right_y = out_float["thumb_right_y"];
                });
            }
            #endif
        }
    }
}