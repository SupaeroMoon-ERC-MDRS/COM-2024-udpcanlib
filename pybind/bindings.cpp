#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/complex.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include <pybind11/attr.h>

#include "udpcan.hpp"

namespace py = pybind11;

template<typename T>
void declare_msgwrap(py::module& m, const std::string& prefix){
    py::class_<udpcan::MessageWrapper<T>>(m, (prefix + std::string("Wrapper")).c_str(), py::module_local())
        .def(py::init<uint8_t>())
        .def("access", [](udpcan::MessageWrapper<T>& self, T& t){
            return self.access([&t](const T& m){
                t = m;
            });
        })
        .def("update", [](udpcan::MessageWrapper<T>& self, const T& t){
            return self.update([&t](T& m){
                m = t;
            });
        })
        .def("hasUpdate", [](udpcan::MessageWrapper<T>& self){
            bool b;
            self.hasUpdate(b);
            return b;
        });
}

PYBIND11_MODULE(udpcanpy, m){
    py::class_<udpcan::RemoteControl, std::shared_ptr<udpcan::RemoteControl> >(m, "RemoteControl", py::module_local())
        .def(py::init<>())
        .def_readwrite("l_top", &udpcan::RemoteControl::l_top)
        .def_readwrite("l_bottom", &udpcan::RemoteControl::l_bottom)
        .def_readwrite("l_right", &udpcan::RemoteControl::l_right)
        .def_readwrite("l_left", &udpcan::RemoteControl::l_left)
        .def_readwrite("r_top", &udpcan::RemoteControl::r_top)
        .def_readwrite("r_bottom", &udpcan::RemoteControl::r_bottom)
        .def_readwrite("r_right", &udpcan::RemoteControl::r_right)
        .def_readwrite("r_left", &udpcan::RemoteControl::r_left)
        .def_readwrite("l_shoulder", &udpcan::RemoteControl::l_shoulder)
        .def_readwrite("r_shoulder", &udpcan::RemoteControl::r_shoulder)
        .def_readwrite("e_stop", &udpcan::RemoteControl::e_stop)
        .def_readwrite("left_trigger", &udpcan::RemoteControl::left_trigger)
        .def_readwrite("right_trigger", &udpcan::RemoteControl::right_trigger)
        .def_readwrite("thumb_left_x", &udpcan::RemoteControl::thumb_left_x)
        .def_readwrite("thumb_left_y", &udpcan::RemoteControl::thumb_left_y)
        .def_readwrite("thumb_right_x", &udpcan::RemoteControl::thumb_right_x)
        .def_readwrite("thumb_right_y", &udpcan::RemoteControl::thumb_right_y);

    py::class_<udpcan::RaspiState, std::shared_ptr<udpcan::RaspiState> >(m, "RaspiState", py::module_local())
        .def(py::init<>())
        .def_readwrite("rpi_3v7_wl_sw_a", &udpcan::RaspiState::rpi_3v7_wl_sw_a)
        .def_readwrite("rpi_3v3_sys_a", &udpcan::RaspiState::rpi_3v3_sys_a)
        .def_readwrite("rpi_1v8_sys_a", &udpcan::RaspiState::rpi_1v8_sys_a)
        .def_readwrite("rpi_1v1_sys_a", &udpcan::RaspiState::rpi_1v1_sys_a)
        .def_readwrite("rpi_0v8_sw_a", &udpcan::RaspiState::rpi_0v8_sw_a)
        .def_readwrite("vdd_core_a", &udpcan::RaspiState::vdd_core_a)
        .def_readwrite("rpi_3v7_wl_sw_v", &udpcan::RaspiState::rpi_3v7_wl_sw_v)
        .def_readwrite("rpi_3v3_sys_v", &udpcan::RaspiState::rpi_3v3_sys_v)
        .def_readwrite("rpi_1v8_sys_v", &udpcan::RaspiState::rpi_1v8_sys_v)
        .def_readwrite("rpi_1v1_sys_v", &udpcan::RaspiState::rpi_1v1_sys_v)
        .def_readwrite("rpi_0v8_sw_v", &udpcan::RaspiState::rpi_0v8_sw_v)
        .def_readwrite("vdd_core_v", &udpcan::RaspiState::vdd_core_v)
        .def_readwrite("rpi_ext5v_v", &udpcan::RaspiState::rpi_ext5v_v)
        .def_readwrite("rpi_temp", &udpcan::RaspiState::rpi_temp)
        .def_readwrite("rpi_cpu", &udpcan::RaspiState::rpi_cpu)
        .def_readwrite("rpi_mem", &udpcan::RaspiState::rpi_mem)
        .def_readwrite("rpi_rssi", &udpcan::RaspiState::rpi_rssi)
        .def_readwrite("rpi_ina_voltage", &udpcan::RaspiState::rpi_ina_voltage)
        .def_readwrite("rpi_ina_current", &udpcan::RaspiState::rpi_ina_current);

    py::class_<udpcan::ServoCalibState, std::shared_ptr<udpcan::ServoCalibState> >(m, "ServoCalibState", py::module_local())
        .def(py::init<>())
        .def_readwrite("fl_calib", &udpcan::ServoCalibState::fl_calib)
        .def_readwrite("fr_calib", &udpcan::ServoCalibState::fr_calib)
        .def_readwrite("rl_calib", &udpcan::ServoCalibState::rl_calib)
        .def_readwrite("rr_calib", &udpcan::ServoCalibState::rr_calib);

    py::class_<udpcan::NavArm, std::shared_ptr<udpcan::NavArm> >(m, "NavArm", py::module_local())
        .def(py::init<>())
        .def_readwrite("arm_active", &udpcan::NavArm::arm_active)
        .def_readwrite("joint_0", &udpcan::NavArm::joint_0)
        .def_readwrite("joint_1", &udpcan::NavArm::joint_1)
        .def_readwrite("joint_2", &udpcan::NavArm::joint_2)
        .def_readwrite("joint_3", &udpcan::NavArm::joint_3);


    py::class_<udpcan::NavLocomotion, std::shared_ptr<udpcan::NavLocomotion> >(m, "NavLocomotion", py::module_local())
        .def(py::init<>())
        .def_readwrite("odometry", &udpcan::NavLocomotion::odometry)
        .def_readwrite("motor_fl_target", &udpcan::NavLocomotion::motor_fl_target)
        .def_readwrite("motor_fr_target", &udpcan::NavLocomotion::motor_fr_target)
        .def_readwrite("motor_ml_target", &udpcan::NavLocomotion::motor_ml_target)
        .def_readwrite("motor_mr_target", &udpcan::NavLocomotion::motor_mr_target)
        .def_readwrite("motor_rl_target", &udpcan::NavLocomotion::motor_rl_target)
        .def_readwrite("motor_rr_target", &udpcan::NavLocomotion::motor_rr_target);

    py::class_<udpcan::ScienceWeight, std::shared_ptr<udpcan::ScienceWeight> >(m, "ScienceWeight", py::module_local())
        .def(py::init<>())
        .def_readwrite("scale_meas", &udpcan::ScienceWeight::scale_meas)
        .def_readwrite("scale_valid", &udpcan::ScienceWeight::scale_valid);

    declare_msgwrap<udpcan::RemoteControl>(m, "RemoteControl");
    declare_msgwrap<udpcan::RaspiState>(m, "RaspiState");
    declare_msgwrap<udpcan::NavArm>(m, "NavArm");
    declare_msgwrap<udpcan::NavLocomotion>(m, "NavLocomotion");
    declare_msgwrap<udpcan::ServoCalibState>(m, "ServoCalibState");
    declare_msgwrap<udpcan::ScienceWeight>(m, "ScienceWeight");

    py::class_<udpcan::NetworkHandler, std::shared_ptr<udpcan::NetworkHandler> >(m, "NetworkHandler", py::module_local())
        .def(py::init<>())
        .def("setSubnet", &udpcan::NetworkHandler::setSubnet)
        .def("parse", &udpcan::NetworkHandler::parse)
        .def("init", &udpcan::NetworkHandler::init)
        .def("reset", &udpcan::NetworkHandler::reset)
        .def("close", &udpcan::NetworkHandler::close)
        .def("start", &udpcan::NetworkHandler::start)
        .def("stop", &udpcan::NetworkHandler::stop)
        .def("flush", &udpcan::NetworkHandler::flush)
        .def("getRemoteControl", [](udpcan::NetworkHandler& self){return self.get<udpcan::RemoteControl>();})
        .def("pushRemoteControl", [](udpcan::NetworkHandler& self){return self.push<udpcan::RemoteControl>();})
        .def("getRaspiState", [](udpcan::NetworkHandler& self){return self.get<udpcan::RaspiState>();})
        .def("pushRaspiState", [](udpcan::NetworkHandler& self){return self.push<udpcan::RaspiState>();})
        .def("getNavArm", [](udpcan::NetworkHandler& self){return self.get<udpcan::NavArm>();})
        .def("pushNavArm", [](udpcan::NetworkHandler& self){return self.push<udpcan::NavArm>();})
        .def("getNavLocomotion", [](udpcan::NetworkHandler& self){return self.get<udpcan::NavLocomotion>();})
        .def("pushNavLocomotion", [](udpcan::NetworkHandler& self){return self.push<udpcan::NavLocomotion>();})
        .def("getServoCalibState", [](udpcan::NetworkHandler& self){return self.get<udpcan::ServoCalibState>();})
        .def("pushServoCalibState", [](udpcan::NetworkHandler& self){return self.push<udpcan::ServoCalibState>();})
        .def("getScienceWeight", [](udpcan::NetworkHandler& self){return self.get<udpcan::ScienceWeight>();})
        .def("pushScienceWeight", [](udpcan::NetworkHandler& self){return self.push<udpcan::ScienceWeight>();});
}