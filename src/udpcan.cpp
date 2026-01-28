#include "udpcan.hpp"

using namespace udpcan;

NetworkHandler::NetworkHandler():
    remote_msg(15),
    raspi_state(30),
    nav_arm(41),
    nav_loco(40),
    servo_calib_state(16)
{
}

std::string NetworkHandler::getWlanIp(){
    char buffer[1000];
    std::string res;
    std::string ip;
    FILE* f = popen("ip a", "r");
    while (fgets(buffer, 1000, f) != nullptr) {
        res += buffer;
    }
    pclose(f);

    char inet[6] = "inet ";
    size_t inetpos = res.find(inet, 0);
    while(inetpos != std::string::npos){
        size_t slash = res.find('/', inetpos);
        std::string maybeIp = res.substr(inetpos + 5, slash - inetpos - 5);
        if(maybeIp.starts_with(subnet)){
            ip = maybeIp;
            break;
        }

        inetpos = res.find(inet, slash);
    }

    return ip;
}

uint32_t NetworkHandler::parse(const std::string& fn){
    return database.parse(fn);
}

uint32_t NetworkHandler::init(const int32_t type){
    return udp.init(database.dbc_version, getWlanIp(), UDPCAN_PORT, static_cast<NodeType>(type));
}

uint32_t NetworkHandler::reset(){
    return udp.reset(database.dbc_version, UDPCAN_PORT);
}

uint32_t NetworkHandler::close(){
    return udp.shutdown();
}

uint32_t NetworkHandler::flush(){
    return udp.flush();
}

uint32_t NetworkHandler::start(){
    if(!udp.isInitialized()){
        return NET_E_UNINITIALIZED;
    }

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
    return udp.shutdown();
}

void NetworkHandler::thread(){
    using namespace std::chrono_literals;
    while (!stop_thr){
        std::this_thread::sleep_for(1ms);
		std::unique_lock<std::mutex> lk(thr_mtx);

        std::vector<RecvPacket> packets;
        if(udp.isInitialized() && !udp.needReset()){
            udp.tryConnectRemote();
            udp.recv();
            udp.getPackets(packets);
        }
        else{
            udp.reset(database.dbc_version, UDPCAN_PORT);
            continue;
        }

        for(const RecvPacket& packet : packets){
            std::vector<std::pair<uint8_t, std::map<std::string, std::any>>> out = {};
            uint32_t res = database.decode(packet.buf, out);
            if(res != CAN_E_SUCCESS){
                continue;
            }

            for(const std::pair<uint8_t, std::map<std::string, std::any>> &p : out){
                if(p.first == remote_msg.getId()){
                    res = remote_msg.update([&p](RemoteControl& remote){
                        remote.updateFrom(p.second);
                    });
                }
                else if(p.first == raspi_state.getId()){
                    res = raspi_state.update([&p](RaspiState& state){
                        state.updateFrom(p.second);
                    });
                }                
                else if(p.first == nav_arm.getId()){
                    res = nav_arm.update([&p](NavArm& arm){
                        arm.updateFrom(p.second);
                    });
                }                  
                else if(p.first == nav_loco.getId()){
                    res = nav_loco.update([&p](NavLocomotion& loco){
                        loco.updateFrom(p.second);
                    });
                }                 
                else if(p.first == servo_calib_state.getId()){
                    res = servo_calib_state.update([&p](ServoCalibState& servo_calb){
                        servo_calb.updateFrom(p.second);
                    });
                }
            }
        }

        /*remote_msg.update([](RemoteControl& rem){
            rem.left_trigger = (rem.left_trigger + 1) % 256;
            rem.right_trigger = (rem.right_trigger - 1) % 256;
        });*/
    }
}