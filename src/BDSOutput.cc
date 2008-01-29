#include "BDSOutput.hh"
#include "BDSSamplerSD.hh"
#include <ctime>

extern G4String outputFilename;
BDSOutput::BDSOutput()
{
  format = _ASCII; // default - write an ascii file
}

BDSOutput::BDSOutput(int fmt)
{
  format = fmt;
}

BDSOutput::~BDSOutput()
{
  if(of != NULL)
    of.close();


#ifdef USE_ROOT
  if(format==_ROOT)
    if(theRootOutputFile->IsOpen())
      {
	theRootOutputFile->Write();
	theRootOutputFile->Close();
	delete theRootOutputFile;
      }
#endif
}


void BDSOutput::SetFormat(G4int val)
{
  format = val;
  time_t tm = time(NULL);

  if( format == _ASCII)
    {
      G4String filename = outputFilename+".txt";
      G4cout<<"output format ASCII, filename: "<<filename<<G4endl;
      of.open(filename);
      of<<"### BDSIM output created "<<ctime(&tm)<<" ####"<<G4endl;
      of<<"# PT E[GeV] X[mum] Y[mum] Z[m] Xp[rad] Yp[rad]  "<<G4endl;

    }
  if( format == _ROOT)
    {
      G4cout<<"output format ROOT"<<G4endl;
    }
}

// output initialization (ROOT only)
void BDSOutput::Init(G4int FileNum)
{
  if(format != _ROOT) return;

#ifdef USE_ROOT
  // set up the root file
  G4String filename = outputFilename + "_" 
    + BDSGlobals->StringFromInt(FileNum) + ".root";
  
  G4cout<<"Setting up new file: "<<filename<<G4endl;
  theRootOutputFile=new TFile(filename,"RECREATE", "BDS output file");
  

  //build sampler tree
  for(G4int i=0;i<BDSSampler::GetNSamplers();i++)
    {
      //G4String name="samp"+BDSGlobals->StringFromInt(i+1);
      G4String name=SampName[i];
      TTree* SamplerTree = new TTree(name, "Sampler output");
      
      SamplerTree->Branch("E0",&E0,"E0 (GeV)/F");
      SamplerTree->Branch("x0",&x0,"x0 (mum)/F");
      SamplerTree->Branch("y0",&y0,"y0 (mum)/F");
      SamplerTree->Branch("z0",&z0,"z0 (m)/F");
      SamplerTree->Branch("xp0",&xp0,"xp0 (rad)/F");
      SamplerTree->Branch("yp0",&yp0,"yp0 (rad)/F");
      SamplerTree->Branch("zp0",&zp0,"zp0 (rad)/F");
      SamplerTree->Branch("t0",&t0,"t0 (ns)/F");

      SamplerTree->Branch("E",&E,"E (GeV)/F");
      SamplerTree->Branch("x",&x,"x (mum)/F");
      SamplerTree->Branch("y",&y,"y (mum)/F");
      SamplerTree->Branch("z",&z,"z (m)/F");
      SamplerTree->Branch("xp",&xp,"xp (rad)/F");
      SamplerTree->Branch("yp",&yp,"yp (rad)/F");
      SamplerTree->Branch("zp",&zp,"zp (rad)/F");
      SamplerTree->Branch("t",&t,"t (ns)/F");

      SamplerTree->Branch("X",&X,"X (mum)/F");
      SamplerTree->Branch("Y",&Y,"Y (mum)/F");
      SamplerTree->Branch("Z",&Z,"Z (m)/F");
      SamplerTree->Branch("Xp",&Xp,"Xp (rad)/F");
      SamplerTree->Branch("Yp",&Yp,"Yp (rad)/F");
      SamplerTree->Branch("Zp",&Zp,"Zp (rad)/F");

      SamplerTree->Branch("s",&s,"s (m)/F");

      SamplerTree->Branch("weight",&weight,"weight/F");
      SamplerTree->Branch("partID",&part,"partID/I");
      SamplerTree->Branch("nEvent",&nev,"nEvent/I");
      SamplerTree->Branch("parentID",&pID,"parentID/I");
      SamplerTree->Branch("trackID",&track_id,"trackID/I");
    }

  if(BDSGlobals->GetStoreTrajectory() || BDSGlobals->GetStoreMuonTrajectories() || BDSGlobals->GetStoreNeutronTrajectories()) 
    // create a tree with trajectories
    {
      //G4cout<<"BDSOutput::storing trajectories set"<<G4endl;
      TTree* TrajTree = new TTree("Trajectories", "Trajectories");
      TrajTree->Branch("x",&x,"x/F");
      TrajTree->Branch("y",&y,"y/F");
      TrajTree->Branch("z",&z,"z/F");
      TrajTree->Branch("part",&part,"part/I");
    }

  // build energy loss histogram
  G4int nBins = G4int(zMax/m);

  EnergyLossHisto = new TH1F("ElossHisto", "Energy Loss",nBins,0.,zMax/m);
  EnergyLossNtuple= new TNtuple("ElossNtuple", "Energy Loss","z:E:partID:parentID");

#endif
}

