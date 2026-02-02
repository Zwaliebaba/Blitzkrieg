#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#pragma once

#include "../Misc/BasicShare.h"
#include "Texture.h"
#include "../Image/Image.h"

BASIC_SHARE_DECLARE(CTextureShare, std::string, CTexture, GFX_TEXTURE, 106, ".tga");

class CTextureManager : public ITextureManager
{
  OBJECT_COMPLETE_METHODS(CTextureManager);
  DECLARE_SERIALIZE;
  //
  CTextureShare share;

public:
  void SetSerialMode(ESharedDataSerialMode eSerialMode) override { share.SetSerialMode(eSerialMode); }
  // setup sharing mode
  void SetShareMode(ESharedDataSharingMode eShareMode) override { share.SetShareMode(eShareMode); }
  // remove all shared resource from this manager
  void Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool Init() override { return share.Init(); }
  //
  IGFXTexture * GetTexture(const char *pszName) override { return share.Get(pszName); }
  //
  const char * GetTextureName(IGFXTexture *pTexture) override
  {
    const std::string *pName = share.GetKey(checked_cast<CTexture *>(pTexture));
    return pName != nullptr ? pName->c_str() : "default";
  }

  //
  void SetQuality(ETextureQuality eQuality) override;
  //
  void ClearContainers() { share.ClearContainers(); }
  void ReloadAllData() { share.ReloadAllData(); }
};

#endif // __TEXTUREMANAGER_H__