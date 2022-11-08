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
<<<<<<< HEAD
/// @since  2022-11-08
=======
>>>>>>> 86aa1860c0... Add first template of param container
/// \brief  Definitions of the containers for the general parameters

#ifndef O2_TOF_PARAMCONTAINER_H
#define O2_TOF_PARAMCONTAINER_H

#include "TNamed.h"
<<<<<<< HEAD
#include "TFile.h"
#include "Framework/Logger.h"
#include "map"
=======
>>>>>>> 86aa1860c0... Add first template of param container

namespace o2
{
namespace tof
{
<<<<<<< HEAD
using paramvar_t = float;

template <int nPar>
class Parameters
{
 public:
  /// Default constructor
  Parameters(std::array<std::string, nPar> parNames, std::string name) : mName{name}, mPar{}, mParNames{parNames} {};

  /// Default destructor
  ~Parameters() = default;
=======

template <int nPar>
class Parameters : public TNamed
{
 public:
  /// Default constructor
  Parameters(std::array<std::string, nPar> parNames, TString name = "DefaultParameters") : TNamed(name, name), mPar{}, mParNames{parNames} {};

  /// Default destructor
  ~Parameters() override = default;
>>>>>>> 86aa1860c0... Add first template of param container

  /// Setter for the parameter at position iparam
  /// \param iparam index in the array of the parameters
  /// \param value value of the parameter at position iparam
<<<<<<< HEAD
  void SetParameter(const unsigned int iparam, const paramvar_t value) { mPar[iparam] = value; }

  /// Setter for the parameter, using an array
  /// \param param array with parameters
  void SetParameters(const paramvar_t* params) { std::copy(params, params + mPar.size(), mPar.begin()); }

  /// Setter for the parameter, using a vector
  /// \param params vector with parameters
  void SetParameters(const std::array<paramvar_t, nPar> params)
=======
  void SetParameter(const unsigned int iparam, const pidvar_t value) { mPar[iparam] = value; }

  /// Setter for the parameter, using an array
  /// \param param array with parameters
  void SetParameters(const pidvar_t* params) { std::copy(params, params + mPar.size(), mPar.begin()); }

  /// Setter for the parameter, using a vector
  /// \param params vector with parameters
  void SetParameters(const std::array<pidvar_t, nPar> params)
>>>>>>> 86aa1860c0... Add first template of param container
  {
    for (int i = 0; i < nPar; i++) {
      mPar[i] = params[i];
    }
  }

  /// Setter for the parameter, using a parameter object
  /// \param params parameter object with parameters
  void SetParameters(const Parameters<nPar> params) { SetParameters(params.mPar); };

  /// Setter for the parameter, using a parameter pointer
  /// \param params pointer to parameter object with parameters
  void SetParameters(const Parameters<nPar>* params) { SetParameters(params->mPar); };

  /// Printer of the parameter values
<<<<<<< HEAD
  void Print(Option_t* option = "") const
  {
    LOG(info) << "Parameters '" << mName << "'";
=======
  void Print(Option_t* option = "") const override
  {
    LOG(info) << "Parameters '" << fName << "'";
>>>>>>> 86aa1860c0... Add first template of param container
    for (int i = 0; i < nPar; i++) {
      LOG(info) << "Parameter " << i << "/" << nPar - 1 << " is " << mPar[i];
    }
  }

  /// Adds the parameters to the metadata
  void AddToMetadata(std::map<std::string, std::string>& metadata) const
  {
    for (int i = 0; i < nPar; i++) {
      metadata[Form("p%i", i)] = Form("%f", mPar[i]);
    }
  }

  /// Loader from file
  /// \param FileName name of the input file
  /// \param ParamName name of the input object
  void LoadParamFromFile(const TString FileName, const TString ParamName)
  {
    TFile f(FileName, "READ");
    if (!f.Get(ParamName)) {
      LOG(fatal) << "Did not find parameters " << ParamName << " in file " << FileName;
    }
    LOG(info) << "Loading parameters " << ParamName << " from TFile " << FileName;
    Parameters<nPar>* p;
    f.GetObject(ParamName, p);
    if (!p) {
      LOG(fatal) << "Could not get parameters " << ParamName << " from file";
      f.ls();
    }
    f.Close();
    SetParameters(p);
    Print();
  }

  /// Getter for the parameters
  /// \return returns an array of parameters
<<<<<<< HEAD
  const paramvar_t* GetParameters() const { return mPar.to_array(); }

  /// Getter for the parameters
  /// \return returns an array of parameters
  const paramvar_t GetParameter(int i) const { return mPar[i]; }

  /// Getter for the parameters
  /// \return returns an array of parameters
  const std::string GetParameterName(int i) const { return mParNames[i]; }
=======
  const pidvar_t* GetParameters() const { return mPar.to_array(); }

  /// Getter for the parameters
  /// \return returns an array of parameters
  const pidvar_t GetParameter(int i) const { return mPar[i]; }
>>>>>>> 86aa1860c0... Add first template of param container

  /// Getter for the size of the parameter
  /// \return returns the size of the parameter array
  static int size() { return nPar; }

  /// Getter of the parameter at position i
  /// \param i index of the parameter to get
  /// \return returns the parameter value at position i
<<<<<<< HEAD
  paramvar_t operator[](const unsigned int i) const { return mPar[i]; }

 private:
  /// Array of the parameter
  std::array<paramvar_t, nPar> mPar;
  const std::array<std::string, nPar> mParNames;
  std::string mName;
=======
  pidvar_t operator[](const unsigned int i) const { return mPar[i]; }

 private:
  /// Array of the parameter
  std::array<pidvar_t, nPar> mPar;
  const std::array<std::string, nPar> mParNames;

  ClassDefOverride(Parameters, 1); // Container for parameter of parametrizations
>>>>>>> 86aa1860c0... Add first template of param container
};

/// \brief Class container to hold different parameters meant to be stored on the CCDB
class ParameterCollection : public TNamed
{
 public:
  /// Default constructor
  ParameterCollection(TString name = "DefaultParameters") : TNamed(name, name), mParameters{} {};

