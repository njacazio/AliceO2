#if !defined(__CLING__) || defined(__ROOTCLING__)
// #define ENABLE_UPGRADES
#include "DetectorsBase/GeometryManager.h"
#include <IOTOFBase/IOTOFBaseParam.h>
#include "DetectorsCommonDataFormats/AlignParam.h"
#include "DetectorsCommonDataFormats/DetectorNameConf.h"
#include "DetectorsCommonDataFormats/DetID.h"
#include "IOTOFBase/GeometryTGeo.h"
#include <TCanvas.h>
#include <TFile.h>
#include <TGeoBBox.h>
#include <TGeoVolume.h>
#include <TH2F.h>
#include <TObjArray.h>
#include <TROOT.h>
#include <TRandom.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <fmt/format.h>
#include <string>
#include <vector>
#endif

// Inspired from Detectors/ITSMFT/ITS/macros/test/ITSMisaligner.C

using AlgPar = std::array<double, 6>;
using PreviewPoint = std::array<double, 5>; // lay, sta, mod, chip, deltaY

AlgPar generateMisalignment(double x, double y, double z, double psi, double theta, double phi);
double getVolumeHalfLengthZ(const char* volumeName);
int countNodesMatching(const char* volumeName, const char* pattern);
int getNumberOfStaves(int lay);
double getChipLocalZ(int lay, int mod, int chip);
double getChipPhi(int lay, int sta);
double computeSagitta(int lay, int sta, int mod, int chip, double sagA);
void drawMisalignmentPreview(const std::vector<PreviewPoint>& previewPoints,
                             double sagA);

void IOTOFMisaligner(long tmin = 0, long tmax = -1,
                     double xEnv = 0., double yEnv = 0., double zEnv = 0., double psiEnv = 0., double thetaEnv = 0., double phiEnv = 0.,
                     double xHBa = 0., double yHBa = 0., double zHBa = 0., double psiHBa = 0., double thetaHBa = 0., double phiHBa = 0.,
                     double xSta = 0., double ySta = 0., double zSta = 0., double psiSta = 0., double thetaSta = 0., double phiSta = 0.,
                     double xHSt = 0., double yHSt = 0., double zHSt = 0., double psiHSt = 0., double thetaHSt = 0., double phiHSt = 0.,
                     double xMod = 0., double yMod = 0., double zMod = 0., double psiMod = 0., double thetaMod = 0., double phiMod = 0.,
                     double xChp = 0., double yChp = 0., double zChp = 0., double psiChp = 0., double thetaChp = 0., double phiChp = 0.,
                     double sagA = 1000.,
                     const std::string& objectPath = "",
                     const std::string& fileName = "IOTOFAlignment.root")
{
  std::vector<o2::detectors::AlignParam> params;
  std::vector<PreviewPoint> previewPoints;
  o2::base::GeometryManager::loadGeometry("", false);
  o2::conf::ConfigurableParam::setValue<bool>("IOTOFBase", "segmentedInnerTOF", true);
  o2::conf::ConfigurableParam::setValue<bool>("IOTOFBase", "segmentedOuterTOF", true);
  auto geom = o2::iotof::GeometryTGeo::Instance();
  if (!geom || geom->getSize() <= 0) {
    LOG(fatal) << "Failed to load IOTOF geometry: size is " << (geom ? geom->getSize() : -1) << ". Is the geometry file correct and IOTOF segmented?";
    return;
  }
  std::string symname;
  AlgPar pars;
  bool glo = true;
  o2::detectors::DetID detIOTOF("TF3");

  (void)tmin;
  (void)tmax;
  (void)objectPath;

  for (int ich = 0; ich < geom->getSize(); ich++) {
    LOGP(info, "Processing IOTOF chip index %d/%d", ich, geom->getSize());
    int lay = -1;
    int sta = -1;
    int mod = -1;
    int chip = -1;
    geom->getIOTOFChipId(ich, lay, sta, mod, chip);
    const auto* chipSymbolicName = o2::base::GeometryManager::getSymbolicName(detIOTOF, ich);
    if (chipSymbolicName == nullptr) {
      LOG(error) << "Failed to find IOTOF symbolic name for chip index " << ich;
      continue;
    }

    symname = chipSymbolicName;
    pars = generateMisalignment(xChp, yChp, zChp, psiChp, thetaChp, phiChp);
    pars[1] += computeSagitta(lay, sta, mod, chip, sagA);
    int chID = o2::base::GeometryManager::getSensID(detIOTOF, ich);
    params.emplace_back(symname.c_str(), chID, pars[0], pars[1], pars[2], pars[3], pars[4], pars[5], glo);
    previewPoints.push_back({static_cast<double>(lay), static_cast<double>(sta), static_cast<double>(mod), static_cast<double>(chip), pars[1]});
  }

  drawMisalignmentPreview(previewPoints, sagA);

  if (!fileName.empty()) {
    LOGP(info, "Storing IOTOF alignment in local file {}", fileName);
    TFile algFile(fileName.c_str(), "recreate");
    algFile.WriteObjectAny(&params, "std::vector<o2::detectors::AlignParam>", "alignment");
    algFile.Close();
  }
}

