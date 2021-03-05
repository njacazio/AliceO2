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
/// \file   testCCDBFileFetch.cxx
/// \brief  Test BasicCCDBManager fetching of files
/// \author Nicolo' Jacazio
///

#define BOOST_TEST_MODULE CCDB
#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include "CCDB/CcdbApi.h"
#include "CCDB/BasicCCDBManager.h"
#include "Framework/Logger.h"
#include <boost/test/unit_test.hpp>

using namespace o2::ccdb;

BOOST_AUTO_TEST_CASE(TestCCDBFileFetch)
{
  // BOOST_CHECK_MESSAGE(api.isHostReachable(),
  //                     "Host not reachable!");
  // test reading
  std::string path = "Test/nj/my_ccdb_file.root";
  auto& cdb = o2::ccdb::BasicCCDBManager::instance();
  cdb.setURL(uri);
  // cdb.setTimestamp((start + stop) / 2);
  cdb.setCaching(true);

  auto* file = cdb.get<TFile*>(path); // will be loaded from scratch and fill the cache
  file->ls();
  BOOST_CHECK(false);
}
