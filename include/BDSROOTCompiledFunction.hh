//
// Created by Stewart Boogert on 16/10/2021.
//

#ifndef BDSROOTCOMPILEDFUNCTION_HH
#define BDSROOTCOMPILEDFUNCTION_HH

#include <string>

class BDSROOTCompiledFunctionBase;

class BDSROOTCompiledFunction {
public:
    BDSROOTCompiledFunction(std::string fileName, std::string className);
    ~BDSROOTCompiledFunction();
protected:
    std::string _functionName;
    BDSROOTCompiledFunctionBase* _function;

};

#endif //BDSROOTCOMPILEDFUNCTION_HH
