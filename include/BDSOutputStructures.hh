#ifndef BDSOUTPUTSTRUCTURES_H
#define BDSOUTPUTSTRUCTURES_H

#include "BDSOutputROOTEventSampler.hh"

#include "globals.hh"

#include <string>
#include <vector>

// forward declarations
template <class T> class G4THitsCollection;
class BDSEnergyCounterHit;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
class BDSEventInfo;
class BDSGlobalConstants;
class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventLoss;
class BDSOutputROOTEventRunInfo;
class BDSOutputROOTEventTrajectory;
class BDSSamplerHit;
typedef G4THitsCollection<BDSSamplerHit> BDSSamplerHitsCollection;
class BDSTrajectory;
class BDSTrajectoryPoint;
typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

class G4PrimaryVertex;

/**
 * @brief Holder for output information.
 * 
 * This holds instances of the output classes that can be filled
 * and directly copied to the output file.  It also holds and fills
 * the histograms.
 *
 * @author S. Boogert, L. Nevay
 */

class BDSOutputStructures
{
public:
  BDSOutputStructures(const BDSGlobalConstants* globals);
  virtual ~BDSOutputStructures();

  /// Construct samplers.
  void InitialiseGeometryDependent();
  
  /// Clear the local structures in this class in preparation for a new event.
  void ClearStructuresEventLevel();

  /// Clear the local structures in this class in preparation for a new run.
  void ClearStructuresRunLevel();

  /// Create histograms.
  void CreateHistograms();
  
  ///@{ Create histograms for evtHistos and runHistos.
  void Create1DHistogram(G4String name,
			 G4String title,
                         G4int    nbins,
			 G4double xmin,
			 G4double xmax);
  void Create1DHistogram(G4String name,
			 G4String title,
                         std::vector<double>& edges);
  ///@}

  /// Calculate the number of bins and required maximum s.
  void CalculateHistogramParameters();

  /// Primary sampler structure.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* primary;
#else 
  BDSOutputROOTEventSampler<float>* primary;
#endif
  /// Sampler structures.
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> samplerTrees;
#else
  std::vector<BDSOutputROOTEventSampler<float>*> samplerTrees;
#endif
  std::vector<std::string> samplerNames; ///< Sampler names to use.
  
  BDSOutputROOTEventRunInfo*    runInfo;   ///< Run information.
  BDSOutputROOTEventHistograms* runHistos; ///< Run level histograms
  BDSOutputROOTEventLoss*       eLoss;     ///< General energy deposition.
  BDSOutputROOTEventLoss*       pFirstHit; ///< Primary hit point.
  BDSOutputROOTEventLoss*       pLastHit;  ///< Primary loss point.
  BDSOutputROOTEventLoss*       tHit;      ///< Tunnel energy deposition.
  BDSOutputROOTEventTrajectory* traj;      ///< Trajectories.
  BDSOutputROOTEventHistograms* evtHistos; ///< Event level histograms.
  BDSOutputROOTEventInfo*       evtInfo;   ///< Event information.

  /// Whether the single 3D histogram will be built.
  G4bool useScoringMap;

  /// The maximum s in mm such that there is an integer number of
  /// elossHistoBinWidths along the line. Used for histogramming purposes.
  G4double sMaxHistograms;

  /// Number of bins for each histogram required.
  G4int nbins;
  
  /// Wether we've set up the member vector of samplers. Can only be done once the geometry
  /// is constructed so it's done in initialisation. This happens with each new file, but we
  /// should only prepare the local samplers once, hence this cache variable.
  G4bool localSamplersInitialised;

private:
  BDSOutputStructures() = delete;
};

#endif