  /// Default destructor
  ~ParameterCollection() override = default;

  /// @brief Function to load the parameters from the container into the vector based on the asked key, e.g. pass or version
  ///        Parameters that are not found in storage are kept unchanged
  /// @tparam nPar dimension of the parameter to configure
  /// @param p parameter list to configure from the stored information
  /// @param key key to look for in the stored information e.g. pass
  /// @return true if found and configured false if not fully configured
  template <int nPar>
<<<<<<< HEAD
  bool LoadParameters(Parameters<nPar>& p, const std::string& key) const;
=======
  bool LoadParameters(Parameters<nPar>& p, const std::string& key) const
  {
    if (mParameters.find(key) == mParameters.end()) { // Can't find the required key
      return false;
    }

    const auto& toGet = mParameters.at(key);
    for (int i = 0; i < p.size(); i++) {
      const auto& name = MakeParameterKey(i);
      if (toGet.find(name) == toGet.end()) {
        LOG(debug) << "Did not find parameter " << name << " in collection, keeping preexisting";
        continue;
      }
      p.SetParameter(i, toGet.at(name));
    }
    return true;
  }
>>>>>>> 86aa1860c0... Add first template of param container

  /// @brief Function to push the parameters from the sub container into the collection and store it under a given key
  /// @tparam nPar dimension of the parameter to store
  /// @param p parameter list to store
  /// @param key store key
  /// @return true if modified and false if a new key is added
  template <int nPar>
  bool StoreParameters(const Parameters<nPar>& p, const std::string& key)
  {
    const bool alreadyPresent = (mParameters.find(key) == mParameters.end());
    if (alreadyPresent) {
      LOG(debug) << "Changing parametrization corresponding to key " << key << " from size " << mParameters[key].size() < " to " << p.GetName() << " of size " << p.size();
    } else {
<<<<<<< HEAD
      mParameters[key] = std::unordered_map<std::string, paramvar_t>{};
=======
      mParameters[key] = std::unordered_map<std::string, pidvar_t>{};
>>>>>>> 86aa1860c0... Add first template of param container
      LOG(debug) << "Adding new parametrization corresponding to key " << key << ": " << p.GetName() << " of size " << p.size();
    }
    for (int i = 0; i < p.size(); i++) {
      mParameters[key][p.GetParameterName(i)] = p[i];
    }
    return alreadyPresent;
  }

<<<<<<< HEAD
 private:
  /// Array of the parameter
  std::unordered_map<std::string, std::unordered_map<std::string, paramvar_t>> mParameters;
=======
  /// @brief Function to push the parameters from the sub container into the collection and store it under a given key
  /// @tparam nPar dimension of the parameter to store
  /// @param p parameter list to store
  /// @param key store key
  /// @return true if modified and false if a new key is added
  bool StoreParameters(const pidvar_t p, const std::string& key, const std::string& parname)
  {
    const bool alreadyPresent = (mParameters.find(key) == mParameters.end());
    if (alreadyPresent) {
      LOG(debug) << "Changing parametrization corresponding to key " << key << " from size " << mParameters[key].size() < " to " << p.GetName() << " of size " << p.size();
    } else {
      LOG(debug) << "Adding new parametrization corresponding to key " << key << ": " << p.GetName() << " of size " << p.size();
    }
    mParameters[key] = std::unordered_map<std::string, pidvar_t>{};
    for (int i = 0; i < p.size(); i++) {
      mParameters[key][MakeParameterKey(i)] = p[i];
    }
    return alreadyPresent;
  }

  /// @brief Function to create the keys to be used for storing and retrieving the parameters
  /// @param i index of the parameter
  /// @return The key used to store/retrieve the parameter as a std::string
  static std::string MakeParameterKey(int i, std::string tag) { return Form("p_%i", i); }

 private:
  /// Array of the parameter
  std::unordered_map<std::string, std::unordered_map<std::string, pidvar_t>> mParameters;
>>>>>>> 86aa1860c0... Add first template of param container

  ClassDefOverride(ParameterCollection, 1); // Container for containers of parameter of parametrizations. To be used as a manager, in help of CCDB
};

} // namespace tof
} // namespace o2

#endif // O2_TOF_PARAMCONTAINER_H