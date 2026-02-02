#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#pragma once

#include "../Misc/BasicShare.h"
#include "ParticleSourceData.h"
#include "SmokinParticleSourceData.h"

BASIC_SHARE_DECLARE(CParticleDataShare, std::string, SParticleSourceData, PFX_KEYDATA, 107, "");

BASIC_SHARE_DECLARE(CSmokinParticleDataShare, std::string, SSmokinParticleSourceData, PFX_COMPLEX_KEYDATA, 108, "");

class CParticleDataManager : public IParticleManager
{
  OBJECT_COMPLETE_METHODS(CParticleDataManager);
  DECLARE_SERIALIZE;
  //
  CParticleDataShare shareKeyBased;
  CSmokinParticleDataShare shareSmokin;

public:
  void SetSerialMode(ESharedDataSerialMode eSerialMode) override
  {
    shareKeyBased.SetSerialMode(eSerialMode);
    shareSmokin.SetSerialMode(eSerialMode);
  }

  // setup sharing mode
  void SetShareMode(ESharedDataSharingMode eShareMode) override
  {
    shareKeyBased.SetShareMode(eShareMode);
    shareSmokin.SetShareMode(eShareMode);
  }

  // remove all shared resource from this manager
  void Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool Init() override { return shareKeyBased.Init() && shareSmokin.Init(); }
  //
  IParticleSource * GetKeyBasedSource(const char *pszName) override;
  IParticleSource * GetSmokinParticleSource(const char *pszName) override;
  //
  void SetQuality(float fQuality) override;
};

#endif