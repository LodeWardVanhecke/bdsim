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
#ifndef BDSWRAPPERMUONSPLITTING_H
#define BDSWRAPPERMUONSPLITTING_H
#include "BDSWrapperProcess.hh"

#include "G4Types.hh"

class G4Step;
class G4Track;
class G4VParticleChange;
class G4VProcess;

class BDSWrapperMuonSplitting: public BDSWrapperProcess
{
public:
  BDSWrapperMuonSplitting() = delete;
  BDSWrapperMuonSplitting(G4VProcess* originalProcess,
			  G4int splittingFactorIn); 
  virtual ~BDSWrapperMuonSplitting();
  
  virtual G4VParticleChange* PostStepDoIt(const G4Track& track,
					  const G4Step& step);
  
private:
  
  G4bool MuonPresent(G4VParticleChange* aChange) const;
  void DeleteSecondaries(G4VParticleChange* aChange) const;
  G4int splittingFactor;
};

#endif
