/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#ifndef BDSSDTHINTHING_H
#define BDSSDTHINTHING_H

#include "BDSHitThinThing.hh"
#include "BDSSensitiveDetector.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
class G4VHit;

/**
 * @brief The sensitive detector class that provides sensitivity to record thin thing hits.
 * 
 * @author Laurie Nevay
 */

class BDSSDThinThing: public BDSSensitiveDetector
{ 
public:
  /// Include unique name for each instance.
  BDSSDThinThing(G4String name,
		 G4bool   storeTrajectoryLocalIn,
		 G4bool   storeTrajectoryLinksIn,
		 G4bool   storeTrajectoryIonsIn);

  /// @{ Assignment and copy constructor not implemented nor used
  BDSSDThinThing& operator=(const BDSSDThinThing&) = delete;
  BDSSDThinThing(BDSSDThinThing&) = delete;
  /// @}

  virtual ~BDSSDThinThing();

  /// Overriden from G4VSensitiveDetector. Creates hits collection and registers it with
  /// the hits collection of this event (HCE).
  virtual void Initialize(G4HCofThisEvent* HCE) override;

  /// Overriden from G4VSensitiveDetector. Creates hit instances and appends them to the
  /// hits collection.
  virtual G4bool ProcessHits(G4Step* aStep,
			     G4TouchableHistory* rOHist) override;

  /// Override creation of hit w.r.t. hit already generated by other SD, namely the
  /// energy deposition SD for collimator hits.
  virtual G4bool ProcessHitsOrdered(G4Step* step,
				    G4TouchableHistory*  rOHist,
				    const std::vector<G4VHit*>& hits) override;

  /// Return the last collimator hit.
  virtual G4VHit* last() const override;

private:  
  /// The hits collection for this sensitive detector class that's owned by each instance.
  BDSHitsCollectionThinThing* thinThingCollection;

  /// The name of the hits collection that's created and registered.
  G4String itsCollectionName;

  /// Hits collection ID - an integer look up for the hits collection
  /// provided by G4SDManager (a registry) that is given to the
  /// G4HCofThisEvent (Hits collection of the event).
  G4int itsHCID;

  const G4bool storeTrajectoryLocal;
  const G4bool storeTrajectoryLinks;
  const G4bool storeTrajectoryIons;
};

#endif