void BDSOutput::WriteHits(BDSSamplerHitsCollection *hc)
{
  if( format == _ASCII) {
    
    //of<<"#hits (PDGtype  E[GeV],x[micron],y[micron],z[m],x'[rad],y'[rad]):"<<G4endl;
    
    G4cout.precision(6);
    
    for (G4int i=0; i<hc->entries(); i++)
      {
	of<<(*hc)[i]->GetPDGtype()
	  <<" "
	  <<(*hc)[i]->GetMom()/GeV
	  <<" "
	  <<(*hc)[i]->GetX()/micrometer
	  <<" "
	  <<(*hc)[i]->GetY()/micrometer
	  <<" "
	  <<(*hc)[i]->GetZ() / m
	  <<" "
	  <<(*hc)[i]->GetXPrime() / radian
	  <<" "
	  <<(*hc)[i]->GetYPrime() / radian
	  <<G4endl;
      }
    
    of<<G4endl;
    //of<<"end of hits collection"<<G4endl;
  }
 
  if( format == _ROOT) {
#ifdef USE_ROOT
    G4String name;


    for (G4int i=0; i<hc->entries(); i++)
     {
       
       //if ((*hc)[i]->GetType()=="plane") 
       //name="samp";
       //else if ((*hc)[i]->GetType()=="cylinder")
       //name ="cyln";
       //name="samp" + BDSGlobals->StringFromInt((*hc)[i]->GetNumber());

       TTree* sTree=(TTree*)gDirectory->Get((*hc)[i]->GetName());
       
       if(!sTree) G4Exception("BDSOutput: ROOT Sampler not found!");

       E0=(*hc)[i]->GetInitMom() / GeV;
       x0=(*hc)[i]->GetInitX() / micrometer;
       y0=(*hc)[i]->GetInitY() / micrometer;
       z0=(*hc)[i]->GetInitZ() / m;
       xp0=(*hc)[i]->GetInitXPrime() / radian;
       yp0=(*hc)[i]->GetInitYPrime() / radian;
       zp0=(*hc)[i]->GetInitZPrime() / radian;
       t0=(*hc)[i]->GetInitT() / ns;

       E=(*hc)[i]->GetMom() / GeV;
       x=(*hc)[i]->GetX() / micrometer;
       y=(*hc)[i]->GetY() / micrometer;
       z=(*hc)[i]->GetZ() / m;
       xp=(*hc)[i]->GetXPrime() / radian;
       yp=(*hc)[i]->GetYPrime() / radian;
       zp=(*hc)[i]->GetZPrime() / radian;
       t=(*hc)[i]->GetT() / ns;

       X=(*hc)[i]->GetGlobalX() / m;
       Y=(*hc)[i]->GetGlobalY() / m;
       Z=(*hc)[i]->GetGlobalZ() / m;
       Xp=(*hc)[i]->GetGlobalXPrime() / radian;
       Yp=(*hc)[i]->GetGlobalYPrime() / radian;
       Zp=(*hc)[i]->GetGlobalZPrime() / radian;

       s=(*hc)[i]->GetS() / m;

       weight=(*hc)[i]->GetWeight();
       part=(*hc)[i]->GetPDGtype(); 
       nev=(*hc)[i]->GetEventNo(); 
       pID=(*hc)[i]->GetParentID(); 
       track_id=(*hc)[i]->GetTrackID();
      
       sTree->Fill();
       
     }
#endif
  }
 
}

