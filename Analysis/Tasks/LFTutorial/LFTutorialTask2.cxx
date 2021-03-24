// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

// O2 includes
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "AnalysisDataModel/EventSelection.h"       // Here the event selection tables are defined
#include "AnalysisDataModel/Centrality.h"           // Here centrality tables are defined
#include "AnalysisDataModel/PID/PIDResponse.h"      // Here PID response tables are defined
#include "AnalysisDataModel/TrackSelectionTables.h" // Here are defined the tables for the track selection

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Example task illustrating how to perform filtering on the event selection and on the PID information
// Fully compatible with hyperloop configuration

struct LFTutorialTask2 {
  HistogramRegistry histos{"Histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  // Equivalent of the AliRoot task UserCreateOutputObjects
  void init(o2::framework::InitContext&)
  {
    // Define your axes
    std::vector<double> centBinning = {0., 1., 5., 10., 20., 30., 40., 50., 70., 100.};
    AxisSpec centAxis = {centBinning, "V0M (%)"};
    AxisSpec vtxZAxis = {100, -20, 20};
    std::vector<double> ptBinning = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.8, 3.2, 3.6, 4., 5., 10., 20.};
    AxisSpec ptAxis = {ptBinning, "#it{p}_{T} (GeV/#it{c})"};

    histos.add("Centrality", "Centrality distribution (V0M)", kTH1F, {centAxis});
    histos.add("VtxZAfterSel", "Vertex distribution in Z;Z (cm)", kTH1F, {vtxZAxis});
    histos.add("p", "Momentum distribution;#it{p} (GeV/#it{c})", kTH1F, {{100, 0, 20}});
    histos.add("pt", "Transverse momentum distribution", kTH1F, {ptAxis});
    histos.add("TOFNsigmaPion", "TOF Nsigma Pion", kTH2F, {ptAxis, {100, -10, 10, "TOF #pi N#sigma"}});
  }

  // Defining configurables from the command line or from the dpl configuration file
  Configurable<float> cfgCutVertex{"cfgCutVertex", 10.0f, "Accepted z-vertex range"};
  Configurable<float> cfgCutEta{"cfgCutEta", 0.8f, "Eta range for tracks"};
  Configurable<float> nsigmacut{"nsigmacut", 3, "Value of the Nsigma cut"};

  // Defining filters
  Filter collisionFilter = (nabs(aod::collision::posZ) < cfgCutVertex.value);

  // Joining the information contained in different tables into one (tables must be mergiable e.g. not collision and tracks).
  // Using filtered versions of the tables
  using CollisionCandidates = soa::Filtered<soa::Join<aod::Collisions, aod::EvSels, aod::Cents>>::iterator; // Note the ::iterator, here we are subscribing to a single column and not the full (filtered) table
  using TrackCandidates = soa::Join<aod::Tracks, aod::TracksExtra, aod::pidRespTOFPi>;                      // Note the absence of ::iterator, here we are subscribing to all tracks in a table. In this way tracks are grouped per collision

  // Equivalent of the AliRoot task UserExec
  void process(CollisionCandidates const& inputCollision, TrackCandidates const& inputTracks)
  {
    // Performing the event selection
    if (!inputCollision.alias()[kINT7]) {
      return;
    }
    if (!inputCollision.sel7()) {
      return;
    }
    histos.fill(HIST("VtxZAfterSel"), inputCollision.posZ());

    // fill centrality histos
    histos.fill(HIST("Centrality"), inputCollision.centV0M());

    // Loop over tracks
    for (auto t : inputTracks) {
      histos.fill(HIST("p"), t.p());
      histos.fill(HIST("pt"), t.pt());
      histos.fill(HIST("TOFNsigmaPion"), t.pt(), t.tofNSigmaPi());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<LFTutorialTask2>(cfgc)};
  return workflow;
}
