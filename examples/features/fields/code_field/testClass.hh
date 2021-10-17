//
// Created by Stewart Boogert on 17/10/2021.
//

#include "TROOT.h"
#include "BDSROOTCompiledFunctionBase.hh"

class testClass : public BDSROOTCompiledFunctionBase {
public:
    testClass() : BDSROOTCompiledFunctionBase() {}
    testClass(const char *_name, const char *_title) : BDSROOTCompiledFunctionBase(_name, _title) {}
    virtual ~testClass() {}
    virtual void SetParameters(double* ) {return;}
    virtual void Evaluate(double* /* vars */, double* /*return*/) {return;}
};

