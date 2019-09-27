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
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "BDSOutputROOTEventTrajectory.hh"

#ifndef __ROOTBUILD__
#include "G4VPhysicalVolume.hh"

#include "BDSDebug.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSTrajectory.hh"
#endif

ClassImp(BDSOutputROOTEventTrajectory)
BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory():
  auxNavigator(nullptr),
  n(0)
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{
#ifndef __ROOTBUILD__
  delete auxNavigator;
#endif
}

#ifndef __ROOTBUILD__
int findPrimaryStepIndex(BDSTrajectory* traj)
{
  if(!traj->GetParent())
    {return -1;}

  if(traj->GetParent()->GetTrackID() == 1)
    {return traj->GetParentStepIndex();}
  else
    {return findPrimaryStepIndex(traj->GetParent());}
}

void BDSOutputROOTEventTrajectory::Fill(const std::map<BDSTrajectory*, bool>& trajMap)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " ntrajectory=" << trajectories.size() << G4endl;
#endif
  if(!auxNavigator)
    {// navigator for converting coordinates to curvilinear coordinate system
      auxNavigator = new BDSAuxiliaryNavigator();
    }

  // assign trajectory indicies
  int idx = 0;
  for (auto iT = trajMap.begin(); iT != trajMap.end(); ++iT)
    {
      BDSTrajectory* traj = (*iT).first;
      if ((*iT).second) // ie we want to save this trajectory
	{
	  traj->SetTrajIndex(idx);
	  idx++;
	}
      else // we don't want to save it
	{traj->SetTrajIndex(-1);}
    }

  // assign parent (and step) indicies
  for (auto iT = trajMap.begin(); iT != trajMap.end(); ++iT)
    {
      BDSTrajectory* traj   = (*iT).first;
      BDSTrajectory* parent = traj->GetParent();
      if ((*iT).second && parent)
	{ // to store and not primary
	  traj->SetParentIndex(parent->GetTrajIndex());

	  // search for parent step index
	  if (parent->GetTrajIndex() != -1)
	    {
	      auto trajStartPos = traj->GetPoint(0)->GetPosition();
	      traj->SetParentStepIndex(-1);
	      for (auto i = 0; i < parent->GetPointEntries(); ++i)
		{
		  if(parent->GetPoint(i)->GetPosition() == trajStartPos)
		    {
		      traj->SetParentStepIndex(i);
		      break;
		    }
		}
	    }
	  else
	    {parent->SetParentStepIndex(-1);}
	}
      else
	{traj->SetParentIndex(-1);}
    }

  n = 0;
  for (auto iT = trajMap.begin(); iT != trajMap.end(); ++iT)
    {
      BDSTrajectory* traj = (*iT).first;

      // check if the trajectory is to be stored
      if(!(*iT).second)
	{continue;}

      partID.push_back((int &&) traj->GetPDGEncoding());
      trackID.push_back((unsigned int &&) traj->GetTrackID());
      parentID.push_back((unsigned int &&) traj->GetParentID());
      parentIndex.push_back((int &&) traj->GetParentIndex());
      parentStepIndex.push_back((int &&) traj->GetParentStepIndex());

      // now we convert the geant4 type based BDSTrajectory information into
      // basic C++ and ROOT types for the output
      std::vector<int>    preProcessType;
      std::vector<int>    preProcessSubType;
      std::vector<int>    postProcessType;
      std::vector<int>    postProcessSubType;
      std::vector<double> preWeight;
      std::vector<double> postWeight;
      std::vector<double> energyDeposited;

      std::vector<TVector3> position;
      std::vector<TVector3> momentum;
      std::vector<double>   positionS;
      std::vector<int>      modelIndex;
      std::vector<double>   time;

      std::vector<TVector3> localPosition;
      std::vector<TVector3> localMomentum;

      std::vector<int>      charges;
      std::vector<double>   kineticEnergy;
      std::vector<int>      turn;
      std::vector<double>   masses;
      std::vector<double>   rigidities;

      std::vector<bool>     ion;
      std::vector<int>      ionANumber;
      std::vector<int>      ionZNumber;
      std::vector<int>      electrons;

      // loop over trajectory points in this trajectory and fill structures
      for (auto i = 0; i < traj->GetPointEntries(); ++i)
	{
	  BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
	  
	  // Position
	  G4ThreeVector pos = point->GetPosition();
	  position.push_back(TVector3(pos.getX() / CLHEP::m,
				      pos.getY() / CLHEP::m,
				      pos.getZ() / CLHEP::m));

	  G4VPhysicalVolume* vol = auxNavigator->LocateGlobalPointAndSetup(pos,nullptr,true,true,true);
	  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(vol);
	  if(theInfo)
	    {modelIndex.push_back(theInfo->GetBeamlineIndex());}
	  else
	    {modelIndex.push_back(-1);}

	  // Process types
	  preProcessType.push_back(point->GetPreProcessType());
	  preProcessSubType.push_back(point->GetPreProcessSubType());
	  postProcessType.push_back(point->GetPostProcessType());
	  postProcessSubType.push_back(point->GetPostProcessSubType());
	  
	  preWeight.push_back(point->GetPreWeight());
	  postWeight.push_back(point->GetPostWeight());
	  energyDeposited.push_back(point->GetEnergy());
	  G4ThreeVector mom = point->GetPreMomentum() / CLHEP::GeV;
	  momentum.push_back(TVector3(mom.getX(),
				      mom.getY(),
				      mom.getZ()));
	  positionS.push_back(point->GetPreS() / CLHEP::m);
	  time.push_back(point->GetPreGlobalTime() / CLHEP::ns);

	  if (point->extraLocal)
	    {
	      G4ThreeVector localPos = point->GetPositionLocal();
	      G4ThreeVector localMom = point->GetMomentumLocal() / CLHEP::GeV;
	      localPosition.push_back(TVector3(localPos.getX() / CLHEP::m,
					       localPos.getY() / CLHEP::m,
					       localPos.getZ() / CLHEP::m));
	      localMomentum.push_back(TVector3(localMom.getX(),
					       localMom.getY(),
					       localMom.getZ()));
	    }
	  
	  if (point->extraLink)
	    {
	      charges.push_back(point->GetLinkCharge());
	      kineticEnergy.push_back(point->GetLinkKineticEnergy());
	      turn.push_back(point->GetLinkTurnsTaken());
	      masses.push_back(point->GetLinkMass());
	      rigidities.push_back(point->GetLinkRigidity());
	    }
	  
	  if (point->extraIon)
	    {
	      ion.push_back(point->GetIsIon());
	      ionANumber.push_back(point->GetIonA());
	      ionZNumber.push_back(point->GetIonZ());
	      electrons.push_back(point->GetNElectrons());
	    }	  
	}
      
      XYZ.push_back(position);
      modelIndicies.push_back(modelIndex);
      PXPYPZ.push_back(momentum);
      S.push_back(positionS);
      preProcessTypes.push_back(preProcessType);
      preProcessSubTypes.push_back(preProcessSubType);
      postProcessTypes.push_back(postProcessType);
      postProcessSubTypes.push_back(postProcessSubType);
      preWeights.push_back(preWeight);
      postWeights.push_back(postWeight);
      energiesDeposit.push_back(energyDeposited);
      T.push_back(time);

      if (localPosition.size()>0)
	{
          xyz.push_back(localPosition);
          pxpypz.push_back(localMomentum);
	}

      if (charges.size()>0)
	{
          charge.push_back(charges);
          kineticEnergies.push_back(kineticEnergy);
          turnsTaken.push_back(turn);
          mass.push_back(masses);
          rigidity.push_back(rigidities);
	}
      
      if (ion.size()>0)
	{
	  isIon.push_back(ion);
	  ionA.push_back(ionANumber);
          ionZ.push_back(ionZNumber);
          nElectrons.push_back(electrons);
	}
      
      // recursively search for primary interaction step
      primaryStepIndex.push_back(findPrimaryStepIndex(traj));

      // geant4 trackID to trackIndex in this table
      trackID_trackIndex.insert(std::pair<int,int>(traj->GetTrackID(),n));

      // this->printTrajectoryInfo(n);
      n++;
    }
  
