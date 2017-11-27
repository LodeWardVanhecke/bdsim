/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "BDSDebug.hh"
#include "BDSFieldEM.hh"
#include "BDSFieldEMGlobal.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

BDSFieldEMGlobal::BDSFieldEMGlobal(BDSFieldEM* fieldIn):
  field(fieldIn)
{;}

BDSFieldEMGlobal::~BDSFieldEMGlobal()
{
  delete field;
}

std::pair<G4ThreeVector,G4ThreeVector> BDSFieldEMGlobal::GetField(const G4ThreeVector& position,
								  const G4double       t) const
{
  G4ThreeVector localPosition = ConvertToLocal(position);
  auto          localField    = field->GetFieldTransformed(localPosition, t);
  auto          globalField   = ConvertAxisToGlobal(localField);
// #ifdef BDSDEBUG
//   G4cout << __METHOD_NAME__ << "Local Position: " << localPosition << G4endl;
//   G4cout << __METHOD_NAME__ << "Local Field:    " << localField.first  << " " << localField.second  << G4endl;
//   G4cout << __METHOD_NAME__ << "Global Field:   " << globalField.first << " " << globalField.second << G4endl;
// #endif
  return globalField;
}
