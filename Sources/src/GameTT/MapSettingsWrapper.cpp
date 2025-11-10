#include "StdAfx.h"

#include "MapSettingsWrapper.h"
#include "OptionEntryWrapper.h"
#include "../RandomMapGen/MapInfo_Types.h"
#include "UIConsts.h"


bool CMapSettingsWrapper::CFakeOptionSystem::Set(const std::string &szVarName, const variant_t &var)
{
  if (bServer) GetSingleton<IOptionSystem>()->Set(szVarName, var);
  // save this value to settings;

  if (szVarName == "Multiplayer.Timelimit") { settings.nTimeLimit = static_cast<long>(var); }
  else if (szVarName == "Multiplayer.TimeToCapture") { settings.nTimeToCapture = static_cast<long>(var); }
  else if (szVarName == "Multiplayer.KillScoreLimit") { settings.nKillScoreLimit = static_cast<long>(var); }
  else if (szVarName == "Multiplayer.GameSpeed") { settings.szGameSpeed = bstr_t(var); }
  else if (szVarName == "Multiplayer.FlagScoreLimit") { settings.nFlagScoreLimit = static_cast<long>(var); }
  return true;
}

bool CMapSettingsWrapper::CFakeOptionSystem::Get(const std::string &szVarName, variant_t *pVar) const
{
  if (bServer) GetSingleton<IOptionSystem>()->Get(szVarName, pVar);

  // get from options.
  if (szVarName == "Multiplayer.Timelimit") { *pVar = static_cast<long>(settings.nTimeLimit); }
  else if (szVarName == "Multiplayer.TimeToCapture") { *pVar = static_cast<long>(settings.nTimeToCapture); }
  else if (szVarName == "Multiplayer.KillScoreLimit") { *pVar = static_cast<long>(settings.nKillScoreLimit); }
  else if (szVarName == "Multiplayer.GameSpeed") { *pVar = settings.szGameSpeed.c_str(); }
  else if (szVarName == "Multiplayer.FlagScoreLimit") { *pVar = static_cast<long>(settings.nFlagScoreLimit); }
  return true;
}

CMapSettingsWrapper::CMapSettingsWrapper(const bool _bCanChange, const int _nFlag)
  : bCanChange(_bCanChange), nFlag(_nFlag)
{
  pOptionSystem = new CFakeOptionSystem(bCanChange);

  if (bCanChange)
  {
    IOptionSystem *pSystem = GetSingleton<IOptionSystem>();

    variant_t var;

    pSystem->Get("Multiplayer.Timelimit", &var);
    pOptionSystem->settings.nTimeLimit = static_cast<long>(var);

    pSystem->Get("Multiplayer.TimeToCapture", &var);
    pOptionSystem->settings.nTimeToCapture = static_cast<long>(var);

    pSystem->Get("Multiplayer.KillScoreLimit", &var);
    pOptionSystem->settings.nKillScoreLimit = static_cast<long>(var);

    pSystem->Get("Multiplayer.GameSpeed", &var);
    pOptionSystem->settings.szGameSpeed = bstr_t(var);

    pSystem->Get("Multiplayer.FlagScoreLimit", &var);
    pOptionSystem->settings.nFlagScoreLimit = static_cast<long>(var);
  }

}

void CMapSettingsWrapper::Init(const SMultiplayerGameSettings &_settings) { pOptionSystem->settings = _settings; }

const SMultiplayerGameSettings &CMapSettingsWrapper::GetSettingsWOApply() const { return pOptionSystem->settings; }

const SMultiplayerGameSettings &CMapSettingsWrapper::GetSettings()
{
  NI_ASSERT_T(pOptionSystem != 0, "not inititalized screen");
  pListWrapper->Apply();
  return pOptionSystem->settings;
}


void CMapSettingsWrapper::Init(IUIListControl *_pList, IUIStatic *_pGameType)
{
  pList = _pList;
  pGameType = _pGameType;
  // Create options container and fill it with options.
  pListWrapper = new COptionsListWrapper(nFlag, pList, 100, pOptionSystem);
  if (!bCanChange) pListWrapper->DisableChange();
}

void CMapSettingsWrapper::SetGameType(const int /* SQuickLoadMapInfo::EMultiplayerMapType */ nGameType) { pGameType->SetWindowText(0, CUIConsts::GetMapTypeString(nGameType)); }

void CMapSettingsWrapper::Apply() { pListWrapper->Apply(); }

bool CMapSettingsWrapper::ProcessMessage(const SGameMessage &msg)
{
  // process messages about changing options.
  if (pListWrapper && pListWrapper->ProcessMessage(msg)) return true;

  return false;
}