#if 0
  // Fill maps for later analysis
  int trackIndex = 0;
  for (auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
    {
      BDSTrajectory *traj = *iT;

      // map of trackID to trackIndex
      trackID_trackIndex.insert(std::pair<int, int>(traj->GetTrackID(),trackIndex));

      std::cout << trajVec.size() << " " << parentID.size() << " " << parentIndex.size() << " "
		<< traj->GetTrackID() << " " << traj->GetParentID() << " " << trackIndex << std::endl;

      // map of trackIndex to trackProcess
      auto processPair = findParentProcess(trackIndex);
      trackIndex_trackProcess.insert(std::pair<int,std::pair<int,int>>(trackIndex,processPair));

      // map of modelIndex to trackProcess
      if(processPair.first != -1)
	{
	  int mi = modelIndicies[processPair.first][processPair.second];
	  trackIndex_modelIndex.insert(std::pair<int,int>(trackIndex, mi));
	  try
	    {modelIndex_trackIndex.at(mi).push_back(trackIndex);}
	  catch(const std::exception&)
	    {
	      modelIndex_trackIndex.insert(std::pair<int,std::vector<int>>(mi,std::vector<int>()));
	      modelIndex_trackIndex.at(mi).push_back(trackIndex);
	    }
	}

      ++trackIndex;
    }
#endif
}

