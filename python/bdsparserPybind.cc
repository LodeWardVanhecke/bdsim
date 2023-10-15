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
    .def(py::init<>())
    .def_static("Instance",[](std::string fileName) {return BDSParser::Instance(fileName);})
    .def_static("Instance",[]() {return BDSParser::Instance();})
    .def_static("IsInitialised",&BDSParser::IsInitialised)
    .def("GetOptions",&BDSParser::GetOptions)
    .def("GetOptionsBase",&BDSParser::GetOptionsBase)
    .def("GetBeam",[](BDSParser &bdsp) {return &bdsp.GetBeam();})
    .def("GetBeamBase",&BDSParser::GetBeamBase)
    .def("GetBeamline",[](BDSParser &bdsp) {return &bdsp.GetBeamline();})
    .def("GetSamplerFilterIDToSet",[](BDSParser &bdsp) {return bdsp.GetSamplerFilterIDToSet();})
    .def("GetSequence",&BDSParser::GetSequence)
    .def("GetElement", [](BDSParser &bdsp, const std::string& name) { return bdsp.GetElement(name);});
}
