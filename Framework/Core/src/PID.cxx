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

///
/// \file PID.cxx
/// \author Nicolò Jacazio nicolo.jacazio@cern.ch
/// \since 2024-09-11
/// \brief TOF PID utilities to work with the information stored in the AO2D
///

#include "Framework/PID.h"
#include <cmath>
#include "CommonConstants/PhysicsConstants.h"
// #include "ReconstructionDataFormats/PID.h"

using namespace o2::framework::pid;

float tof::MassToExpTime(float tofExpMom, float length, float massSquared)
{
  if (tofExpMom <= 0.f) {
    return -999.f;
  }
  return length * std::sqrt((massSquared) + (tofExpMom * tofExpMom)) / (o2::constants::physics::LightSpeedCm2PS * tofExpMom);
}

float TrackTimeToTOFSignal(float tracktime, float exptime)
{
  return tracktime * 1000.f + exptime;
}