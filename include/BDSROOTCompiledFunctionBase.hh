//
// Created by Stewart Boogert on 17/10/2021.
//

#ifndef BDSROOTCOMPILEDFUNCTIONBASE_HH
#define BDSROOTCOMPILEDFUNCTIONBASE_HH

class BDSROOTCompiledFunctionBase : public TNamed {
public :
    BDSROOTCompiledFunctionBase() : TNamed() {}
    BDSROOTCompiledFunctionBase(const char* name, const char* title) : TNamed(name,title) {}
    virtual ~BDSROOTCompiledFunctionBase() {};
    void SetNumberOfParameters(int numberOfParameters=0) { _numberOfParameters = numberOfParameters;}
    void SetNumberOfVariables(int numberOfVariables=0) { _numberOfVariables = numberOfVariables;}
    virtual int GetNumberOfParameters() {return 0;}
    virtual int GetNumberOfVariables() {return 0;}
    virtual void SetParameters(double *pars) = 0;
    virtual void Evaluate(double *vars, double *ret) = 0;

protected :
    int _numberOfParameters = 0;
    int _numberOfVariables = 0;
};

#endif //BDSROOTCOMPILEDFUNCTIONBASE_HH
