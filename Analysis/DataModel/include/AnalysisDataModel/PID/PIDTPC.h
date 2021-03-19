// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

///
/// \file   PIDTPC.h
/// \author Nicolo' Jacazio
/// \since  2020-07-24
/// \brief  Implementation of the TPC detector response for PID
///

#ifndef O2_FRAMEWORK_PIDTPC_H_
#define O2_FRAMEWORK_PIDTPC_H_

// ROOT includes
#include "Rtypes.h"
#include "TMath.h"

// O2 includes
#include "Framework/Logger.h"
#include "ReconstructionDataFormats/PID.h"
#include "AnalysisDataModel/PID/DetectorResponse.h"
#include "AnalysisDataModel/PID/TPCReso.h"
#include "AnalysisDataModel/PID/BetheBloch.h"

namespace o2::pid::tpc
{

/// \brief Class to handle the the TPC detector response
template <typename Coll, typename Trck, o2::track::PID::ID id>
class ELoss
{
 public:
  ELoss() = default;
  ~ELoss() = default;

  /// Gets the expected signal of the measurement
  float GetExpectedSignal(const DetectorResponse& response, const Coll& col, const Trck& trk) const;
  float GetExpectedSignal2(const Coll& col, const Trck& trk, const Parameters& parameters) const { return BetheBlochParamTrack<id>(trk, parameters); }

  /// Gets the expected resolution of the measurement
  float GetExpectedSigma(const DetectorResponse& response, const Coll& col, const Trck& trk) const;
  float GetExpectedSigma2(const Coll& col, const Trck& trk, const Parameters& parameters) const { return TPCResoParamTrack<id>(trk, parameters); }

  /// Gets the number of sigmas with respect the expected value
  float GetSeparation(const DetectorResponse& response, const Coll& col, const Trck& trk) const { return (trk.tpcSignal() - GetExpectedSignal(response, col, trk)) / GetExpectedSigma(response, col, trk); }
  float GetSeparation2(const Coll& col, const Trck& trk, const Parameters& expparameters, const Parameters& resoparameters) const { return (trk.tpcSignal() - GetExpectedSignal2(col, trk, expparameters)) / GetExpectedSigma2(col, trk, resoparameters); }
};

template <typename Coll, typename Trck, o2::track::PID::ID id>
float ELoss<Coll, Trck, id>::GetExpectedSignal(const DetectorResponse& response, const Coll& col, const Trck& trk) const
{
  const float x[2] = {trk.tpcInnerParam() / o2::track::PID::getMass(id), (float)o2::track::PID::getCharge(id)};
  return response(DetectorResponse::kSignal, x);
}

template <typename Coll, typename Trck, o2::track::PID::ID id>
float ELoss<Coll, Trck, id>::GetExpectedSigma(const DetectorResponse& response, const Coll& col, const Trck& trk) const
{
  const float x[2] = {trk.tpcSignal(), (float)trk.tpcNClsFound()};
  return response(DetectorResponse::kSigma, x);
}

} // namespace o2::pid::tpc

#endif // O2_FRAMEWORK_PIDTPC_H_
