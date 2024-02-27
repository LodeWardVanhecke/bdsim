//
// Created by Stewart Boogert on 17/10/2021.
//

#include "BDSROOTCompiledClassBase.hh"
#include "TROOT.h"
#include <iostream>

class testClass : public BDSROOTCompiledClassBase {
public:
    testClass() : BDSROOTCompiledClassBase() {}
    testClass(const char *name, const char *title) : BDSROOTCompiledClassBase(name, title) {}
    virtual ~testClass() {}
    virtual void SetParameters(double*) {return;}
    virtual void Evaluate(double* vars, double &retval) {
      std::cout << vars[0] << " " << vars[1] << std::endl;
      retval = 0;
      std::cout << retval << std::endl;
    }
};

