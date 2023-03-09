#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parser.h"

PYBIND11_MODULE(GMAD, m) {
    py::class_<GMAD::Parser>(m,"Parser")
      .def_static("Instance",[](std::string fileName) {return GMAD::Parser::Instance(fileName);})
      .def_static("Instance",[](){return GMAD::Parser::Instance();});
}