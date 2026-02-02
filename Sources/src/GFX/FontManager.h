#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Misc/BasicShare.h"
#include "Font.h"

BASIC_SHARE_DECLARE(CFontShare, std::string, CFont, GFX_FONT, 104, "\\1.tfd");

class CFontManager : public IFontManager
{
  OBJECT_COMPLETE_METHODS(CFontManager);
  DECLARE_SERIALIZE;
  //
  CFontShare share;

public:
  void SetSerialMode(ESharedDataSerialMode eSerialMode) override { share.SetSerialMode(eSerialMode); }
  // setup sharing mode
  void SetShareMode(ESharedDataSharingMode eShareMode) override { share.SetShareMode(eShareMode); }
  // remove all shared resource from this manager
  void Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool Init() override { return share.Init(); }
  //
  IGFXFont * GetFont(const char *pszName) override { return share.Get(pszName); }
};

#endif // __FONTMANAGER_H__