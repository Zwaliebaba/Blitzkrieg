#include "StdAfx.h"

#include "SoundManager.h"

#include "SampleSounds.h"
#include "../Formats/fmtTerrain.h"

// ************************************************************************************************************************ //
// **
// ** sound manager
// **
// **
// **
// ************************************************************************************************************************ //

CSoundManager::~CSoundManager()
{
  // here you must first clear all the balls, and then only you can release the sound system,
  // because 
  share2.Clear();
  share3.Clear();
  pSFX = nullptr;
}

bool CSoundManager::Init()
{
  share2.Init();
  share3.Init();
  //
  share3.SetMinDistance(GetGlobalVar("Sound.MinDistance", 1) * fWorldCellSize / 2.0f);
  // lets lock sound system :)
  pSFX = GetSingleton<ISFX>();
  //
  return true;
}

int CSoundManager::operator&(IStructureSaver &ss)
{
  share2.Serialize(&ss);
  share3.Serialize(&ss);
  // stop all sounds on reading
  if (ss.IsReading()) { pSFX = GetSingleton<ISFX>(); }
  //
  return 0;
}

ISound *CSoundManager::GetSound3D(const char *pszName)
{
  CSoundSample *pSample = GetSample3D(pszName);
  if (pSample == nullptr) return nullptr;
  CSound3D *pSound = CreateObject<CSound3D>(SFX_SOUND_3D);
  if (pSound == nullptr) return nullptr;
  //
  pSound->SetSample(pSample);
  //
  return pSound;
}

ISound *CSoundManager::GetSound2D(const char *pszName)
{
  CSoundSample *pSample = GetSample2D(pszName);
  if (pSample == nullptr) return nullptr;
  CSound2D *pSound = CreateObject<CSound2D>(SFX_SOUND_2D);
  if (pSound == nullptr) return nullptr;
  //
  pSound->SetSample(pSample);
  //
  return pSound;
}

const char *CSoundManager::GetSoundName(ISound *pSound)
{
  if (auto pSnd = dynamic_cast<CSound2D *>(pSound))
  {
    const std::string *pName = share2.GetKey(pSnd->GetSample());
    return pName != nullptr ? pName->c_str() : "";
  }
  if (auto pSnd = dynamic_cast<CSound3D *>(pSound))
  {
    const std::string *pName = share3.GetKey(pSnd->GetSample());
    return pName != nullptr ? pName->c_str() : "";
  }
  return "";
}

// remove all shared resource from this manager
void CSoundManager::Clear(const EClearMode eMode, const int nUsage, const int nAmount)
{
  if (eMode == CLEAR_ALL)
  {
    share2.Clear();
    share3.Clear();
  }
  else
  {
    share2.ClearUnreferencedResources();
    share3.ClearUnreferencedResources();
  }
}