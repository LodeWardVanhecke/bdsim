#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"

#include "BDSAcceleratorComponent.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBendBuilder.hh"
#include "BDSComponentFactory.hh"
#include "BDSDebug.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorType.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSUtilities.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

class BDSBeamPipeInfo;
class BDSMagnetStrengh;

using namespace GMAD;

BDSLine* BDS::BuildSBendLine(const Element*     element,
			     BDSMagnetStrength* st,
			     const G4double     brho,
			     const BDSIntegratorSet* integratorSet)
{
  const G4String     name = element->name;
  const G4double   length = element->l  * CLHEP::m;
  const G4double    angle = (*st)["angle"];
  const G4double       e1 = element->e1 * CLHEP::rad;
  const G4double       e2 = element->e2 * CLHEP::rad;
  const G4double  angleIn = 0.5 * angle + e1;
  const G4double angleOut = 0.5 * angle + e2;
  const G4bool yokeOnLeft = BDSComponentFactory::YokeOnLeft(element,st);
  
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
  
  // Calculate number of sbends to split parent into
  G4int nSBends = BDS::CalculateNSBendSegments(length, angle, e1, e2);
  
  BDSLine* sbendline  = new BDSLine(name);
  
  // Single element if no poleface and zero bend angle or dontSplitSBends=1, therefore nSBends = 1
  if (!BDS::IsFinite(angle) || (nSBends == 1))
    {
      //Zero angle bend only needs one element.
      std::string thename = name + "_1_of_1";
      
      BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
      BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
						   brho,
						   intType,
						   st);
      // prepare one sbend segment
      auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -angleIn, -angleOut);
      auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, -angleIn, -angleOut, yokeOnLeft);
      mgInfo->name = thename;
      BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
					 thename,
					 length,
					 bpInfo,
					 mgInfo,
					 vacuumField,
					 angle);
      
      oneBend->SetBiasVacuumList(element->biasVacuumList);
      oneBend->SetBiasMaterialList(element->biasMaterialList);
      
      sbendline->AddComponent(oneBend);
      
      return sbendline;
    }
  
  //calculate their angles and length
  G4double semiangle  = angle  / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double rho        = length / angle;

  BDSMagnetStrength* centralStrength = new BDSMagnetStrength(*st);
  (*centralStrength)["angle"] = semiangle;
  (*centralStrength)["length"] = semilength;
  
  G4double zExtentIn  = 0;
  G4double zExtentOut = 0;
  G4bool fadeIn = true;
  G4bool fadeOut = true;

  G4double outerDiameter = element->outerDiameter*CLHEP::m;
  if (outerDiameter < 1e-6)
    {//outerDiameter not set - use global option as default
      outerDiameter = BDSGlobalConstants::Instance()->OuterDiameter();
    }
  
  //calculate extent along z due poleface rotation
  if (angleIn > 0)
    {zExtentIn = 0.5*outerDiameter*tan(angleIn - 0.5*std::abs(semiangle));}
  else if (angleIn < 0)
    {zExtentIn = 0.5*outerDiameter*tan(0.5*std::abs(semiangle) + angleIn);}
  if (angleOut > 0)
    {zExtentOut = 0.5*outerDiameter*tan(angleOut - 0.5*std::abs(semiangle));}
  else if (angleOut < 0)
    {zExtentOut = 0.5*outerDiameter*tan(0.5*std::abs(semiangle) + angleOut);}
  
  //decide if wedge angles fade or not depending on the extents
  if (std::abs(zExtentIn) < semilength/4.0)
    {fadeIn = false;}
  if (std::abs(zExtentOut) < semilength/4.0)
    {fadeOut = false;}
  
  //if the element faces fade in, the middle wedges should be numbered as such
  //if not, it'll be repeated from the first segment onwards
  G4int centralWedgeNum = 0.5*(nSBends+1);
  G4String  centralName = name;
  if (fadeIn)
    {centralName += "_"+std::to_string(centralWedgeNum)+"_of_" + std::to_string(nSBends);}
  else
    {centralName += "_1_of_" + std::to_string(nSBends);}
  
  // register the central wedge which will always be used as the
  // middle wedge regardless of poleface rotations

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       centralStrength);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, -0.5*semiangle, -0.5*semiangle);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, -0.5*semiangle, -0.5*semiangle, yokeOnLeft);
  mgInfo->name = centralName;
  BDSMagnet* centralWedge = new BDSMagnet(BDSMagnetType::sectorbend,
					  centralName,
					  semilength,
					  bpInfo,
					  mgInfo,
					  vacuumField,
					  semiangle);
  
  //oneBend can be accComp or BDSMagnet depending on registration/reusage or new magnet
  BDSAcceleratorComponent* oneBend = nullptr;
  
  BDSMagnetType magType = BDSMagnetType::sectorbend;
  // check magnet outer info
  BDSMagnetOuterInfo* magnetOuterInfoCheck = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut, yokeOnLeft);
  BDSComponentFactory::CheckBendLengthAngleWidthCombo(semilength, semiangle,
						      magnetOuterInfoCheck->outerDiameter,
						      name + "_semi");
  // clean up
  delete magnetOuterInfoCheck;
  
  // first element should be fringe if poleface specified
  if (BDS::IsFinite(e1) && includeFringe) // note angleIn is always non zero for non-zero bend
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      (*fringeStIn)["polefaceangle"] = e1;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(rho, e1, element->fint);
      G4String segmentName           = name + "_e1_fringe";
      G4double fringeAngle           = -e1 - 0.5*((*fringeStIn)["angle"]);
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, fringeAngle, -fringeAngle,
						      segmentName, magType, fringeStIn, brho,
						      integratorSet);
      sbendline->AddComponent(startfringe);
    }
  
  // logic for wedge elements in the beamline:
  // reuse central wedge for all wedges of in/out half if no poleface angle(s)
  // if small poleface, new first/last wedge, reuse central wedge for remainder of in/out half
  // otherwise fade in/out faces for all wedges in app. halves.
  // 'central' one is definitely used for the central part, but also it's just a segment
  // with even incoming and outgoing face angles w.r.t. the chord.
  for (G4int i = 0; i < nSBends; ++i)
    {
      G4String thename = name + "_"+std::to_string(i+1)+"_of_" + std::to_string(nSBends);
      if (i < 0.5*(nSBends-1))
        {// first half of magnet
          if (!BDS::IsFinite(e1)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeIn) // build incremented angled segment
            {oneBend = BDS::BuildSBend(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet, yokeOnLeft);}
          else
            {// finite pole face, but not strong so build one angled, then repeat the rest to save memory
              if (i == 0) // the first one is unique
                {oneBend = BDS::BuildSBend(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet, yokeOnLeft);}
              else // others afterwards are a repeat of the even angled one
                {oneBend = centralWedge;}
            }
        }
      else if (i > 0.5*(nSBends-1))
        {// second half of magnet
          if (!BDS::IsFinite(e2)) // no pole face rotation so just repeat central segment
            {oneBend = centralWedge;}
          else if (fadeOut) // build incremented angled segment
            {oneBend = BDS::BuildSBend(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet, yokeOnLeft);}
          else
            {// finite pole face, but not strong so build only one unique angled on output face
              if (i == (nSBends-1)) // one from end - TBC - why isn't this the last one?
                {oneBend = BDS::BuildSBend(element, fadeIn, fadeOut, i, nSBends, st, brho, integratorSet, yokeOnLeft);}
              else // after central, but before unique end piece - even angled.
                {oneBend = centralWedge;}
            }
        }
      else // the middle piece
        {oneBend = centralWedge;}

      // append to the line
      sbendline->AddComponent(oneBend);
      
#ifdef BDSDEBUG
      G4cout << "---->creating sbend line,"
	     << " element= " << thename
	     << " angleIn= " << angleIn
	     << " angleOut= " << angleOut << "m"
	     << G4endl;
#endif
    }
  
  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(e2) && includeFringe)
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["angle"]         = -thinElementLength/rho;
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = element->e2;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(rho,element->e2,element->fintx);
      (*fringeStOut)["length"]        = thinElementLength;
      G4double fringeAngle            = e2+ 0.5*((*fringeStOut)["angle"]);
      G4String segmentName            = name + "_e2_fringe";
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeAngle, -fringeAngle,
						    segmentName, magType, fringeStOut, brho,
						    integratorSet);
      sbendline->AddComponent(endfringe);
    }
  return sbendline;
}

