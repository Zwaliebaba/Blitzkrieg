#ifndef __INTERFACESWITCHMODETO_H__
#define __INTERFACESWITCHMODETO_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "../Misc/FileUtils.h"

// if mode exitst then ask for switch. 
// warning message and does nothing

class CInterfaceSwitchModeTo : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceSwitchModeTo);

public:
  using CModName = std::pair<std::string, std::string>;
  using CModNames = std::vector<CModName>;

  struct SEnumDirs
  {
    CModNames *pModDirs;
    void operator()(const class NFile::CFileIterator &fileIt) const;
  };

private:
  // input
  NInput::CCommandRegistrator commandMsgs;
  bool bModExists;// mode exists and switch is possible
  std::string szDirName;// desired mod name

  int nCommandOnOk;
  std::string szCommandParams;
  bool bSilentSwitch;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceSwitchModeTo() override;
  CInterfaceSwitchModeTo() : CInterfaceInterMission("InterMission") {}

public:
  bool STDCALL Init() override;
  bool Create(const std::string &szModName,
              const std::string &szModVer,
              int nCommandID,
              const std::string &szComandParams,
              bool bSilentSwitch);
  void OnOk();

};

class CICSwitchModeTo : public CInterfaceCommandBase<CInterfaceSwitchModeTo, MISSION_INTERFACE_SWITCH_MODE_TO>
{
  OBJECT_NORMAL_METHODS(CICSwitchModeTo);
  std::string szNewModName;// new mode to switch to
  std::string szNewModVersion;// new mode to switch to

  int nCommandOnOk;
  std::string szCommandParams;
  bool bSilentSwitch;

  void PreCreate(IMainLoop *pML) override {}

  void PostCreate(IMainLoop *pML, CInterfaceSwitchModeTo *pI) override
  {
    const bool bOk = pI->Create(szNewModName, szNewModVersion, nCommandOnOk, szCommandParams, bSilentSwitch);
    pML->PushInterface(pI);
    if (bOk) pI->OnOk();
  }

  //
  CICSwitchModeTo() {}

public:
  void STDCALL Configure(const char *pszConfig) override
  {
    if (pszConfig)
    {
      std::vector<std::string> szStrings;
      NStr::SplitString(pszConfig, szStrings, ';');
      NI_ASSERT_T(szStrings.size() == 5, "Invalid number of parameters for encyclopedia interface");
      szNewModName = szStrings[0];
      szNewModVersion = szStrings[1];
      nCommandOnOk = NStr::ToInt(szStrings[2]);
      szCommandParams = szStrings[3];
      bSilentSwitch = NStr::ToInt(szStrings[4]);
    }
  }
};

#endif // __INTERFACESWITCHMODETO_H__