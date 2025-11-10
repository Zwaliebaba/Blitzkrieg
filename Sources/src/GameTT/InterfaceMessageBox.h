#ifndef __INTERFACEMESSAGEBOX_H__
#define __INTERFACEMESSAGEBOX_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceMessageBox : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceMessageBox);
  DECLARE_SERIALIZE;

  // input
  NInput::CCommandRegistrator commandMsgs;

  std::string szGlobalVarOnOk;
  bool bDoubleButton;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceMessageBox() override;
  CInterfaceMessageBox();

public:
  bool STDCALL Init() override;
  void Create(const std::string &szCaptionKey, const std::string &szMessageKey, bool _bDoubleButton, const std::string &szGlobalVarOnOk);
};

class CICMessageBox : public CInterfaceCommandBase<CInterfaceMessageBox, MISSION_INTERFACE_MESSAGE_BOX>
{
  OBJECT_NORMAL_METHODS(CICMessageBox);

  std::string szCaptionKey;
  std::string szMessageKey;
  bool bDoubleButton;
  std::string szGlobalVarOnOk;

  void PreCreate(IMainLoop *pML) override {}

  void PostCreate(IMainLoop *pML, CInterfaceMessageBox *pIMM) override
  {
    pIMM->Create(szCaptionKey, szMessageKey, bDoubleButton, szGlobalVarOnOk);
    pML->PushInterface(pIMM);
  }

  //
public:
  void STDCALL Configure(const char *pszConfig) override
  {
    NI_ASSERT_T(pszConfig != 0, "MISSION_INTERFACE_MESSAGE_BOX - must exists configuration");
    if (pszConfig)
    {
      std::vector<std::string> strings;
      NStr::SplitString(pszConfig, strings, ';');
      NI_ASSERT_T(strings.size() == 4, NStr::Format( "wrong number of parameters %d", strings.size() ));
      szCaptionKey = strings[0];
      szMessageKey = strings[1];
      bDoubleButton = NStr::ToInt(strings[2]);
      szGlobalVarOnOk = strings[3];
    }
  }
};

#endif // __INTERFACEMESSAGEBOX_H__