AlgPar generateMisalignment(double x, double y, double z, double psi, double theta, double phi)
{
  AlgPar pars;
  pars[0] = gRandom->Gaus(0, x);
  pars[1] = gRandom->Gaus(0, y);
  pars[2] = gRandom->Gaus(0, z);
  pars[3] = gRandom->Gaus(0, psi);
  pars[4] = gRandom->Gaus(0, theta);
  pars[5] = gRandom->Gaus(0, phi);
  return std::move(pars);
}

double getVolumeHalfLengthZ(const char* volumeName)
{
  auto* volume = gGeoManager->GetVolume(volumeName);
  if (volume == nullptr) {
    LOG(fatal) << "Can't find volume " << volumeName;
    return 0.;
  }
  auto* box = dynamic_cast<TGeoBBox*>(volume->GetShape());
  if (box == nullptr) {
    LOG(fatal) << "Volume " << volumeName << " is not a box";
    return 0.;
  }
  return box->GetDZ();
}

int countNodesMatching(const char* volumeName, const char* pattern)
{
  auto* volume = gGeoManager->GetVolume(volumeName);
  if (volume == nullptr) {
    LOG(fatal) << "Can't find volume " << volumeName;
    return 0;
  }

  TObjArray* nodes = volume->GetNodes();
  int count = 0;
  for (int i = 0; i < nodes->GetEntriesFast(); ++i) {
    if (strstr(nodes->At(i)->GetName(), pattern) != nullptr) {
      ++count;
    }
  }
  return count;
}

int getNumberOfStaves(int lay)
{
  const char* layerName = lay == 0 ? o2::iotof::GeometryTGeo::getITOFLayerPattern() : o2::iotof::GeometryTGeo::getOTOFLayerPattern();
  const char* staveName = lay == 0 ? o2::iotof::GeometryTGeo::getITOFStavePattern() : o2::iotof::GeometryTGeo::getOTOFStavePattern();
  return countNodesMatching(layerName, staveName);
}

double getChipLocalZ(int lay, int mod, int chip)
{
  const char* staveName = lay == 0 ? o2::iotof::GeometryTGeo::getITOFStavePattern() : o2::iotof::GeometryTGeo::getOTOFStavePattern();
  const char* moduleName = lay == 0 ? o2::iotof::GeometryTGeo::getITOFModulePattern() : o2::iotof::GeometryTGeo::getOTOFModulePattern();
  const char* chipName = lay == 0 ? o2::iotof::GeometryTGeo::getITOFChipPattern() : o2::iotof::GeometryTGeo::getOTOFChipPattern();

  constexpr int kChipsPerModuleZ = 2;

  const double staveSizeZ = 2. * getVolumeHalfLengthZ(staveName);
  const double moduleSizeZ = 2. * getVolumeHalfLengthZ(moduleName);
  const double chipSizeZ = 2. * getVolumeHalfLengthZ(chipName);
  const int chipZIndex = chip % kChipsPerModuleZ;
  return ((mod + 0.5) * moduleSizeZ - 0.5 * staveSizeZ) + ((chipZIndex + 0.5) * chipSizeZ - 0.5 * moduleSizeZ);
}

