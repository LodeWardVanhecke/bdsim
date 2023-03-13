//
// Created by Stewart Boogert on 11/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "aperture.h"

PYBIND11_MODULE(aperture, m) {
    py::class_<GMAD::Published<GMAD::Aperture>>(m,"PublishedAperture")
    .def("NameExists",&GMAD::Aperture::NameExists);

    py::class_<GMAD::Aperture, GMAD::Published<GMAD::Aperture>>(m,"Aperture")
    .def(py::init<>())
    .def_readwrite("name", &GMAD::Aperture::name)
    .def_readwrite("apertureType", &GMAD::Aperture::apertureType)
    .def_readwrite("aper1", &GMAD::Aperture::aper1)
    .def_readwrite("aper2", &GMAD::Aperture::aper2)
    .def_readwrite("aper3", &GMAD::Aperture::aper3)
    .def_readwrite("aper4", &GMAD::Aperture::aper4)
    .def("clear",&GMAD::Aperture::clear)
    .def("print",&GMAD::Aperture::print)
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,std::string value) {aperture.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,int value) {aperture.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,bool value) {aperture.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,long int value) {aperture.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,double value) {aperture.set_value<double>(name,value);});
}