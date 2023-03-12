#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parser.h"
#include "beam.h"
#include "element.h"
#include "options.h"
#include "region.h"

PYBIND11_MODULE(parser, m) {
    py::class_<GMAD::Parser>(m,"Parser")
      .def_static("Instance",[](std::string fileName) {return GMAD::Parser::Instance(fileName);})
      .def_static("Instance",[](){return GMAD::Parser::Instance();})

       /// Exit method
      .def("quit",&GMAD::Parser::quit)
      /// Method that transfers parameters to element properties
      .def("write_table",&GMAD::Parser::write_table)

       /// Expand a sequence by name from start to end into the target list. This
       /// removes sublines from the beamline into one LINE.
       ///@{ Add value to front of temporary list
       //
       .def("expand_line",[](GMAD::Parser &parser,
                             GMAD::FastList<GMAD::Element>& target,
                             const std::string& name,
                             std::string        start = "",
                             std::string        end   = "") {parser.expand_line(target,name,start,end);})
       /// Expand the main beamline as defined by the use command.
       .def("expand_line",[](GMAD::Parser &parser,
                             const std::string name,
                             std::string start,
                             std::string end) {parser.expand_line(name,start,end);})
       .def("get_sequence",&GMAD::Parser::get_sequence)

       ///@{ Add value to front of temporary list
       .def("Store",[](GMAD::Parser &parser, double value) {parser.Store(value);})
       .def("Store",[](GMAD::Parser &parser, const std::string& name) {parser.Store(name);})

       //
       .def("ClearParams",&GMAD::Parser::ClearParams)
       // .def("SetValue_Beam",[](GMAD::Parser &parser,std::string property, GMAD::Beam &beam) {parser.SetValue<GMAD::Beam, GMAD::Beam>(property,beam);})
       .def("GetGlobal_Beam",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Beam>();})
       .def("GetGlobal_Parameters",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Parameters>();})
       .def("GetGlobal_Options",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Options>();})
       .def("GetGlobal_Region",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Region>();})
       .def("GetGlobal_NewColour",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::NewColour>();})
       .def("GetGlobal_Crystal",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Crystal>();})
       .def("GetGlobal_Field",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Field>();})
       .def("GetGlobal_Query",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Query>();})
       .def("GetGlobal_Atom",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Atom>();})
       .def("GetGlobal_Material",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Material>();})
       .def("GetGlobal_Tunnel",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Tunnel>();})
       .def("GetGlobal_CavityModel",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::CavityModel>();})
       .def("GetGlobal_Scorer",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Scorer>();})
       .def("GetGlobal_ScorerMesh",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::ScorerMesh>();})
       .def("GetGlobal_Placement",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Placement>();})
       .def("GetGlobal_SpamplerPlacement",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::SamplerPlacement>();})
       .def("GetGlobal_BLMPlacement",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::BLMPlacement>();})
       .def("GetGlobal_Aperture",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Aperture>();})


       .def("PrintBeamline", &GMAD::Parser::PrintBeamline)
       .def("PrintElements", &GMAD::Parser::PrintElements)
       .def("PrintOptions", &GMAD::Parser::PrintOptions)
       //
       .def("TryPrintingObject", &GMAD::Parser::TryPrintingObject)
       //
       .def("GetBeamline",&GMAD::Parser::GetBeamline);
}