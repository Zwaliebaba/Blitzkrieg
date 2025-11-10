#include "StdAfx.h"

#include "ParticleManager.h"
#include "KeyBasedParticleSource.h"
#include "SmokinParticleSource.h"

IParticleSource *CParticleDataManager::GetKeyBasedSource(const char *pszName)
{
  SParticleSourceData *pData = shareKeyBased.Get(pszName);
  if (pData)
  {
    // CRAP{ to work with old particles
    if (pData->szTextureName.empty() || (pData->nTextureDX == 0) || (pData->nTextureDY == 0))
    {
      shareKeyBased.Remove(pszName);
      pData = shareKeyBased.Get(pszName);
    }
    // CRAP}
    auto pEffect = new CKeyBasedParticleSource();
    pEffect->Init(pData);
    return pEffect;
  }
  return nullptr;
}

IParticleSource *CParticleDataManager::GetSmokinParticleSource(const char *pszName)
{
  SSmokinParticleSourceData *pData = shareSmokin.Get(pszName);
  auto pEffect = new CSmokinParticleSource();
  pEffect->Init(pData);
  return pEffect;
}

// remove all shared resource from this manager
void CParticleDataManager::Clear(const EClearMode eMode, const int nUsage, const int nAmount)
{
  if (eMode == CLEAR_ALL)
  {
    shareKeyBased.Clear();
    shareSmokin.Clear();
  }
  else
  {
    shareKeyBased.ClearUnreferencedResources();
    shareSmokin.ClearUnreferencedResources();
  }
}

void CParticleDataManager::SetQuality(const float fQuality)
{
  // simple particle sources
  for (auto it = shareKeyBased.begin(); it != shareKeyBased.end(); ++it) it->second->fDensityCoeff = fQuality;
  // complex source
  for (auto it = shareSmokin.begin(); it != shareSmokin.end(); ++it)
  {
    it->second->fDensityCoeff = fQuality;
    if (IParticleSource *pSource = GetKeyBasedSource((it->second->szParticleEffectName + ".xml").c_str())) it->second->nUpdateStep = pSource->GetOptimalUpdateTime();
  }
}

int CParticleDataManager::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  shareKeyBased.Serialize(&ss);
  shareSmokin.Serialize(&ss);
  return 0;
}