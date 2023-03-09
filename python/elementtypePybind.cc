#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "elementtype.h"

PYBIND11_MODULE(ElementType, m) {
  py::enum_<GMAD::ElementType>(m,"ElementType");

}



