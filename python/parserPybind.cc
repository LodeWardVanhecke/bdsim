#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parser.h"

#include "atom.h"
#include "aperture.h"
#include "beam.h"
#include "element.h"
#include "options.h"
#include "region.h"
#include "fastlist.h"
#include "physicsbiasing.h"
#include "tunnel.h"

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

       .def("Add_Atom",[](GMAD::Parser *parser) {parser->Add<GMAD::Atom, GMAD::FastList<GMAD::Atom>>();})
       .def("Add_Aperture",[](GMAD::Parser *parser) {parser->Add<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>();})
       .def("Add_BLMPlacement",[](GMAD::Parser *parser) {parser->Add<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>();})
       .def("Add_CavityModel",[](GMAD::Parser *parser) {parser->Add<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>();})
       .def("Add_Crystal",[](GMAD::Parser *parser) {parser->Add<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>();})
       .def("Add_Field",[](GMAD::Parser *parser) {parser->Add<GMAD::Field, GMAD::FastList<GMAD::Field>>();})
       .def("Add_Material",[](GMAD::Parser *parser) {parser->Add<GMAD::Material, GMAD::FastList<GMAD::Material>>();})
       .def("Add_Modulator",[](GMAD::Parser *parser) {parser->Add<GMAD::Modulator, GMAD::FastList<GMAD::Modulator>>();})
       .def("Add_NewColour",[](GMAD::Parser *parser) {parser->Add<GMAD::NewColour, GMAD::FastList<GMAD::NewColour>>();})
       .def("Add_PhysicsBiasing",[](GMAD::Parser *parser) {parser->Add<GMAD::PhysicsBiasing, GMAD::FastList<GMAD::PhysicsBiasing>>();})
       .def("Add_Placement",[](GMAD::Parser *parser) {parser->Add<GMAD::Placement, GMAD::FastList<GMAD::Placement>>();})
       .def("Add_Query",[](GMAD::Parser *parser) {parser->Add<GMAD::Query, GMAD::FastList<GMAD::Query>>();})
       .def("Add_Region",[](GMAD::Parser *parser) {parser->Add<GMAD::Region, GMAD::FastList<GMAD::Region>>();})
       .def("Add_SamplerPlacement",[](GMAD::Parser *parser) {parser->Add<GMAD::SamplerPlacement, GMAD::FastList<GMAD::SamplerPlacement>>();})
       .def("Add_Scorer",[](GMAD::Parser *parser) {parser->Add<GMAD::Scorer, GMAD::FastList<GMAD::Scorer>>();})
       .def("Add_ScorerMesh",[](GMAD::Parser *parser) {parser->Add<GMAD::ScorerMesh, GMAD::FastList<GMAD::ScorerMesh>>();})
       .def("Add_Tunnel",[](GMAD::Parser *parser) {parser->Add<GMAD::Tunnel, GMAD::FastList<GMAD::Tunnel>>();})

       .def("Add_Atom",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Atom, GMAD::FastList<GMAD::Atom>>(unique, className);})
       .def("Add_Aperture",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>(unique, className);})
       .def("Add_BLMPlacement",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>(unique, className);})
       .def("Add_CavityModel",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>(unique, className);})
       .def("Add_Crystal",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>(unique, className);})
       .def("Add_Field",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Field, GMAD::FastList<GMAD::Field>>(unique, className);})
       .def("Add_Material",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Material, GMAD::FastList<GMAD::Material>>(unique, className);})
       .def("Add_Modulator",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Modulator, GMAD::FastList<GMAD::Material>>(unique, className);})
       .def("Add_NewColour",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::NewColour, GMAD::FastList<GMAD::NewColour>>(unique, className);})
       .def("Add_PhysicsBiasing",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::PhysicsBiasing, GMAD::FastList<GMAD::PhysicsBiasing>>(unique, className);})
       .def("Add_Placement",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Placement, GMAD::FastList<GMAD::Placement>>(unique, className);})
       .def("Add_Query",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Query, GMAD::FastList<GMAD::Query>>(unique, className);})
       .def("Add_Region",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Region, GMAD::FastList<GMAD::Region>>(unique, className);})
       .def("Add_SamplerPlacement",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::SamplerPlacement, GMAD::FastList<GMAD::SamplerPlacement>>(unique, className);})
       .def("Add_Scorer",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Scorer, GMAD::FastList<GMAD::Scorer>>(unique, className);})
       .def("Add_ScorerMesh",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::ScorerMesh, GMAD::FastList<GMAD::ScorerMesh>>(unique, className);})
       .def("Add_Tunnel",[](GMAD::Parser *parser, bool unique, std::string className) {parser->Add<GMAD::Tunnel, GMAD::FastList<GMAD::Tunnel>>(unique, className);})


       .def("GetGlobal_Atom",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Atom>();})
       .def("GetGlobal_Aperture",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Aperture>();})
       .def("GetGlobal_Beam",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Beam>();})
       .def("GetGlobal_BLMPlacement",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::BLMPlacement>();})
       .def("GetGlobal_CavityModel",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::CavityModel>();})
       .def("GetGlobal_Crystal",[](GMAD::Parser parser) {return parser.GetGlobalPtr<GMAD::Crystal>();})
       .def("GetGlobal_Field",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Field>();})
       .def("GetGlobal_Material",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Material>();})
       .def("GetGlobal_Modulator",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Modulator>();})
       .def("GetGlobal_NewColour",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::NewColour>();})
       .def("GetGlobal_Options",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Options>();})
       .def("GetGlobal_PhysicsBias",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::PhysicsBiasing>();})
       .def("GetGlobal_Placement",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Placement>();})
       .def("GetGlobal_Query",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Query>();})
       .def("GetGlobal_Region",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Region>();})
       .def("GetGlobal_SpamplerPlacement",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::SamplerPlacement>();})
       .def("GetGlobal_Scorer",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Scorer>();})
       .def("GetGlobal_ScorerMesh",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::ScorerMesh>();})
       .def("GetGlobal_Tunnel",[](GMAD::Parser *parser) {return parser->GetGlobalPtr<GMAD::Tunnel>();})


       .def("GetGlobal_Parameters",[](GMAD::Parser parser) {return parser.GetGlobal<GMAD::Parameters>();})

       .def("GetList_Atom",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Atom, GMAD::FastList<GMAD::Atom>>();})
       .def("GetList_Aperture",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Aperture, GMAD::FastList<GMAD::Aperture>>();})
       .def("GetList_BLMPlacement",[](GMAD::Parser *parser) {return parser->GetList<GMAD::BLMPlacement, GMAD::FastList<GMAD::BLMPlacement>>();})
       .def("GetList_CavityModel",[](GMAD::Parser *parser) {return parser->GetList<GMAD::CavityModel, GMAD::FastList<GMAD::CavityModel>>();})
       .def("GetList_Crystal",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Crystal, GMAD::FastList<GMAD::Crystal>>();})
       .def("GetList_Field",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Field, GMAD::FastList<GMAD::Field>>();})
       .def("GetList_Material",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Material, GMAD::FastList<GMAD::Material>>();})
       .def("GetList_Modulator",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Modulator, GMAD::FastList<GMAD::Modulator>>();})
       .def("GetList_NewColour",[](GMAD::Parser *parser) {return parser->GetList<GMAD::NewColour, GMAD::FastList<GMAD::NewColour>>();})
       .def("GetList_PhysicsBias",[](GMAD::Parser *parser) {return parser->GetList<GMAD::PhysicsBiasing, GMAD::FastList<GMAD::PhysicsBiasing>>();})
       .def("GetList_Placement",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Placement, GMAD::FastList<GMAD::Placement>>();})
       .def("GetList_Region",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Region, GMAD::FastList<GMAD::Region>>();})
       .def("GetList_SamplerPlacement",[](GMAD::Parser *parser) {return parser->GetList<GMAD::SamplerPlacement, GMAD::FastList<GMAD::SamplerPlacement>>();})
       .def("GetList_Scorer",[](GMAD::Parser *parser) {return parser->GetList<GMAD::Scorer, GMAD::FastList<GMAD::Scorer>>();})
       .def("GetList_ScorerMesh",[](GMAD::Parser *parser) {return parser->GetList<GMAD::ScorerMesh, GMAD::FastList<GMAD::ScorerMesh>>();})


       .def("SetValue_Atom",[](GMAD::Parser &parser, std::string property, std::string value ) {parser.SetValue<GMAD::Atom,std::string>(property, value);})
       .def("SetValue_Atom",[](GMAD::Parser &parser, std::string property, double value ) {parser.SetValue<GMAD::Atom,double>(property, value);})

       .def("PrintBeamline", &GMAD::Parser::PrintBeamline)
       .def("PrintElements", &GMAD::Parser::PrintElements)
       .def("PrintOptions", &GMAD::Parser::PrintOptions)
       //
       .def("TryPrintingObject", &GMAD::Parser::TryPrintingObject)
       //
       .def("GetBeamline",&GMAD::Parser::GetBeamline);
}