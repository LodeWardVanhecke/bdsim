/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSIMLink.hh"

#include <cstdlib>      // standard headers 
#include <cstdio>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GeometryManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4Version.hh"
#include "G4VModularPhysicsList.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSBunchSixTrackLink.hh"
#include "BDSCavityFactory.hh"
#include "BDSColours.hh"
#include "BDSComponentFactoryUser.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldLoader.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryWriter.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIonDefinition.hh"
#include "BDSLinkDetectorConstruction.hh"
#include "BDSLinkEventAction.hh"
#include "BDSLinkRunAction.hh"
#include "BDSLinkRunManager.hh"
#include "BDSLinkStackingAction.hh"
#include "BDSLinkTrackingAction.hh"
#include "BDSMaterials.hh"
#include "BDSOutput.hh" 
#include "BDSOutputFactory.hh"
#include "BDSParallelWorldUtilities.hh"
#include "BDSParser.hh"
#include "BDSParticleExternal.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSLinkPrimaryGeneratorAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSUtilities.hh"
#include "BDSVisManager.hh"

#include <map>

BDSIMLink::BDSIMLink(BDSBunch* bunchIn):
  ignoreSIGINT(false),
  usualPrintOut(true),
  initialised(false),
  initialisationResult(1),
  argcCache(0),
  argvCache(nullptr),
  parser(nullptr),
  bdsOutput(nullptr),
  bdsBunch(bunchIn),
  runManager(nullptr),
  construction(nullptr),
  runAction(nullptr)
{;}

BDSIMLink::BDSIMLink(int argc, char** argv, bool usualPrintOutIn):
  ignoreSIGINT(false),
  usualPrintOut(usualPrintOutIn),
  initialised(false),
  initialisationResult(1),
  argcCache(argc),
  argvCache(argv),
  parser(nullptr),
  bdsOutput(nullptr),
  bdsBunch(nullptr),
  runManager(nullptr),
  construction(nullptr),
  runAction(nullptr)
{
  initialisationResult = Initialise();
}

int BDSIMLink::Initialise(int argc, char** argv, bool usualPrintOutIn)
{
  argcCache = argc;
  argvCache = argv;
  usualPrintOut = usualPrintOutIn;
  initialisationResult = Initialise();
  return initialisationResult;
}

