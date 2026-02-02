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
  using CCursorsModeMap = std::unordered_map<int, SCursorMode>;
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
  void Init(ISingleton *pSingleton) override;
  void Done() override;
  void Clear() override;
  void SetUpdateMode(EUpdateMode _eUpdateMode) override;
  void OnSetCursor() override;
  //
  void RegisterMode(int nMode, const char *pszPictureName, int nSizeX, int nSizeY, int hotX, int hotY, WORD wResourceID) override;
  bool SetMode(int nMode) override;
  bool SetModifier(int nMode) override;
  void Show(bool _bShow) override;
  bool IsShown() const override { return bShow; }

  void SetBounds(int x1, int y1, int x2, int y2) override;
  void Acquire(bool bAcqire) override;
  void LockPos(bool bLock) override;
  void SetPos(int nX, int nY) override;

  const CVec2 GetPos() override
  {
    Update();
    return vPos;
  }

  void ResetSliders() override
  {
    pScrollX->Reset();
    pScrollY->Reset();
  }

  // last unchanged position
  void GetLastPos(CVec2 *pvPos, NTimer::STime *pTime) const override
  {
    *pvPos = vLastPos;
    *pTime = timeLast;
  }

  // update object
  bool Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool Draw(IGFX *pGFX) override;
  // visiting
  void Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void SetSensitivity(float _fSensitivity) override { fSensitivity = _fSensitivity; }
};

#endif // __CURSOR_H__