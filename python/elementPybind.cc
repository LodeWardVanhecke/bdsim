//
// Created by Stewart Boogert on 09/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "element.h"

PYBIND11_MODULE(Element, m) {
  py::class_<GMAD::Element>(m,"Element");
}