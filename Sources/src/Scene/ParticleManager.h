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
  void STDCALL SetSerialMode(ESharedDataSerialMode eSerialMode) override
  {
    shareKeyBased.SetSerialMode(eSerialMode);
    shareSmokin.SetSerialMode(eSerialMode);
  }

  // setup sharing mode
  void STDCALL SetShareMode(ESharedDataSharingMode eShareMode) override
  {
    shareKeyBased.SetShareMode(eShareMode);
    shareSmokin.SetShareMode(eShareMode);
  }

  // remove all shared resource from this manager
  void STDCALL Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool STDCALL Init() override { return shareKeyBased.Init() && shareSmokin.Init(); }
  //
  IParticleSource * STDCALL GetKeyBasedSource(const char *pszName) override;
  IParticleSource * STDCALL GetSmokinParticleSource(const char *pszName) override;
  //
  void STDCALL SetQuality(float fQuality) override;
};

#endif