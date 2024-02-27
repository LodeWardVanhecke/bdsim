//
// Created by Boogert Stewart on 26/02/2024.
//

#include "BDSROOTCompiledClass.hh"
#include "BDSROOTCompiledClassBase.hh"
#include "BDSUtilities.hh"

#include "TROOT.h"
#include "TSystem.h"

BDSROOTCompiledClass::BDSROOTCompiledClass(std::string fileName,
                                           std::string className)
{

  std::cout << "BDSROOTCompiledFunction::BDSROOTCompiledFunction>" << std::endl;
  gSystem->AddIncludePath((" -I"+BDS::GetBDSIMExecPath()+"/../include/bdsim/").c_str());
  gSystem->CompileMacro(fileName.c_str());

  std::string cmdCreateClass = className+" *c = new "+className+"();"; // TODO goes via interpreter so c is global
  gROOT->ProcessLine(cmdCreateClass.c_str());
  function = (BDSROOTCompiledClassBase*)gROOT->GetListOfGlobals()->FindObject("c"); // TODO need to have a unique variable

  double v[2] = {10,20}; // dummy input to test execution of function
  double r; // dummy return to test execution of function

  std::cout <<  "BDSROOTCompiledFunction::BDSROOTCompiledFunction> " << function << " " <<  std::endl;
  function->Evaluate(v,r);
  G4cout << r << G4endl;
}

BDSROOTCompiledClass::~BDSROOTCompiledClass()
{;}