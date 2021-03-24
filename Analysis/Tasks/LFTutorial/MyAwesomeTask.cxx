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

using namespace o2;
using namespace o2::framework;

struct MyAwesomeTask { // This is a task
  // The histogram registry is the container of the output histograms
  HistogramRegistry histos{"Histos", {}, OutputObjHandlingPolicy::AnalysisObject};

  // Equivalent of the AliRoot task UserCreateOutputObjects
  void init(o2::framework::InitContext&)
  {
    // Histogram is added to the ouput registry
    histos.add("p", "Momentum distribution;#it{p} (GeV/#it{c})", kTH1F, {{100, 0, 20}});
  }

  // Equivalent of the AliRoot task UserExec
  void process(aod::Tracks const& inputTracks)
  {
    for (auto track : inputTracks) { // Loop over tracks
      histos.fill(HIST("p"), track.p());
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc) // This puts your task in the DPL workflow
{
  // Equivalent to the AddTask in AliPhysics
  WorkflowSpec workflow{adaptAnalysisTask<MyAwesomeTask>(cfgc)};
  return workflow;
}