double getChipPhi(int lay, int sta)
{
  constexpr double kTwoPi = 2.0 * 3.141592653589793238462643383279502884;
  const int nStaves = getNumberOfStaves(lay);
  if (nStaves <= 0) {
    return 0.;
  }
  return kTwoPi * static_cast<double>(sta) / static_cast<double>(nStaves);
}

double computeSagitta(int lay, int sta, int mod, int chip, double sagA)
{
  if (sagA <= 0.) {
    return 0.;
  }

  const int nStaves = getNumberOfStaves(lay);
  if (nStaves <= 0) {
    return 0.;
  }

  const double z = getChipLocalZ(lay, mod, chip);
  const double phi = getChipPhi(lay, sta);

  // The local y direction is normal to the stave plane; sign follows the chosen phi convention.
  return -sagA * (std::cosh(z / sagA) - 1.0) * std::sin(phi);
}

void drawMisalignmentPreview(const std::vector<PreviewPoint>& previewPoints,
                             double sagA)
{
  if (previewPoints.empty()) {
    Printf("No preview points to display for IOTOF misalignment preview");
    return;
  }

  double zMin[2] = {1.e9, 1.e9};
  double zMax[2] = {-1.e9, -1.e9};

  for (const auto& p : previewPoints) {
    const int lay = static_cast<int>(p[0]);
    const int mod = static_cast<int>(p[2]);
    const int chip = static_cast<int>(p[3]);
    if (lay < 0 || lay > 1) {
      continue;
    }
    const double z = getChipLocalZ(lay, mod, chip);
    zMin[lay] = std::min(zMin[lay], z);
    zMax[lay] = std::max(zMax[lay], z);
  }

  static int sCanvasCounter = 0;
  const auto canvasName = fmt::format("cIOTOFMisalignPreview_{}", sCanvasCounter++);
  auto* canvas = new TCanvas(canvasName.c_str(), "IOTOF misalignment preview", 1300, 600);
  canvas->Divide(2, 1);

  const char* layerLabel[2] = {"ITOF", "OTOF"};
  TH2F* hMap[2] = {nullptr, nullptr};

  for (int lay = 0; lay < 2; ++lay) {
    const std::string histName = fmt::format("hIOTOFdYMap_l{}_{}", lay, sCanvasCounter);
    const std::string histTitle = Form("%s local #Deltay map (sagA=%f);z_{local};#varphi [rad]", layerLabel[lay], sagA);
    const double zLow = zMin[lay] < zMax[lay] ? zMin[lay] : -1.;
    const double zHigh = zMin[lay] < zMax[lay] ? zMax[lay] : 1.;
    hMap[lay] = new TH2F(histName.c_str(), histTitle.c_str(), 120, zLow, zHigh, 120, -3.1416, 3.1416);
  }

  for (const auto& p : previewPoints) {
    const int lay = static_cast<int>(p[0]);
    const int sta = static_cast<int>(p[1]);
    const int mod = static_cast<int>(p[2]);
    const int chip = static_cast<int>(p[3]);
    const double deltaY = p[4];
    if (lay < 0 || lay > 1 || hMap[lay] == nullptr) {
      continue;
    }
    const double z = getChipLocalZ(lay, mod, chip);
    const double phi = getChipPhi(lay, sta);
    hMap[lay]->Fill(z, phi, deltaY);
  }

  for (int lay = 0; lay < 2; ++lay) {
    canvas->cd(lay + 1);
    if (hMap[lay] != nullptr) {
      hMap[lay]->SetStats(false);
      hMap[lay]->Draw("COLZ");
    }
  }

  canvas->Update();
  LOG(info) << "Displayed IOTOF misalignment preview canvas before writing alignment file";
}