void BDSOutputROOTEventTrajectory::Fill(const BDSHitsCollectionEnergyDeposition* phc)
{
  G4cout << phc->GetSize() << G4endl;
}

#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  n = 0;
  partID.clear();
  trackID.clear();
  parentID.clear();
  parentIndex.clear();
  parentStepIndex.clear();
  primaryStepIndex.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  preWeights.clear();
  postWeights.clear();
  energiesDeposit.clear();
  XYZ.clear();
  S.clear();
  PXPYPZ.clear();
  modelIndicies.clear();
  trackID_trackIndex.clear();
  T.clear();

  xyz.clear();
  pxpypz.clear();
  charge.clear();
  kineticEnergies.clear();
  turnsTaken.clear();
  mass.clear();
  rigidity.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
  nElectrons.clear();

  // trackIndex_trackProcess.clear();
  // trackIndex_modelIndex.clear();
  // modelIndex_trackIndex.clear();
}

void BDSOutputROOTEventTrajectory::Fill(const BDSOutputROOTEventTrajectory* other)
{
  if (!other)
    {return;}

  n = 0;
  partID              = other->partID;
  trackID             = other->trackID;
  parentID            = other->parentID;
  parentIndex         = other->parentIndex;
  parentStepIndex     = other->parentStepIndex;
  primaryStepIndex    = other->primaryStepIndex;
  preProcessTypes     = other->preProcessTypes;
  preProcessSubTypes  = other->preProcessSubTypes;
  postProcessTypes    = other->postProcessTypes;
  postProcessSubTypes = other->postProcessSubTypes;
  preWeights          = other->preWeights;
  postWeights         = other->postWeights;
  energiesDeposit     = other->energiesDeposit;
  XYZ                 = other->XYZ;
  S                   = other->S;
  PXPYPZ              = other->PXPYPZ;
  modelIndicies       = other->modelIndicies;
  trackID_trackIndex  = other->trackID_trackIndex;
  T                   = other->T;

  xyz                 = other->xyz;
  pxpypz              = other->pxpypz;
  charge              = other->charge;
  kineticEnergies     = other->kineticEnergies;
  turnsTaken          = other->turnsTaken;
  rigidity            = other->rigidity;
  isIon               = other->isIon;
  ionA                = other->ionA;
  ionZ                = other->ionZ;
  nElectrons          = other->nElectrons;
}

#if 0
std::pair<int,int> BDSOutputROOTEventTrajectory::findParentProcess(int trackIndex)
{
  std::cout << "BDSOutputROOTEventTrajectory::findParentProcess> "
	    << trackIndex << " " << parentID.size() << " " << parentIndex.size() << std::endl;
  int tid = trackIndex;
  int pid = parentID.at(tid);
  std::cout << pid << std::endl;
  int pin = parentIndex.at(tid);
  std::cout << pin << std::endl;

  if (pin == -1)
    {return std::pair<int,int>(-1,-1);}
  int sin = parentStepIndex.at(tid);
  std::cout << sin << std::endl;

  while (pid > 0)
    {
      if(pin == 0)
	{break;}
      tid = pin;
      pid = parentID.at(tid);
      pin = parentIndex.at(tid);
      sin = parentStepIndex.at(tid);

      std::cout << tid << " " << pid << " " << pin << " " << sin << " " << std::endl;
    }

  return std::pair<int,int>(pin,sin);
}
#endif

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::trackInteractions(int trackid)
{
  int ti = trackID_trackIndex.at(trackid);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv; // trajectory point vector - result

  int nstep = XYZ[ti].size();
  for (int i = 0;i<nstep; ++i)
    {
      int ppt = postProcessTypes[ti][i];
      if (ppt != -1 && ppt != 1 && ppt != 10)
	{
	  BDSOutputROOTEventTrajectoryPoint p(partID[ti], trackID[ti],
					      parentID[ti], parentIndex[ti],
					      postProcessTypes[ti][i], postProcessSubTypes[ti][i],
					      postWeights[ti][i],energiesDeposit[ti][i],
					      XYZ[ti][i], PXPYPZ[ti][i],
					      modelIndicies[ti][i], T[ti][i]);
	  tpv.push_back(p);
	}
    }
  return tpv;
}