BDSLine* BDS::BuildRBendLine(const Element*          element,
			     const Element*          prevElement,
			     const Element*          nextElement,
			     G4double                angleIn,
			     G4double                angleOut,
			     const G4double          brho,
			     BDSMagnetStrength*      st,
			     const BDSIntegratorSet* integratorSet,
			     const G4double          charge)
{
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();

  // Angle here is in the 'strength' convention of +ve angle -> -ve x deflection
  G4double      angle = (*st)["angle"];
  // Here we need bending radius to be in correct global carteasian convention, hence -ve.
  G4double bendingRadius = -charge * brho / (*st)["field"];
  G4double  arcLength = std::abs(bendingRadius * angle); // arc length
  const G4String name = element->name;
  G4String    thename = element->name;
  const G4double  rho = bendingRadius; //length / angle;
  const G4double   e1 = element->e1 * CLHEP::rad;
  const G4double   e2 = element->e2 * CLHEP::rad;
  const G4bool yokeOnLeft = BDSComponentFactory::YokeOnLeft(element, st);

  G4bool prevModifies  = false;
  G4bool nextModifies  = false;
    
  BDSLine* rbendline  = new BDSLine(name);
  
  BDSMagnetType magType = BDSMagnetType::rectangularbend;

  // poleface angles
  G4double polefaceAngleIn  = e1 + 0.5*(arcLength-thinElementLength)/rho;
  G4double polefaceAngleOut = e2 + 0.5*(arcLength-thinElementLength)/rho;

  // poleface angles and main element angles are modified if next/previous is an rbend
  // booleans for modification by previous/next element
  if ((prevElement) && (prevElement->type == ElementType ::_RBEND))
    {
      prevModifies = true;
      polefaceAngleIn -= 0.5 * angle;
      angleIn += 0.5*(thinElementLength)/rho;
    }
  if ((nextElement) && (nextElement->type == ElementType ::_RBEND))
    {
      nextModifies = true;
      polefaceAngleOut -= 0.5 * angle;
      angleOut += 0.5*(thinElementLength)/rho;
    }
  
  // first element should be fringe if poleface specified
  if (BDS::IsFinite(e1) && includeFringe &&(!prevModifies))
    {
      BDSMagnetStrength* fringeStIn  = new BDSMagnetStrength();
      (*fringeStIn)["field"]         = (*st)["field"];
      (*fringeStIn)["polefaceangle"] = e1;
      (*fringeStIn)["length"]        = thinElementLength;
      (*fringeStIn)["angle"]         = -thinElementLength/rho;
      (*fringeStIn)["fringecorr"]    = CalculateFringeFieldCorrection(rho, e1, element->fint);
      (*fringeStIn)["fringecorr"]   *= 2*element->hgap*CLHEP::m;
      thename                        = name + "_e1_fringe";
      G4double fringeAngle           = polefaceAngleIn;
      
      BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, -fringeAngle, fringeAngle,
						      thename, magType, fringeStIn, brho,
						      integratorSet);
      rbendline->AddComponent(startfringe);
    }
  
  // subtract thinElementLength from main rbend element if fringe & poleface(s) specified
  if (BDS::IsFinite(e1) && includeFringe && (!prevModifies))
    {
      arcLength -= thinElementLength;
      angleIn   += 0.5*(thinElementLength)/rho;
      angleOut  -= 0.5*(thinElementLength)/rho;
    }
  if (BDS::IsFinite(e2) && includeFringe && (!nextModifies))
    {
      arcLength -= thinElementLength;
      angleOut  += 0.5*(thinElementLength)/rho;
      angleIn   -= 0.5*(thinElementLength)/rho;
    }

  // update the angle as part of the bending covered by the thin fringe part.
  // Length is now shorter.
  angle = -arcLength/rho;
  
  //change angle in the case that the next/prev element modifies
  if (nextModifies)
    {angleOut -= 0.5*(thinElementLength)/rho;}
  if (prevModifies)
    {angleIn  -= 0.5*(thinElementLength)/rho;}
  
  // override copied length and angle
  (*st)["length"] = arcLength;
  (*st)["angle"]  = angle;

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  auto mgInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut, yokeOnLeft);
  mgInfo->name = element->name;

  // Here we change from the strength angle convention of +ve angle corresponds to
  // deflection in negative x, to correct 3d +ve angle corresponds to deflection in
  // positive x. Hence angle sign flip for construction.
  BDSMagnet* oneBend = new BDSMagnet(magType,
				     element->name,
				     arcLength,
				     bpInfo,
				     mgInfo,
				     vacuumField,
				     -angle,
				     nullptr);
  
  rbendline->AddComponent(oneBend);
  
  //Last element should be fringe if poleface specified
  if (BDS::IsFinite(e2) && includeFringe && (!nextModifies))
    {
      BDSMagnetStrength* fringeStOut  = new BDSMagnetStrength();
      (*fringeStOut)["field"]         = (*st)["field"];
      (*fringeStOut)["polefaceangle"] = e2;
      (*fringeStOut)["length"]        = thinElementLength;
      (*fringeStOut)["angle"]         = -thinElementLength / rho;
      (*fringeStOut)["fringecorr"]    = CalculateFringeFieldCorrection(rho, e2, element->fintx);
      (*fringeStOut)["fringecorr"]   *= 2*element->hgap*CLHEP::m;
      thename                         = name + "_e2_fringe";
      G4double fringeAngle            = polefaceAngleOut;
      
      BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, fringeAngle, -fringeAngle,
						    thename, magType, fringeStOut, brho,
						    integratorSet);
      rbendline->AddComponent(endfringe);
    }
  
  return rbendline;
}

