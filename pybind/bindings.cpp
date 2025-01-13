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
        .def("access", [](udpcan::MessageWrapper<T>& self){
            T t;
            self.access([&t](const T& m){
                t = m;
            });
            return t;
        })
        .def("update", [](udpcan::MessageWrapper<T>& self, const T& t){
            self.update([&t](T& m){
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

    py::class_<std::map<std::string, std::any>>(m, "AnyMap", py::module_local())
        .def(py::init<>());

    py::class_<udpcan::internal::Bitarray, std::shared_ptr<udpcan::internal::Bitarray> >(m, "Bitarray", py::module_local())
        .def(py::init<uint32_t>());

    py::class_<udpcan::internal::CanDatabase, std::shared_ptr<udpcan::internal::CanDatabase> >(m, "CanDatabase", py::module_local())
        .def(py::init<>())
        .def_readwrite("dbc_version", &udpcan::internal::CanDatabase::dbc_version)
        .def("encode", &udpcan::internal::CanDatabase::encode);

    py::class_<udpcan::RemoteControl, std::shared_ptr<udpcan::RemoteControl> >(m, "RemoteControl", py::module_local())
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

    declare_msgwrap<udpcan::RemoteControl>(m, "RemoteControl");

    py::class_<udpcan::NetworkHandler, std::shared_ptr<udpcan::NetworkHandler> >(m, "NetworkHandler", py::module_local())
        .def(py::init<>())
        .def("parse", &udpcan::NetworkHandler::parse)
        .def("init", &udpcan::NetworkHandler::init)
        .def("reset", &udpcan::NetworkHandler::reset)
        .def("close", &udpcan::NetworkHandler::close)
        .def("start", &udpcan::NetworkHandler::start)
        .def("stop", &udpcan::NetworkHandler::stop)
        .def("flush", &udpcan::NetworkHandler::flush)
        .def("getRemoteControl", [](udpcan::NetworkHandler& self){return self.get<udpcan::RemoteControl>();})
        .def("pushRemoteControl", [](udpcan::NetworkHandler& self){return self.push<udpcan::RemoteControl>();});
}