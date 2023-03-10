#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "element.h"
#include "fastlist.h"

PYBIND11_MODULE(fastlist, m) {
  py::class_<GMAD::FastList<GMAD::Element>>(m,"FastList_Element")
    .def(py::init<>());

  py::class_<GMAD::FastList<GMAD::Element>::FastListIterator>(m,"FastList_Element_Iterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastListConstIterator>(m,"FastList_Element_ConstIterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapIterator>(m,"FastMap_Element_Iterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapConstIterator>(m,"FastMap_Element_ConstIterator");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapIteratorPair>(m,"FastMap_Element_IteratorPair");
  py::class_<GMAD::FastList<GMAD::Element>::FastMapConstIteratorPair>(m,"FastMap_Element_ConstIteratorPair");
}