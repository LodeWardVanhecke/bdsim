/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSROOTCompiledFunction.hh"
#include "BDSROOTCompiledClassBase.hh"
#include "BDSUtilities.hh"

#include "TROOT.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>
#include <string>


BDSROOTCompiledFunction::BDSROOTCompiledFunction(std::string functionName,
						 std::string functionCode,
                                                 bool compile)
{

  if(compile) {
    // write temporary file
    std::ofstream ofstr("temp.cxx");
    ofstr << functionCode << std::endl;
    gSystem->CompileMacro("temp.cxx");
    auto tf = gROOT->GetGlobalFunction(functionName.c_str());
  }

  /*
  std::cout << "BDSROOTCompiledFunction::BDSROOTCompiledFunction>" << std::endl;
  gSystem->AddIncludePath((" -I"+BDS::GetBDSIMExecPath()+"/../include/bdsim/").c_str());
  gSystem->CompileMacro(fileName.c_str());
  
  std::string cmdCreateClass = className+" *c = new "+className+"();"; // TODO goes via interpreter so c is global
  gROOT->ProcessLine(cmdCreateClass.c_str());
  function = (BDSROOTCompiledFunctionBase*)gROOT->GetListOfGlobals()->FindObject("c"); // TODO need to have a unique variable
  
  double *v; // dummy input to test execution of function
  double *r; // dummy return to test execution of function
  std::cout <<  "BDSROOTCompiledFunction::BDSROOTCompiledFunction> " << function << " " <<  std::endl;
  */
}

BDSROOTCompiledFunction::~BDSROOTCompiledFunction()
{;}