int BDSIMLink::Initialise()
{
  /// Print header & program information
  G4cout<<"BDSIM : version @BDSIM_VERSION@"<<G4endl;
  G4cout<<"        (C) 2001-@CURRENT_YEAR@ Royal Holloway University London"<<G4endl;
  G4cout<<G4endl;
  G4cout<<"        Reference: https://arxiv.org/abs/1808.10745"<<G4endl;
  G4cout<<"        Website:   http://www.pp.rhul.ac.uk/bdsim"<<G4endl;
  G4cout<<G4endl;

  /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = new BDSExecOptions(argcCache,argvCache);
  if (usualPrintOut)
    {execOptions->Print();}
  ignoreSIGINT = execOptions->IgnoreSIGINT(); // different sig catching for cmake
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "DEBUG mode is on." << G4endl;
#endif

  /// Parse lattice file
  parser = BDSParser::Instance(execOptions->InputFileName());
  /// Update options generated by parser with those from executable options.
  parser->AmalgamateOptions(execOptions->Options());
  parser->AmalgamateBeam(execOptions->Beam(), execOptions->Options().recreate);
  /// Check options for consistency
  parser->CheckOptions();
  
  /// Explicitly initialise materials to construct required materials before global constants.
  BDSMaterials::Instance()->PrepareRequiredMaterials(execOptions->Options().verbose);

  /// No longer needed. Everything can safely use BDSGlobalConstants from now on.
  delete execOptions; 

  /// Force construction of global constants after parser has been initialised (requires
  /// materials first). This uses the options and beam from BDSParser.
  /// Non-const as we'll update the particle definition.
  BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  /// Initialize random number generator
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options

  /// Check geant4 exists in the current environment
  if (!BDS::Geant4EnvironmentIsSet())
    {
      G4cerr << "No Geant4 environmental variables found - please source geant4.sh environment" << G4endl;
      G4cout << "A common fault is the wrong Geant4 environment as compared to the one BDSIM was compiled with." << G4endl;
      return 1;
    }

  /// Construct mandatory run manager (the G4 kernel) and
  /// register mandatory initialization classes.
  runManager = new BDSLinkRunManager();

  /// Register the geometry and parallel world construction methods with run manager.
  construction = new BDSLinkDetectorConstruction();
  runManager->SetUserInitialization(construction);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "> Constructing physics processes" << G4endl;
#endif
  G4String physicsListName = parser->GetOptions().physicsList;

#if G4VERSION_NUMBER > 1049
  // from 10.5 onwards they have a looping particle killer that warnings and kills particles
  // deemed to be looping that are <100 MeV. This is unrelated to the primary energy so troublesome.
  // set to the 'low' limits here ~10keV. This must be done before any physics is created as the
  // parameters are copied into the transportation physics process for each particle and it's very
  // hard to sift through and fix afterwards
  G4PhysicsListHelper::GetPhysicsListHelper()->UseLowLooperThresholds();
#endif
  G4VModularPhysicsList* physList = BDS::BuildPhysics(physicsListName);

  // Construction of the physics lists defines the necessary particles and therefore
  // we can calculate the beam rigidity for the particle the beam is designed w.r.t. This
  // must happen before the geometry is constructed (which is called by
  // runManager->Initialize()).
  BDSParticleDefinition* designParticle = nullptr;
  BDSParticleDefinition* beamParticle = nullptr;
  G4bool beamDifferentFromDesignParticle = false;
  BDS::ConstructDesignAndBeamParticle(BDSParser::Instance()->GetBeam(),
				      globalConstants->FFact(),
				      designParticle,
				      beamParticle,
				      beamDifferentFromDesignParticle);
  if (usualPrintOut)
    {
      G4cout << "Design particle properties: " << G4endl << *designParticle;
      if (beamDifferentFromDesignParticle)
	{G4cout << "Beam particle properties: " << G4endl << *beamParticle;}
    }
  // update rigidity where needed
  construction->SetDesignParticle(designParticle);
  //BDSFieldFactory::SetDesignParticle(designParticle);
  
  //auto biasPhysics = BDS::BuildAndAttachBiasWrapper(parser->GetBiasing());
  //if (biasPhysics)//could be nullptr and can't be passed to geant4 like this
  //  {physList->RegisterPhysics(biasPhysics);}
  runManager->SetUserInitialization(physList);

  /// Instantiate the specific type of bunch distribution.
  GMAD::Beam b;
  b.distrType = "sixtracklink";
  if (!bdsBunch)
    {
      bdsBunch = BDSBunchFactory::CreateBunch(beamParticle,
                                              parser->GetBeam(),
                                              globalConstants->BeamlineTransform(),
                                              globalConstants->BeamlineS(),
                                              globalConstants->GeneratePrimariesOnly());
    }
  else
    {
      bdsBunch->SetOptions(beamParticle, b, BDSBunchType::reference); // update particle definition only
    }
  /// We no longer need beamParticle so delete it to avoid confusion. The definition is
  /// held inside bdsBunch (can be updated dynamically).
  delete beamParticle;
  
  /// Print the geometry tolerance
  G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  if (usualPrintOut)
    {
      G4cout << __METHOD_NAME__ << "Geometry Tolerances: "     << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Surface: " << std::setw(7) << theGeometryTolerance->GetSurfaceTolerance() << " mm"   << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Angular: " << std::setw(7) << theGeometryTolerance->GetAngularTolerance() << " rad"  << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Radial: "  << std::setw(7) << theGeometryTolerance->GetRadialTolerance()  << " mm"   << G4endl;
    }
  /// Set user action classes
  runAction = new BDSLinkRunAction();
  BDSLinkEventAction* eventAction = new BDSLinkEventAction(bdsOutput, runAction);
  runManager->SetUserAction(eventAction);
  runManager->SetUserAction(runAction);
  //G4int verboseSteppingEventStart = globalConstants->VerboseSteppingEventStart();
  //G4int verboseSteppingEventStop  = BDS::VerboseEventStop(verboseSteppingEventStart,
  //                                                        globalConstants->VerboseSteppingEventContinueFor());
  /*runManager->SetUserAction(new BDSLinkTrackingAction(globalConstants->Batch(),
                                                      eventAction,
                                                      verboseSteppingEventStart,
                                                      verboseSteppingEventStop,
                                                      globalConstants->VerboseSteppingPrimaryOnly(),
                                                      globalConstants->VerboseSteppingLevel()));*/
  runManager->SetUserAction(new BDSLinkStackingAction(globalConstants));
  
  /*
  // Only add steppingaction if it is actually used, so do check here (for performance reasons)
  G4int verboseSteppingEventStart = globalConstants->VerboseSteppingEventStart();
  G4int verboseSteppingEventStop  = BDS::VerboseEventStop(verboseSteppingEventStart,
							  globalConstants->VerboseSteppingEventContinueFor());
  if (globalConstants->VerboseSteppingBDSIM())
    {
      runManager->SetUserAction(new BDSSteppingAction(true,
						      verboseSteppingEventStart,
						      verboseSteppingEventStop));
    }
  */
  
  auto primaryGeneratorAction = new BDSLinkPrimaryGeneratorAction(bdsBunch, &currentElementIndex, construction);
  runManager->SetUserAction(primaryGeneratorAction);
  //BDSFieldFactory::SetPrimaryGeneratorAction(primaryGeneratorAction);

  /// Initialize G4 kernel
  runManager->Initialize();
  
  /// Implement bias operations on all volumes only after G4RunManager::Initialize()
  construction->BuildPhysicsBias();

  if (usualPrintOut && BDSGlobalConstants::Instance()->PhysicsVerbose())
    {
      BDS::PrintPrimaryParticleProcesses(bdsBunch->ParticleDefinition()->Name());
      BDS::PrintDefinedParticles();
    }

  /// Set verbosity levels at run and G4 event level. Per event and stepping are controlled
  /// in event, tracking and stepping action. These have to be done here due to the order
  /// of construction in Geant4.
  runManager->SetVerboseLevel(globalConstants->VerboseRunLevel());
  G4EventManager::GetEventManager()->SetVerboseLevel(globalConstants->VerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(globalConstants->VerboseTrackingLevel());
  
  /// Close the geometry in preparation for running - everything is now fixed.
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if (!bCloseGeometry)
    { 
      G4cerr << __METHOD_NAME__ << "error - geometry not closed." << G4endl;
      return 1;
    }

  if (globalConstants->ExportGeometry())
    {
      BDSGeometryWriter geometrywriter;
      geometrywriter.ExportGeometry(globalConstants->ExportType(),
				    globalConstants->ExportFileName());
    }

  const auto& nameInds = construction->NameToElementIndex();
  nameToElementIndex.insert(nameInds.begin(), nameInds.end());

  initialised = true;
  return 0;
}

void BDSIMLink::BeamOn(int nGenerate)
{
  if (initialisationResult > 1 || !initialised)
    {return;} // a mode where we don't do anything

  G4cout.precision(10);
  /// Catch aborts to close output stream/file. perhaps not all are needed.
  struct sigaction act;
  act.sa_handler = &BDS::HandleAborts;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (!ignoreSIGINT)
    {sigaction(SIGINT,  &act, 0);}
  sigaction(SIGABRT, &act, 0);
  sigaction(SIGTERM, &act, 0);
  sigaction(SIGSEGV, &act, 0);
  
  /// Run in either interactive or batch mode
  try
    {
      if (!BDSGlobalConstants::Instance()->Batch())   // Interactive mode
	{
	  BDSVisManager visManager = BDSVisManager(BDSGlobalConstants::Instance()->VisMacroFileName(),
						   BDSGlobalConstants::Instance()->Geant4MacroFileName());
	  visManager.StartSession(argcCache, argvCache);
	}
      else
	{// batch mode
	  if (nGenerate < 0)
	    {runManager->BeamOn(BDSGlobalConstants::Instance()->NGenerate());}
	  else
	    {runManager->BeamOn(nGenerate);}
	}
    }
  catch (const BDSException& exception)
    {
      // don't do this for now in case it's dangerous and we try tracking with open geometry
      //G4GeometryManager::GetInstance()->OpenGeometry();
      G4cout << exception.what() << G4endl;
      exit(1);
    } 
}

BDSIMLink::~BDSIMLink()
{
  /// Termination & clean up.
  G4GeometryManager::GetInstance()->OpenGeometry();
    
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "deleting..." << G4endl;
#endif
  delete bdsOutput;
  delete runAction;

  try
    {
      // order important here because of singletons relying on each other
      delete BDSBeamPipeFactory::Instance();
      delete BDSCavityFactory::Instance();
      delete BDSGeometryFactory::Instance();
      delete BDSAcceleratorModel::Instance();
      delete BDSTemporaryFiles::Instance();
      delete BDSFieldFactory::Instance(); // this uses BDSGlobalConstants which uses BDSMaterials
      delete BDSGlobalConstants::Instance();
      delete BDSMaterials::Instance();
      
      // instances not used in this file, but no other good location for deletion
      if (initialisationResult < 2)
	{
	  delete BDSColours::Instance();
	  delete BDSFieldLoader::Instance();
	  //delete BDSSDManager::Instance();
	  delete BDSSamplerRegistry::Instance();
	}
    }
  catch (...)
    {;} // ignore any exception as this is a destructor

  delete bdsBunch;
  delete parser;

  if (usualPrintOut)
    {G4cout << __METHOD_NAME__ << "End of Run. Thank you for using BDSIM!" << G4endl;}
}

void BDSIMLink::SelectLinkElement(const std::string& elementName)
{
  auto search = nameToElementIndex.find(elementName);
  if (search != nameToElementIndex.end())
    {currentElementIndex = search->second;}
  else
    {currentElementIndex = 0;}
}

void BDSIMLink::SelectLinkElement(int index)
{
  currentElementIndex = index;
}

void BDSIMLink::AddLinkCollimator(const std::string& collimatorName,
				  const std::string& materialName,
				  G4double length,
				  G4double aperture,
				  G4double rotation,
				  G4double xOffset,
				  G4double yOffset)
{
  G4GeometryManager* gm = G4GeometryManager::GetInstance();
  if (gm->IsGeometryClosed())
    {gm->OpenGeometry();}

  construction->AddLinkCollimator(collimatorName,
				  materialName,
				  length,
				  aperture,
				  rotation,
				  xOffset,
				  yOffset);
  nameToElementIndex[collimatorName] = construction->NumberOfElements() - 1;
  
  /// Close the geometry in preparation for running - everything is now fixed.
  G4bool bCloseGeometry = gm->CloseGeometry();
  if (!bCloseGeometry)
    {throw BDSException(__METHOD_NAME__, "error - geometry not closed.");}
}

BDSHitsCollectionSamplerLink* BDSIMLink::SamplerHits() const
{
  if (runAction)
    {return runAction->SamplerHits();}
  else
    {return nullptr;}
}
