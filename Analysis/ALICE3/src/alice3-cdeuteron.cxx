// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \author Nicolo' Jacazio <nicolo.jacazio@cern.ch>, CERN
/// \author Alexander Kalweit <alexander.kalweit@cern.ch>, CERN

// O2 includes
#include "Framework/AnalysisTask.h"
#include "Framework/runDataProcessing.h"
#include "AnalysisCore/MC.h"
#include "TLorentzVector.h"
#include "Framework/HistogramRegistry.h"
#include "ReconstructionDataFormats/PID.h"
#include "AnalysisCore/RecoDecay.h"
#include "DetectorsVertexing/DCAFitterN.h"
#include "AnalysisCore/trackUtilities.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct Alice3CDeuteron {
  Configurable<float> magField{"magField", 0.5, "Magnetic field"};
  Configurable<float> minRadius{"minRadius", 0, "Minimum decay radius"};
  Configurable<float> maxRadius{"maxRadius", 0.1, "Maximum decay radius"};
  Configurable<float> minMomPt{"minMomPt", 0.0, "Minimum pT of the mother"};
  Configurable<float> minKaonPt{"minKaonPt", 0.0, "Minimum pT of the pion daughter"};
  Configurable<float> minPionPt{"minPionPt", 0.0, "Minimum pT of the kaon daughter"};
  Configurable<float> minVtxContrib{"minVtxContrib", 3, "Minimum number of contributors to the primary vertex"};
  Configurable<float> minDca{"minDca", 0.0001, "Minimum track DCA to the primary vertex"};
  Configurable<float> maxDca{"maxDca", 0.001, "Maximum track DCA to the primary vertex"};
  HistogramRegistry histos{"Histos", {}, OutputObjHandlingPolicy::AnalysisObject};
  o2::vertexing::DCAFitterN<3> fitter;

  void init(InitContext&)
  {

    fitter.setBz(magField);
    fitter.setPropagateToPCA(true);
    fitter.setMaxR(1.);
    fitter.setMinParamChange(1e-3);
    fitter.setMinRelChi2Change(0.9);
    fitter.setMaxDZIni(1e9);
    fitter.setMaxChi2(1e9);
    fitter.setUseAbsDCA(true);

    const AxisSpec axisInvMass{100, 2.5, 4, "Inv. Mass_{c-d}"};
    const AxisSpec axisDecayRadius{2000, 0, 0.1, "Decay radius"};
    const AxisSpec axisDecayRadiusReso{2000, -0.01, 0.01, "Decay radius resolution"};
    const AxisSpec axisDca{5000, -0.01, 0.01, "DCA to secondary"};
    const AxisSpec axisDcaXY{5000, -0.05, 0.05, "DCA_{xy}"};
    const AxisSpec axisDcaZ{5000, -0.05, 0.05, "DCA_{z}"};
    const TString tit = Form(" [%.6f, %.6f] R [%.6f, %.6f] DCA ",
                             minRadius.value, maxRadius.value,
                             minDca.value, maxDca.value);

    histos.add("sig/invmass", "invmass" + tit, kTH1D, {axisInvMass});
    histos.add("sig/decayradius", "decayradius" + tit, kTH1D, {axisDecayRadius});
    histos.add("sig/decayradiusResoX", "decayradiusResoX" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("sig/decayradiusResoY", "decayradiusResoY" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("sig/decayradiusResoZ", "decayradiusResoZ" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("sig/decayradiusReso", "decayradiusReso" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("sig/decaydca0", "decaydca0" + tit, kTH1D, {axisDca});
    histos.add("sig/decaydca1", "decaydca1" + tit, kTH1D, {axisDca});
    histos.add("sig/dcaxy1", "dcaxy1 Deuteron" + tit, kTH1D, {axisDcaXY});
    histos.add("sig/dcaxy2", "dcaxy2 Kaon" + tit, kTH1D, {axisDcaXY});
    histos.add("sig/dcaxy3", "dcaxy3 Pion" + tit, kTH1D, {axisDcaXY});
    histos.add("sig/dcaxy1xdcaxy2", "dcaxy1xdcaxy2" + tit, kTH1D, {axisDcaXY});
    histos.add("sig/dcaxy3xdcaxy2", "dcaxy3xdcaxy2" + tit, kTH1D, {axisDcaXY});
    histos.add("sig/dcaz1", "dcaz1 Deuteron" + tit, kTH1D, {axisDcaZ});
    histos.add("sig/dcaz2", "dcaz2 Kaon" + tit, kTH1D, {axisDcaZ});
    histos.add("sig/dcaz3", "dcaz3 Pion" + tit, kTH1D, {axisDcaZ});
    histos.add("sig/dcaz1xdcaz2", "dcaz1xdcaz2" + tit, kTH1D, {axisDcaZ});
    histos.add("sig/dcaz3xdcaz2", "dcaz3xdcaz2" + tit, kTH1D, {axisDcaZ});

    histos.add("bkg/invmass", "invmass" + tit, kTH1D, {axisInvMass});
    histos.add("bkg/decayradius", "decayradius" + tit, kTH1D, {axisDecayRadius});
    histos.add("bkg/decayradiusResoX", "decayradiusResoX" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("bkg/decayradiusResoY", "decayradiusResoY" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("bkg/decayradiusResoZ", "decayradiusResoZ" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("bkg/decayradiusReso", "decayradiusReso" + tit, kTH1D, {axisDecayRadiusReso});
    histos.add("bkg/decaydca0", "decaydca0" + tit, kTH1D, {axisDca});
    histos.add("bkg/decaydca1", "decaydca1" + tit, kTH1D, {axisDca});
    histos.add("bkg/dcaxy1", "dcaxy1 Deuteron" + tit, kTH1D, {axisDcaXY});
    histos.add("bkg/dcaxy2", "dcaxy2 Kaon" + tit, kTH1D, {axisDcaXY});
    histos.add("bkg/dcaxy3", "dcaxy3 Pion" + tit, kTH1D, {axisDcaXY});
    histos.add("bkg/dcaxy1xdcaxy2", "dcaxy1xdcaxy2" + tit, kTH1D, {axisDcaXY});
    histos.add("bkg/dcaxy3xdcaxy2", "dcaxy3xdcaxy2" + tit, kTH1D, {axisDcaXY});
    histos.add("bkg/dcaz1", "dcaz1 Deuteron" + tit, kTH1D, {axisDcaZ});
    histos.add("bkg/dcaz2", "dcaz2 Kaon" + tit, kTH1D, {axisDcaZ});
    histos.add("bkg/dcaz3", "dcaz3 Pion" + tit, kTH1D, {axisDcaZ});
    histos.add("bkg/dcaz1xdcaz2", "dcaz1xdcaz2" + tit, kTH1D, {axisDcaZ});
    histos.add("bkg/dcaz3xdcaz2", "dcaz3xdcaz2" + tit, kTH1D, {axisDcaZ});
  }

  void process(const soa::Join<o2::aod::Collisions, o2::aod::McCollisionLabels>::iterator& coll,
               const o2::aod::McCollisions& Mccoll,
               const soa::Join<o2::aod::Tracks, o2::aod::McTrackLabels, o2::aod::TracksExtra, o2::aod::TracksCov>& tracks,
               const aod::McParticles& mcParticles)
  {
    for (const auto& mcParticle : mcParticles) {
      // ParticlesOfInterest.push_back(mcParticle.globalIndex());
    }

    std::array<float, 2> dca1{1e10f, 1e10f};
    std::array<float, 2> dca2{1e10f, 1e10f};
    std::array<float, 2> dca3{1e10f, 1e10f};
    for (const auto& track1 : tracks) {
      const auto index1 = track1.globalIndex();
      if (track1.mcParticle().pdgCode() != 1000010020) {
        continue;
      }
      if (!getTrackPar(track1).propagateParamToDCA({coll.posX(),
                                                    coll.posY(),
                                                    coll.posZ()},
                                                   magField * 10.f, &dca1, 100.)) {
        continue;
      }

      if (abs(dca1[0]) < minDca || abs(dca1[1]) < minDca) {
        continue;
      }
      if (abs(dca1[0]) > maxDca || abs(dca1[1]) > maxDca) {
        continue;
      }
      for (const auto& track2 : tracks) {
        const auto index2 = track2.globalIndex();
        if (index1 == index2) {
          continue;
        }
        if (track2.mcParticle().pdgCode() != -321) {
          continue;
        }
        if (track2.pt() < minKaonPt) {
          continue;
        }
        if (!getTrackPar(track2).propagateParamToDCA({coll.posX(),
                                                      coll.posY(),
                                                      coll.posZ()},
                                                     magField * 10.f, &dca2, 100.)) {
          continue;
        }

        if (abs(dca2[0]) < minDca || abs(dca2[1]) < minDca) {
          continue;
        }
        if (abs(dca2[0]) > maxDca || abs(dca2[1]) > maxDca) {
          continue;
        }

        for (const auto& track3 : tracks) {
          const auto index3 = track3.globalIndex();
          if (index2 == index3) {
            continue;
          }
          if (index1 == index3) {
            continue;
          }
          if (track3.mcParticle().pdgCode() != 211) {
            continue;
          }
          if (track3.pt() < minPionPt) {
            continue;
          }
          if (!getTrackPar(track3).propagateParamToDCA({coll.posX(),
                                                        coll.posY(),
                                                        coll.posZ()},
                                                       magField * 10.f, &dca3, 100.)) {
            continue;
          }

          if (abs(dca3[0]) < minDca || abs(dca3[1]) < minDca) {
            continue;
          }
          if (abs(dca3[0]) > maxDca || abs(dca3[1]) > maxDca) {
            continue;
          }

          const auto mother1 = track1.mcParticle().mother0_as<aod::McParticles>();
          const auto mother2 = track2.mcParticle().mother0_as<aod::McParticles>();
          const auto mother3 = track3.mcParticle().mother0_as<aod::McParticles>();
          bool issig = true;
          if (mother1 != mother2) {
            issig = false;
          }
          if (mother1 != mother3) {
            issig = false;
          }

          auto pc1 = getTrackParCov(track1);
          auto pc2 = getTrackParCov(track2);
          auto pc3 = getTrackParCov(track3);
          if (pc1.getSigmaY2() * pc1.getSigmaZ2() - pc1.getSigmaZY() * pc1.getSigmaZY() < 0.) {
            Printf("Track 1 has issues");
            continue;
          }
          if (pc2.getSigmaY2() * pc2.getSigmaZ2() - pc2.getSigmaZY() * pc2.getSigmaZY() < 0.) {
            Printf("Track 2 has issues");
            continue;
          }
          if (pc3.getSigmaY2() * pc3.getSigmaZ2() - pc3.getSigmaZY() * pc3.getSigmaZY() < 0.) {
            Printf("Track 3 has issues");
            continue;
          }
          const int status = fitter.process(pc1, pc2, pc3);
          if (status != 0) {

            TLorentzVector v1{};
            v1.SetPtEtaPhiM(track1.pt(), track1.eta(), track1.phi(), 1.8756129);

            TLorentzVector v2{};
            v2.SetPtEtaPhiM(track2.pt(), track2.eta(), track2.phi(), 0.493677);

            TLorentzVector v3{};
            v3.SetPtEtaPhiM(track3.pt(), track3.eta(), track3.phi(), 0.139570);
            v1 += v2;
            v1 += v3;
            if (v1.Pt() < minMomPt) {
              continue;
            }
            if (issig) {
              histos.fill(HIST("sig/invmass"), v1.M());
            } else {
              histos.fill(HIST("bkg/invmass"), v1.M());
            }

            // fitter.propagateTracksToVertex();
            const auto& secVtx = fitter.getPCACandidate();
            const float r = sqrt(secVtx[0] * secVtx[0] + secVtx[1] * secVtx[1] + secVtx[2] * secVtx[2]);
            if (r < minRadius) {
              continue;
            }
            if (r > maxRadius) {
              continue;
            }

            const float vx = mother1.vx();
            const float vy = mother1.vy();
            const float vz = mother1.vz();
            const float rmc = sqrt((secVtx[0] - vx) * (secVtx[0] - vx) + (secVtx[1] - vy) * (secVtx[1] - vy) + (secVtx[2] - vz) * (secVtx[2] - vz));
            if (issig) {
              histos.fill(HIST("sig/decayradius"), r);
              histos.fill(HIST("sig/decayradiusResoX"), secVtx[0] - vx);
              histos.fill(HIST("sig/decayradiusResoY"), secVtx[1] - vy);
              histos.fill(HIST("sig/decayradiusResoZ"), secVtx[2] - vz);
              histos.fill(HIST("sig/decayradiusReso"), rmc);
              histos.fill(HIST("sig/decaydca0"), TMath::Sqrt(fitter.getChi2AtPCACandidate(0)));
              histos.fill(HIST("sig/decaydca1"), TMath::Sqrt(fitter.getChi2AtPCACandidate(1)));

              histos.fill(HIST("sig/dcaxy1"), dca1[0]);
              histos.fill(HIST("sig/dcaz1"), dca1[1]);
              histos.fill(HIST("sig/dcaxy2"), dca2[0]);
              histos.fill(HIST("sig/dcaz2"), dca2[1]);
              histos.fill(HIST("sig/dcaxy3"), dca3[0]);
              histos.fill(HIST("sig/dcaz3"), dca3[1]);

              histos.fill(HIST("sig/dcaxy1xdcaxy2"), dca1[0] * dca2[0]);
              histos.fill(HIST("sig/dcaz1xdcaz2"), dca1[1] * dca2[1]);

              histos.fill(HIST("sig/dcaxy3xdcaxy2"), dca3[0] * dca2[0]);
              histos.fill(HIST("sig/dcaz3xdcaz2"), dca3[1] * dca2[1]);

            } else {
              histos.fill(HIST("bkg/decayradius"), r);
              histos.fill(HIST("bkg/decayradiusResoX"), secVtx[0] - vx);
              histos.fill(HIST("bkg/decayradiusResoY"), secVtx[1] - vy);
              histos.fill(HIST("bkg/decayradiusResoZ"), secVtx[2] - vz);
              histos.fill(HIST("bkg/decayradiusReso"), rmc);
              histos.fill(HIST("bkg/decaydca0"), TMath::Sqrt(fitter.getChi2AtPCACandidate(0)));
              histos.fill(HIST("bkg/decaydca1"), TMath::Sqrt(fitter.getChi2AtPCACandidate(1)));
              histos.fill(HIST("bkg/dcaxy1"), dca1[0]);
              histos.fill(HIST("bkg/dcaz1"), dca1[1]);
              histos.fill(HIST("bkg/dcaxy2"), dca2[0]);
              histos.fill(HIST("bkg/dcaz2"), dca2[1]);
              histos.fill(HIST("bkg/dcaxy3"), dca3[0]);
              histos.fill(HIST("bkg/dcaz3"), dca3[1]);

              histos.fill(HIST("bkg/dcaxy1xdcaxy2"), dca1[0] * dca2[0]);
              histos.fill(HIST("bkg/dcaz1xdcaz2"), dca1[1] * dca2[1]);

              histos.fill(HIST("bkg/dcaxy3xdcaxy2"), dca3[0] * dca2[0]);
              histos.fill(HIST("bkg/dcaz3xdcaz2"), dca3[1] * dca2[1]);
            }

            // fitterCasc.getTrack(1).getPxPyPzGlo(pvecbach);
          } //end if cascade recoed
        }
      }
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{adaptAnalysisTask<Alice3CDeuteron>(cfgc)};
}