BDSMagnet* BDS::BuildDipoleFringe(const GMAD::Element* element,
				  G4double           angleIn,
				  G4double           angleOut,
				  G4String           name,
				  BDSMagnetType      magType,
				  BDSMagnetStrength* st,
				  G4double           brho,
				  const BDSIntegratorSet* integratorSet)
{
  BDSBeamPipeInfo* beamPipeInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  auto magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSIntegratorType intType = integratorSet->dipolefringe;
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       st);

  return new BDSMagnet(magType,
		       name,
		       (*st)["length"],
		       beamPipeInfo,
		       magnetOuterInfo,
		       vacuumField,
		       (*st)["angle"],
		       nullptr);
}

G4int BDS::CalculateNSBendSegments(const G4double length,
				   const G4double angle,
				   const G4double e1,
				   const G4double e2,
				   const G4double aperturePrecision)
{
  // Split a bend into equal segments such that the maximum distance between the
  // chord and arc is 1mm.
  
  // from formula: L/2 / N tan (angle/N) < precision. (L=physical length)
  // add poleface rotations onto angle as absolute number (just to be safe)
  G4double totalAngle = std::abs(angle) + std::abs(e1) + std::abs(e2);
  G4int nSBends = (G4int) ceil(std::sqrt(length*totalAngle/2/aperturePrecision));
  if (nSBends==0)
    {nSBends = 1;} // can happen in case angle = 0
  if (BDSGlobalConstants::Instance()->DontSplitSBends())
    {nSBends = 1;}  // use for debugging
  if (nSBends % 2 == 0)
    {nSBends += 1;} // always have odd number of poles for poleface rotations
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " splitting sbend into " << nSBends << " sbends" << G4endl;
#endif
  return nSBends;
}

