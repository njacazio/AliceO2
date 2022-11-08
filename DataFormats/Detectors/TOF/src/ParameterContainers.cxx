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

/// \file   ParameterContainers.h
/// \author Nicolò Jacazio nicolo.jacazio@cern.ch
/// @since  2022-11-08
/// \brief  Implementation of the containers for the general parameters

#include "DataFormatsTOF/ParameterContainers.h"

// ClassImp(o2::tof::Parameters);
using namespace o2::tof;

template <int nPar>
bool ParameterCollection::LoadParameters(Parameters<nPar>& p, const std::string& key) const
{
  if (mParameters.find(key) == mParameters.end()) { // Can't find the required key
    return false;
  }

  const auto& toGet = mParameters.at(key);
  for (int i = 0; i < p.size(); i++) {
    const auto& name = p.GetParameterName(i);
    if (toGet.find(name) == toGet.end()) {
      LOG(debug) << "Did not find parameter " << name << " in collection, keeping preexisting";
      continue;
    }
    p.SetParameter(i, toGet.at(name));
  }
  return true;
}