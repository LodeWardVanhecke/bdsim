#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "element.h"
#include "fastlist.h"

PYBIND11_MODULE(fastlist, m) {
  py::class_<GMAD::FastList<GMAD::Element>>(m,"FastList_Element");
}