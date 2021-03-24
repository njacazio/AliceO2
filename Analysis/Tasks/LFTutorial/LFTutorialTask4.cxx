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
#include "AnalysisDataModel/EventSelection.h"       // Here event selection tables are defined
#include "AnalysisDataModel/Centrality.h"           // Here centrality tables are defined
#include "AnalysisDataModel/PID/PIDResponse.h"      // Here PID response tables are defined
#include "AnalysisDataModel/TrackSelectionTables.h" // Here are defined the tables for the track selection

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

// Example task illustrating how to produce tables for other tasks and how to write them to file (covered by david)

namespace o2::aod
{
namespace lftable
{
DECLARE_SOA_COLUMN(CentV0M, centV0M, float);
DECLARE_SOA_COLUMN(PosZ, posZ, float);
DECLARE_SOA_COLUMN(P, p, float);
DECLARE_SOA_COLUMN(Pt, pt, float);
DECLARE_SOA_COLUMN(TOFNSigmaPi, tofNSigmaPi, float);
} // namespace lftable

DECLARE_SOA_TABLE(LFTable, "AOD", "TheLFTable",
                  lftable::CentV0M,
                  lftable::PosZ,
                  lftable::P,
                  lftable::Pt,
                  lftable::TOFNSigmaPi);

} // namespace o2::aod

struct LFTutorialTask4TableMaker {
  Produces<o2::aod::LFTable> outputTable;

  // Defining configurables from the command line or from the dpl configuration file
  Configurable<float> cfgCutVertex{"cfgCutVertex", 10.0f, "Accepted z-vertex range"};
  Configurable<float> cfgCutEta{"cfgCutEta", 0.8f, "Eta range for tracks"};
  Configurable<float> nsigmacut{"nsigmacut", 3, "Value of the Nsigma cut"};

  // Defining filters
  Filter collisionFilter = nabs(aod::collision::posZ) < cfgCutVertex;
  Filter trackFilter = (nabs(aod::track::eta) < cfgCutEta) && (aod::track::isGlobalTrack == (uint8_t) true) && (aod::track::tofSignal > 0.f) && (nabs(aod::pidtof::tofNSigmaPi) < nsigmacut);

  // Joining the information contained in different tables into one (tables must be mergiable e.g. not collision and tracks).
  // Using filtered versions of the tables
  using CollisionCandidates = soa::Filtered<soa::Join<aod::Collisions, aod::EvSels, aod::Cents>>::iterator;
  using TrackCandidates = soa::Filtered<soa::Join<aod::Tracks, aod::TracksExtra, aod::pidRespTOFPi>>;

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

    for (auto t : inputTracks) {
      outputTable(inputCollision.centV0M(), inputCollision.posZ(), t.p(), t.pt(), t.tofNSigmaPi());
    }
  }
};

struct LFTutorialTask4TableUser {
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

  // Equivalent of the AliRoot task UserExec
  void process(aod::LFTable::iterator const& inputTracks)
  {
    histos.fill(HIST("VtxZAfterSel"), inputTracks.posZ());

    // fill centrality histos
    histos.fill(HIST("Centrality"), inputTracks.centV0M());

    histos.fill(HIST("p"), inputTracks.p());
    histos.fill(HIST("pt"), inputTracks.pt());
    histos.fill(HIST("TOFNsigmaPion"), inputTracks.pt(), inputTracks.tofNSigmaPi());
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  WorkflowSpec workflow{adaptAnalysisTask<LFTutorialTask4TableMaker>(cfgc), adaptAnalysisTask<LFTutorialTask4TableUser>(cfgc)};
  return workflow;
}