// write a trajectory to a root/ascii file
G4int BDSOutput::WriteTrajectory(TrajectoryVector* TrajVec)
{
//  G4cout<<"a trajectory stored..."<<G4endl;
  
  G4int tID;
  G4TrajectoryPoint* TrajPoint;
  G4ThreeVector TrajPos;

#ifdef USE_ROOT
  TTree* TrajTree;
    
  G4String name = "Trajectories";
      
  TrajTree=(TTree*)gDirectory->Get(name);

  if(TrajTree == NULL) { G4cerr<<"TrajTree=NULL"<<G4endl; return -1;}
#endif


  if(TrajVec)
    {
      TrajectoryVector::iterator iT;
      
      for(iT=TrajVec->begin();iT<TrajVec->end();iT++)
	{
	  G4Trajectory* Traj=(G4Trajectory*)(*iT);
	  
	  tID=Traj->GetTrackID();	      
	  part = Traj->GetPDGEncoding();
	  
	  for(G4int j=0; j<Traj->GetPointEntries(); j++)
	    {
	      TrajPoint=(G4TrajectoryPoint*)Traj->GetPoint(j);
	      TrajPos=TrajPoint->GetPosition();
	      
	      x = TrajPos.x() / m;
	      y = TrajPos.y() / m;
	      z = TrajPos.z() / m;

#ifdef USE_ROOT
	      if( format == _ROOT) 
		TrajTree->Fill();
#endif

	      //G4cout<<"TrajPos="<<TrajPos<<G4endl;
	    }
	}
    }
  
  return 0;
}

// make energy loss histo

void BDSOutput::WriteEnergyLoss(BDSEnergyCounterHitsCollection* hc)
{

  if( format == _ROOT) {
#ifdef USE_ROOT
  
    G4int n_hit = hc->entries();
    
    for (G4int i=0;i<n_hit;i++)
      {
	G4double Energy=(*hc)[i]->GetEnergy();
	G4double EWeightZ=(*hc)[i]->
	  GetEnergyWeightedPosition()/Energy;
	G4int partID = (*hc)[i]->GetPartID();
	G4int parentID = (*hc)[i]->GetParentID();
	EnergyLossHisto->Fill(EWeightZ/m,Energy/GeV);

	EnergyLossNtuple->Fill(EWeightZ/m,Energy/GeV,partID,parentID);

      }
#endif
  }

 if( format == _ASCII) {
  
    G4int n_hit = hc->entries();
    
    for (G4int i=0;i<n_hit;i++)
      {
	G4double Energy=(*hc)[i]->GetEnergy();
	G4double EWeightZ=(*hc)[i]->
	  GetEnergyWeightedPosition()/Energy;
	G4int partID = (*hc)[i]->GetPartID();
	G4int parentID = (*hc)[i]->GetParentID();
	
	of<<EWeightZ/m<<"  "<<Energy/GeV<<"  "<<partID<<"  "<<parentID<<G4endl;

      }

  }

}



// write some comments to the output file
// only for ASCII output
void BDSOutput::Echo(G4String str)
{
  if(format == _ASCII)  of<<"#"<<str<<G4endl;
  else // default
    G4cout<<"#"<<str<<G4endl;
}

G4int BDSOutput::Commit(G4int FileNum)
{
#ifdef USE_ROOT
  if(format==_ROOT)
    if(theRootOutputFile->IsOpen())
      {
	theRootOutputFile->Write();
	theRootOutputFile->Close();
	delete theRootOutputFile;
      }
#endif

  Init(FileNum);
  return 0;
}

