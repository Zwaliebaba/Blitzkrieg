#ifndef __CURSOR_H__
#define __CURSOR_H__

#pragma once

#include "../Input/Input.h"

struct SCursorMode
{
  CPtr<IGFXTexture> pTexture;
  std::string szTextureName;
  CTRect<float> rect;
  CVec2 vHotSpot;
  CPtr<ISpriteVisObj> pVisObj;
  int wResourceID;
  //
  int operator&(IStructureSaver &ss);
};

class CCursor : public ICursor
{
  OBJECT_COMPLETE_METHODS(CCursor);
  DECLARE_SERIALIZE;
  //
  CPtr<ITextureManager> pTM;
  //
  EUpdateMode eUpdateMode;// update mode - from Windows and from IInput
  using CCursorsModeMap = std::hash_map<int, SCursorMode>;
  CCursorsModeMap modes;// all loaded modes
  SCursorMode *pMode;// current mode
  int nCurrMode;// current mode index
  SCursorMode *pModifier;// current mode modifier
  int nCurrModifier;// current mode modifier index
  CVec2 vPos;// current cursor position
  bool bShow;// show or not
  bool bPosLocked;// cursor position locked
  CTRect<float> rcBounds;// screen bounds
  float fSensitivity;// sensitivity
  // input sliders
  CPtr<IInputSlider> pScrollX, pScrollY;
  // last unchanged position
  CVec2 vLastPos;// last position
  NTimer::STime timeLast;// last time of the position above
  bool bAcquired;// cursor area control acquired
  //
  bool LoadCursor(int nMode);
  SCursorMode *GetCursor(int nMode);
  void Update();
  void AcquireLocal();

public:
  CCursor();
  //
  void STDCALL Init(ISingleton *pSingleton) override;
  void STDCALL Done() override;
  void STDCALL Clear() override;
  void STDCALL SetUpdateMode(EUpdateMode _eUpdateMode) override;
  void STDCALL OnSetCursor() override;
  //
  void STDCALL RegisterMode(int nMode, const char *pszPictureName, int nSizeX, int nSizeY, int hotX, int hotY, WORD wResourceID) override;
  bool STDCALL SetMode(int nMode) override;
  bool STDCALL SetModifier(int nMode) override;
  void STDCALL Show(bool _bShow) override;
  bool STDCALL IsShown() const override { return bShow; }

  void STDCALL SetBounds(int x1, int y1, int x2, int y2) override;
  void STDCALL Acquire(bool bAcqire) override;
  void STDCALL LockPos(bool bLock) override;
  void STDCALL SetPos(int nX, int nY) override;

  const CVec2 STDCALL GetPos() override
  {
    Update();
    return vPos;
  }

  void STDCALL ResetSliders() override
  {
    pScrollX->Reset();
    pScrollY->Reset();
  }

  // last unchanged position
  void STDCALL GetLastPos(CVec2 *pvPos, NTimer::STime *pTime) const override
  {
    *pvPos = vLastPos;
    *pTime = timeLast;
  }

  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void STDCALL SetSensitivity(float _fSensitivity) override { fSensitivity = _fSensitivity; }
};

#endif // __CURSOR_H__