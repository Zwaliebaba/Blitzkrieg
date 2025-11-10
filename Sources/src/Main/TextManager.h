#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__

#pragma once

#include "../Misc/BasicShare.h"
#include "TextSystem.h"
#include "TextObject.h"

class CTextStringShare : public CBasicShare<std::string, CTextString, TEXT_STRING>
{
protected:
  // can't create new objects... only managing and serializing
  CTextString *Create(const std::string &key) override { return nullptr; }

public:
  CTextStringShare() : CBasicShare<std::string, CTextString, TEXT_STRING>(111) {}
};

BASIC_SHARE_DECLARE(CTextDialogShare, std::string, CTextDialog, TEXT_DIALOG, 112, ".txt");

class CTextManager : public ITextManager
{
  OBJECT_COMPLETE_METHODS(CTextManager);
  DECLARE_SERIALIZE;
  //
  CTextStringShare shareString;
  CTextDialogShare shareDialog;

public:
  void STDCALL SetSerialMode(ESharedDataSerialMode eSerialMode) override { shareDialog.SetSerialMode(eSerialMode); }
  // setup sharing mode
  void STDCALL SetShareMode(ESharedDataSharingMode eShareMode) override { shareDialog.SetShareMode(eShareMode); }
  // remove all shared resource from this manager
  void STDCALL Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool STDCALL Init() override;
  bool STDCALL AddTextFile(const char *pszFileName) override;
  //
  IText * STDCALL GetString(const char *pszKey) override;
  IText * STDCALL GetDialog(const char *pszKey) override { return shareDialog.Get(pszKey); }
  //
  const char * STDCALL GetTextName(IText *pText) override
  {
    if (auto pDlg = dynamic_cast<CTextDialog *>(pText))
    {
      const std::string *pName = shareDialog.GetKey(pDlg);
      return pName != nullptr ? pName->c_str() : "";
    }
    if (auto pStr = dynamic_cast<CTextString *>(pText))
    {
      const std::string *pName = shareString.GetKey(pStr);
      return pName != nullptr ? pName->c_str() : "";
    }
    return "";
  }
};

#endif		// __TEXTURE_MANAGER_H__