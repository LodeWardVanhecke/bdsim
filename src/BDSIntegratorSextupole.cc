#include "BDSDebug.hh"
#include "BDSIntegratorSextupole.hh"
#include "BDSMagnetStrength.hh"
#include "BDSStep.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Mag_EqRhs.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSIntegratorSextupole::BDSIntegratorSextupole(BDSMagnetStrength const* strength,
					       G4double                 brho,
					       G4Mag_EqRhs*             eqOfMIn):
  BDSIntegratorMag(eqOfMIn, 6)
{
  // B'' = d^2By/dx^2 = Brho * (1/Brho d^2By/dx^2) = Brho * k2
  bDoublePrime = brho * (*strength)["k2"] / CLHEP::m3;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "B'' = " << bDoublePrime << G4endl;
#endif
}

void BDSIntegratorSextupole::AdvanceHelix(const G4double  yIn[],
					  G4double        h,
					  G4double        yOut[])
{
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);
  G4double momUnit = mom.mag();
  G4double kappa = (-eqOfM->FCof()*bDoublePrime) / momUnit;

  if (std::abs(kappa) < 1e-12)
    {
      AdvanceDriftMag(yIn, h, yOut);
      SetDistChord(0);
      return;
    }
  
  G4ThreeVector pos          = G4ThreeVector( yIn[0], yIn[1], yIn[2]);
  BDSStep       localPosMom  = ConvertToLocal(pos, mom, h, false);
  G4ThreeVector localPos     = localPosMom.PreStepPoint();
  G4ThreeVector localMom     = localPosMom.PostStepPoint();
  G4ThreeVector localMomUnit = localMom.unit();
  
  G4double x0 = localPos.x();
  G4double y0 = localPos.y();
  
  // Evaluate field at the approximate midpoint of the step.
  const G4double halfH = 0.5*h;
  x0 = x0 + localMomUnit.x()*halfH;
  y0 = y0 + localMomUnit.y()*halfH;
  
  G4double x02My02 = x0*x0 - y0*y0;
  
  G4double xp = localMomUnit.x();
  G4double yp = localMomUnit.y();
  G4double zp = localMomUnit.z();
  
  // local r'' (for curvature)
  G4ThreeVector localA;
  localA.setX(zp*x02My02);
  localA.setY(-2*zp*x0*y0);
  localA.setZ(xp*x02My02-2*yp*x0*y0);
  
  localA*=kappa/2; // 2 is actually a 2! factor.
  
  AdvanceChord(h,localPos,localMomUnit,localA);
  
  ConvertToGlobal(localPos,localMomUnit,momUnit,yOut);
}

void BDSIntegratorSextupole::Stepper(const G4double yIn[],
				     const G4double dydx[],
				     const G4double h,
				     G4double       yOut[],
				     G4double       yErr[])
{
  G4double yTemp[7];

  G4ThreeVector pos = G4ThreeVector(yIn[0], yIn[1], yIn[2]);
  G4ThreeVector mom = G4ThreeVector(yIn[3], yIn[4], yIn[5]);

  G4ThreeVector momUnit = mom.unit();

  auxNavigator->LocateGlobalPointAndSetup(pos);
  G4AffineTransform GlobalAffine = auxNavigator->GetGlobalToLocalTransform();
  G4ThreeVector     localMomUnit = GlobalAffine.TransformAxis(momUnit);

  if (localMomUnit.z() < 0.9 || mom.mag() < 40.0 )
    {
      backupStepper->Stepper(yIn, dydx, h, yOut, yErr);
      SetDistChord(backupStepper->DistChord());
      return;
    }
  
  // Do two half steps
  AdvanceHelix(yIn,   0.5*h, yTemp);
  AdvanceHelix(yTemp, 0.5*h, yOut); 
  
  // Do a full Step
  AdvanceHelix(yIn, h, yTemp);

  for(G4int i = 0; i < nVariables; i++)
    {
      yErr[i] = yOut[i] - yTemp[i];
      // if error small, set error to 0
      // this is done to prevent Geant4 going to smaller and smaller steps
      // ideally use some of the global constants instead of hardcoding here
      // could look at step size as well instead.
      if (std::abs(yErr[i]) < 1e-7)
	{yErr[i] = 0;}
    }
  // TBC - we have not set DistChord here!
}
