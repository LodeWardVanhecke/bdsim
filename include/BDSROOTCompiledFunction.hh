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
#ifndef BDSROOTCOMPILEDFUNCTION_H
#define BDSROOTCOMPILEDFUNCTION_H

#include <string>

class BDSROOTCompiledFunctionBase;

/**
 * @brief TBC
 * 
 * @author Stewart Boogert
 */

class BDSROOTCompiledFunction
{
public:
  BDSROOTCompiledFunction(std::string fileName,
			  std::string className);
  ~BDSROOTCompiledFunction();
  
protected:
  /// TBC - LN -> should not use _ in variable name - reserved for C++ implementation
  std::string _functionName;
  BDSROOTCompiledFunctionBase* _function;
};

#endif
