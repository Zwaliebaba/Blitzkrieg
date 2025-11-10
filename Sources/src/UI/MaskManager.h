#ifndef __MASK_MANAGER_H__
#define __MASK_MANAGER_H__

#include "../Misc/BasicShare.h"
#include "MaskSystem.h"
#include "UIMask.h"

BASIC_SHARE_DECLARE(CUIMaskShare, std::string, CUIMask, UI_MASK, 113, ".tga");

class CMaskManager : public IMaskManager
{
  OBJECT_COMPLETE_METHODS(CMaskManager);
  DECLARE_SERIALIZE;
  //
  CUIMaskShare maskShare;

public:
  void STDCALL SetSerialMode(ESharedDataSerialMode eSerialMode) override { maskShare.SetSerialMode(eSerialMode); }
  // setup sharing mode
  void STDCALL SetShareMode(ESharedDataSharingMode eShareMode) override { maskShare.SetShareMode(eShareMode); }
  // remove all shared resource from this manager
  void STDCALL Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool STDCALL Init() override;
  //
  IUIMask * STDCALL GetMask(const char *pszKey) override { return maskShare.Get(pszKey); }

  const char * STDCALL GetMaskName(IUIMask *pMask) override
  {
    const std::string *pName = maskShare.GetKey(checked_cast<CUIMask *>(pMask));
    return pName != nullptr ? pName->c_str() : "";
  }
};


#endif		// __MASK_MANAGER_H__