BDSMagnet* BDS::BuildSBend(const Element*     element,
			   G4bool             fadeIn,
			   G4bool             fadeOut,
			   G4int              index,
			   G4int              nSBends,
			   BDSMagnetStrength* st,
			   G4double           brho,
			   const BDSIntegratorSet* integratorSet,
			   const G4bool       yokeOnLeft)
{
  G4bool       includeFringe = BDSGlobalConstants::Instance()->IncludeFringeFields();
  G4double thinElementLength = BDSGlobalConstants::Instance()->ThinElementLength();
  
  //calculate their angles and length
  G4double length     = element->l*CLHEP::m;
  G4double semiangle  = (*st)["angle"] / (G4double) nSBends;
  G4double semilength = length / (G4double) nSBends;
  G4double rho        = length / (*st)["angle"];
  
  // angle increment for sbend elements with poleface rotation(s) specified
  G4double deltastart = -element->e1/(0.5*(nSBends-1));
  G4double deltaend   = -element->e2/(0.5*(nSBends-1));

  G4String thename = element->name + "_"+std::to_string(index+1)+"_of_" + std::to_string(nSBends);
  
  // subtract thinElementLength from first and last elements if fringe & poleface specified
  length = semilength;
  if ((BDS::IsFinite(element->e1)) && (index == 0) && includeFringe)
    {length -= thinElementLength;}
  if ((BDS::IsFinite(element->e2)) && (index == nSBends-1) && includeFringe)
    {length -= thinElementLength;}

  // overwritten length use to overwrite semiangle
  semiangle = length/rho;
  
  G4double angleIn  = 0;
  G4double angleOut = 0;
  
  // Input and output angles added to or subtracted from the default as appropriate
  // Note: case of i == 0.5*(nSBends-1) is just the default central wedge.
  // More detailed methodology/reasons in developer manual
  if ((BDS::IsFinite(element->e1))||(BDS::IsFinite(element->e2)))
    {
      if (index < 0.5*(nSBends-1))
	{
	  angleIn  = -semiangle*0.5 - (element->e1 + (index*deltastart));
	  angleOut = -semiangle*0.5 - ((0.5*(nSBends-3)-index)*deltastart);
	}
      else if (index > 0.5*(nSBends-1))
	{
	  angleIn  = -semiangle*0.5 + (0.5*(nSBends+1)-index)*deltaend;
	  angleOut = -semiangle*0.5 - (0.5*(nSBends-1)-index)*deltaend;
	}
    }
  
  if ((BDS::IsFinite(element->e1)) && (index == 0) && includeFringe)
    {angleIn += thinElementLength/rho;}
  if ((BDS::IsFinite(element->e2)) && (index == nSBends-1) && includeFringe)
    {angleOut += thinElementLength/rho;}
  
  //set face angles to default if faces do not fade.
  if (!fadeIn && (index == 0))
    {angleOut = -0.5*semiangle;}
  if (!fadeOut && (index == (nSBends-1)))
    {angleIn = -0.5*semiangle;}
  
  // Check for intersection of angled faces.
  G4double intersectionX = BDS::CalculateFacesOverlapRadius(angleIn,angleOut,semilength);
  auto  magnetOuterInfo = BDSComponentFactory::PrepareMagnetOuterInfo(element, angleIn, angleOut, yokeOnLeft);
  magnetOuterInfo->name = thename;
  G4double magnetRadius= 0.625*magnetOuterInfo->outerDiameter*CLHEP::mm;
  // Every geometry type has a completely arbitrary factor of 1.25 except cylindrical
  if (magnetOuterInfo->geometryType == BDSMagnetGeometryType::cylindrical)
    {magnetRadius= 0.5*magnetOuterInfo->outerDiameter*CLHEP::mm;}
  
  //Check if intersection is within radius
  if ((BDS::IsFinite(intersectionX)) && (std::abs(intersectionX) < magnetRadius))
    {
      G4cerr << __METHOD_NAME__ << "Angled faces of element "<< thename
	     << " intersect within the magnet radius." << G4endl;
      exit(1);
    }
  
  BDSMagnetStrength* stSemi = new BDSMagnetStrength(*st); // copy field strength - ie B
  (*stSemi)["length"] = length;
  (*stSemi)["angle"]  = semiangle;  // override copied length and angle

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::dipole);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::dipole,
					       brho,
					       intType,
					       stSemi);

  auto bpInfo = BDSComponentFactory::PrepareBeamPipeInfo(element, angleIn, angleOut);
  BDSMagnet* oneBend = new BDSMagnet(BDSMagnetType::sectorbend,
				     thename,
				     length,
				     bpInfo,
				     magnetOuterInfo,
				     vacuumField,
				     semiangle);
  
  return oneBend;
}

G4double BDS::CalculateFringeFieldCorrection(G4double rho,
					     G4double polefaceAngle,
					     G4double fint)
{
  G4double term1 = fint/rho;
  G4double term2 = (1.0 + pow(sin(polefaceAngle),2)) / cos(polefaceAngle);
  G4double corrValue = term1*term2;
  return corrValue;
}
