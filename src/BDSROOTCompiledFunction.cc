//
// Created by Stewart Boogert on 16/10/2021.
//

#include "BDSUtilities.hh"
#include "BDSROOTCompiledFunction.hh"
#include "TROOT.h"
#include "TSystem.h"

#include <iostream>
#include <string>

#include "BDSROOTCompiledFunctionBase.hh"

BDSROOTCompiledFunction::BDSROOTCompiledFunction(std::string fileName, std::string className) {
    std::cout << "BDSROOTCompiledFunction::BDSROOTCompiledFunction>" << std::endl;
    gSystem->AddIncludePath((" -I"+BDS::GetBDSIMExecPath()+"/../include/bdsim/").c_str());
    gSystem->CompileMacro(fileName.c_str());

    std::string cmdCreateClass = className+" *c = new "+className+"();"; // TODO goes via interpreter so c is global
    gROOT->ProcessLine(cmdCreateClass.c_str());
    _function = (BDSROOTCompiledFunctionBase*)gROOT->GetListOfGlobals()->FindObject("c"); // TODO need to have a unique variable

    double *v; // dummy input to test execution of function
    double *r; // dummy return to test execution of function
    std::cout <<  "BDSROOTCompiledFunction::BDSROOTCompiledFunction> " << _function << " " <<  std::endl;
}

BDSROOTCompiledFunction::~BDSROOTCompiledFunction() {

}