#ifndef __ICONHPBAR_H__
#define __ICONHPBAR_H__

#pragma once

class CIconHPBar : public CTRefCount<ISceneIconBar>
{
  OBJECT_SERVICE_METHODS(CIconHPBar);
  DECLARE_SERIALIZE;
  //
  CPtr<IGFXTexture> pTexture;// picture
  CVec3 vPos;// position, relative to parent
  CVec2 vSize;// absolute size
  float fPercentage;// percentage of length
  bool bEnable;// enable icon drawing
  SSpriteInfo infoMain, infoBar;// temporal structure - main & bar
  bool bMultiplayer;
  bool bBarLocked;
  DWORD dwBarColor;
  //
  void CalcSpriteInfo();

public:
  CIconHPBar();
  //
  void Init(IGFXTexture *_pTexture);
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void STDCALL SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void STDCALL LockBarColor() override;
  void STDCALL UnlockBarColor() override;
  void STDCALL SetBorderColor(DWORD dwColor) override;
  void STDCALL ForceThinIcon() override;
  void STDCALL SetColor(DWORD _color) override;
  void STDCALL SetAlpha(BYTE alpha) override;
  void STDCALL SetSize(const CVec2 &_vSize, bool bHorizontal = true) override;
  void STDCALL SetLength(float _fPercentage) override;
  const CVec2 STDCALL GetSize() override { return vSize; }
  //
  void STDCALL Reposition(const CVec3 &vParentPos) override;
  void STDCALL Enable(bool _bEnable) override { bEnable = _bEnable; }
};

#endif // __ICONHPBAR_H__