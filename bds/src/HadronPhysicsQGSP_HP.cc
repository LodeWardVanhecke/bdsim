#include "HadronPhysicsQGSP_HP.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

HadronPhysicsQGSP_HP::HadronPhysicsQGSP_HP(const G4String& name)
                    :  G4VPhysicsConstructor(name) 
{
  theNeutrons.RegisterMe(&theQGSPNeutron);
  theNeutrons.RegisterMe(&theLEPNeutron);
  theLEPNeutron.SetMaxEnergy(25*GeV);
  theLEPNeutron.SetMinEnergy(19.9*MeV);
  theNeutrons.RegisterMe(&theHPNeutron);

  thePro.RegisterMe(&theQGSPPro);
  thePro.RegisterMe(&theLEPPro);
  theLEPPro.SetMaxEnergy(25*GeV);

  thePiK.RegisterMe(&theQGSPPiK);
  thePiK.RegisterMe(&theLEPPiK);
  theLEPPiK.SetMaxEnergy(25*GeV);
}

HadronPhysicsQGSP_HP::~HadronPhysicsQGSP_HP() {}

void HadronPhysicsQGSP_HP::ConstructParticle()
{
  G4MesonConstructor pMesonConstructor;
  pMesonConstructor.ConstructParticle();

  G4BaryonConstructor pBaryonConstructor;
  pBaryonConstructor.ConstructParticle();

  G4ShortLivedConstructor pShortLivedConstructor;
  pShortLivedConstructor.ConstructParticle();  
}

#include "G4ProcessManager.hh"
void HadronPhysicsQGSP_HP::ConstructProcess()
{
  theNeutrons.Build();
  thePro.Build();
  thePiK.Build();
  theMiscLHEP.Build();
  theStoppingHadron.Build();
  theHadronQED.Build();
}
// 2002 by J.P. Wellisch
