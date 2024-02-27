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
#ifndef BDSROOTCOMPILEDFUNCTIONBASE_H
#define BDSROOTCOMPILEDFUNCTIONBASE_H
#include "TNamed.h"

/**
 * @brief TBC
 * 
 * @author Stewart Boogert
 */

class BDSROOTCompiledClassBase : public TNamed
{
public:
  BDSROOTCompiledClassBase():
    TNamed()
  {;}
  BDSROOTCompiledClassBase(const char* name,
			      const char* title):
    TNamed(name,title)
  {;}
  virtual ~BDSROOTCompiledClassBase() {};
  void SetNumberOfParameters(int numberOfParametersIn=0) { numberOfParameters = numberOfParametersIn;}
  void SetNumberOfVariables(int numberOfVariablesIn=0) { numberOfVariables = numberOfVariablesIn;}
  virtual int GetNumberOfParameters() {return 0;}
  virtual int GetNumberOfVariables() {return 0;}
  virtual void SetParameters(double *pars) = 0;
  virtual void Evaluate(double *vars, double &ret) = 0;
  
protected:
  int numberOfParameters = 0;
  int numberOfVariables = 0;
};

#endif