BDSOutputROOTEventTrajectoryPoint BDSOutputROOTEventTrajectory::primaryProcessPoint(int trackid)
{
  int ti = trackID_trackIndex.at(trackid);  // get track index
  int si = parentStepIndex.at(ti);          // get primary index

  BDSOutputROOTEventTrajectoryPoint p(partID[ti], trackID[ti],
                                      parentID[ti], parentIndex[ti],
                                      postProcessTypes[ti][si], postProcessSubTypes[ti][si],
                                      postWeights[ti][si],energiesDeposit[ti][si],
                                      XYZ[ti][si], PXPYPZ[ti][si],
                                      modelIndicies[ti][si], T[ti][si]);
  return p;
}

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::processHistory(int trackid)
{
  int ti = trackID_trackIndex.at(trackid);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv;      // trajectory point vector
  while (ti != 0)
    {
      int pi  = parentIndex.at(ti);
      int psi = parentStepIndex.at(ti);
      
      BDSOutputROOTEventTrajectoryPoint p(partID[pi], trackID[pi],
					  parentID[pi], parentIndex[pi],
					  postProcessTypes[pi][psi], postProcessSubTypes[pi][psi],
					  postWeights[pi][psi],energiesDeposit[pi][psi],
					  XYZ[pi][psi], PXPYPZ[pi][psi] ,
					  modelIndicies[pi][psi], T[pi][psi]);
      tpv.push_back(p);
      ti = pi;
    }
  std::reverse(tpv.begin(),tpv.end());
  return tpv;
}

void BDSOutputROOTEventTrajectory::printTrajectoryInfo(int i)
{
  int wdt = 11;

  std::cout << std::setw(wdt) << "trIx"      << " " << std::setw(wdt) << "trkId"    << " "
            << std::setw(wdt) << "prId"      << " " << std::setw(wdt) << "prIx"    << " "
            << std::setw(wdt) << "prStpIx"   << " " << std::setw(wdt) << "pID"      << " "
            << std::setw(wdt) << "prePrcT"   << " " << std::setw(wdt) << "prePrcST" << " "
            << std::setw(wdt) << "pstPrcT"   << " " << std::setw(wdt) << "pstPrcST" << " "
            << std::setw(wdt) << "X"         << " " << std::setw(wdt) << "Y"        << " "
            << std::setw(wdt) << "Z"         << " " << std::setw(wdt) << "E"        << " "
            << std::setw(wdt) << "p"         << " " << std::setw(wdt) << "p_x"      << " "
            << std::setw(wdt) << "p_y"       << " " << std::setw(wdt) << "p_z"      << " "
            << std::setw(wdt) << "t"         << std::endl;

  for (size_t j=0; j<XYZ[i].size(); ++j)
    {
      std::cout << std::setw(wdt) << j << " " << std::setw(wdt) <<  trackID[i] << " "
		<< std::setw(wdt) << parentID[i]            << " " << std::setw(wdt) << parentIndex[i]           << " "
		<< std::setw(wdt) << parentStepIndex[i]     << " " << std::setw(wdt) << partID[i]                << " "
		<< std::setw(wdt) << preProcessTypes[i][j]  << " " << std::setw(wdt) << preProcessSubTypes[i][j] << " "
		<< std::setw(wdt) << postProcessTypes[i][j] << " " << std::setw(wdt) << postProcessSubTypes[i][j]<< " "
		<< std::setw(wdt) << XYZ[i][j].X() << " " << std::setw(wdt) << XYZ[i][j].Y()   << " "
		<< std::setw(wdt) << XYZ[i][j].Z() << " " << std::setw(wdt) << energiesDeposit[i][j]           << " "
		<< std::setw(wdt) << PXPYPZ[i][j].Mag()    << " " << std::setw(wdt) << PXPYPZ[i][j].X()        << " "
		<< std::setw(wdt) << PXPYPZ[i][j].Y()      << " " << std::setw(wdt) << PXPYPZ[i][j].Z()        << " "
		<< std::setw(wdt) << T[i][j]             << std::endl;
    }
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for (int i=0; i< (int)t.preProcessTypes.size();++i)
    {
      for (int j=0; j< (int)t.preProcessTypes[i].size(); ++j)
	{
	  //if(t.preProcessTypes[i][j] != 1 && t.preProcessTypes[i][j] != 7)
	  //{
	  out << i << " " << j
	      << " " << t.preProcessTypes[i][j]   << " " << t.preProcessSubTypes[i][j]
	      << " " << t.postProcessTypes[i][j]  << " " << t.postProcessSubTypes[i][j]
	      << " " << t.preWeights[i][j]        << " " << t.postWeights[i][j]
	      << " " << t.energiesDeposit[i][j]          << " " << t.T[i][j]
	      << std::endl;
	  //}
	}
    }
  return out;
}
