#ifndef __UI_MASK_H__
#define __UI_MASK_H__

#include "MaskSystem.h"

class CUIMask : public IUIMask
{
  OBJECT_COMPLETE_METHODS(CUIMask);
  SHARED_RESOURCE_METHODS(nRefData.a, "Mask");
  //
  CArray2D<BYTE> data;

  friend class CMaskObjectLoader;// to load a mask directly into an object
public:
  void STDCALL SwapData(ISharedResource *pResource) override;
  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
  //
  const CArray2D<BYTE> * STDCALL GetMask() override { return &data; }
};

#endif		// __UI_MASK_H__