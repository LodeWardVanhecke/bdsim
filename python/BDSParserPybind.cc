//
// Created by Stewart Boogert on 15/03/2023.
//


#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "BDSParser.hh"

PYBIND11_MODULE(bdsparser, m) {
  py::class_<BDSParser>(m,"BDSParser")
    .def_static("Instance",[](std::string fileName) {return BDSParser::Instance(fileName);})
    .def_static("Instance",[]() {return BDSParser::Instance();})
    .def_static("IsInitialised",&BDSParser::IsInitialised)
    .def("GetOptions",&BDSParser::GetOptions)
    .def("GetOptionsBase",&BDSParser::GetOptionsBase);
    //.def("GetBeam",&BDSParser::GetBeam);
}
