//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "samplerplacement.h"

PYBIND11_MODULE(samplerplacement, m) {
  py::class_<GMAD::Published<GMAD::SamplerPlacement>>(m,"PublishedSamplerPlacement")
    .def("NameExists",&GMAD::SamplerPlacement::NameExists);

  py::class_<GMAD::SamplerPlacement>(m,"SamplerPlacement")
    .def (py::init<>());
}