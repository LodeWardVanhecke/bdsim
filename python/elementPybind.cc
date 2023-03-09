//
// Created by Stewart Boogert on 09/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "element.h"
#include "elementtype.h"

PYBIND11_MODULE(element, m) {
  py::class_<GMAD::Element>(m,"Element")
    .def(py::init<>())
    .def_readwrite("type", &GMAD::Element::type)
    .def_readwrite("name", &GMAD::Element::name)
    .def_readwrite("userTypeName", &GMAD::Element::userTypeName)
    .def_readwrite("userParameters", &GMAD::Element::userParameters)
    .def_readwrite("l", &GMAD::Element::l)
    .def_readwrite("scaling", &GMAD::Element::scaling)
    .def_readwrite("scalingFieldOuter", &GMAD::Element::scalingFieldOuter)
    .def_readwrite("ks", &GMAD::Element::ks)
    .def_readwrite("k1", &GMAD::Element::k1)
    .def_readwrite("k2", &GMAD::Element::k2)
    .def_readwrite("k3", &GMAD::Element::k3)
    .def_readwrite("k4", &GMAD::Element::k4)
    .def_readwrite("angle", &GMAD::Element::angle)
    .def_readwrite("B", &GMAD::Element::B)
    .def_readwrite("e1", &GMAD::Element::e1)
    .def_readwrite("e2", &GMAD::Element::e2)
    .def_readwrite("fint", &GMAD::Element::fint)
    .def_readwrite("fintx", &GMAD::Element::fintx)
    .def_readwrite("fintK2", &GMAD::Element::fintK2)
    .def_readwrite("hgap", &GMAD::Element::hgap)
    .def_readwrite("h1", &GMAD::Element::h1)
    .def_readwrite("h2", &GMAD::Element::h2)
    .def_readwrite("kick", &GMAD::Element::kick)
    .def_readwrite("hkick", &GMAD::Element::hkick)
    .def_readwrite("vkick", &GMAD::Element::vkick)
    .def_readwrite("knl", &GMAD::Element::knl)
    .def_readwrite("ksl", &GMAD::Element::ksl)
    .def_readwrite("gradient", &GMAD::Element::gradient)
    .def_readwrite("E", &GMAD::Element::E)
    .def_readwrite("frequency", &GMAD::Element::frequency)
    .def_readwrite("phase", &GMAD::Element::phase)
    .def_readwrite("tOffset", &GMAD::Element::tOffset)

    .def_readwrite("kick1", &GMAD::Element::kick1)
    .def_readwrite("kick2", &GMAD::Element::kick2)
    .def_readwrite("kick3", &GMAD::Element::kick3)
    .def_readwrite("kick4", &GMAD::Element::kick4)
    .def_readwrite("rmat11", &GMAD::Element::rmat11)
    .def_readwrite("rmat12", &GMAD::Element::rmat12)
    .def_readwrite("rmat13", &GMAD::Element::rmat13)
    .def_readwrite("rmat14", &GMAD::Element::rmat14)
    .def_readwrite("rmat21", &GMAD::Element::rmat21)
    .def_readwrite("rmat22", &GMAD::Element::rmat22)
    .def_readwrite("rmat23", &GMAD::Element::rmat23)
    .def_readwrite("rmat24", &GMAD::Element::rmat24)
    .def_readwrite("rmat31", &GMAD::Element::rmat31)
    .def_readwrite("rmat32", &GMAD::Element::rmat32)
    .def_readwrite("rmat33", &GMAD::Element::rmat33)
    .def_readwrite("rmat34", &GMAD::Element::rmat34)
    .def_readwrite("rmat41", &GMAD::Element::rmat41)
    .def_readwrite("rmat42", &GMAD::Element::rmat42)
    .def_readwrite("rmat43", &GMAD::Element::rmat43)
    .def_readwrite("rmat44", &GMAD::Element::rmat44)

    .def_readwrite("beampipeThickness", &GMAD::Element::beampipeThickness)
    .def_readwrite("aper1", &GMAD::Element::aper1)
    .def_readwrite("aper2", &GMAD::Element::aper2)
    .def_readwrite("aper3", &GMAD::Element::aper3)
    .def_readwrite("aper4", &GMAD::Element::aper4)
    .def_readwrite("apertureType", &GMAD::Element::apertureType)
    .def_readwrite("beampipeMaterial", &GMAD::Element::beampipeMaterial)
    .def_readwrite("vacuumMaterial", &GMAD::Element::vacuumMaterial);
}