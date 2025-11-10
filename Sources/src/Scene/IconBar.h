#ifndef __ICONBAR_H__
#define __ICONBAR_H__

#pragma once

class CIconBar : public ISceneIconBar
{
  OBJECT_NORMAL_METHODS(CIconBar);
  DECLARE_SERIALIZE;
  //
  CVec2 vSize;// absolute size
  bool bHorizontal;// horizontal or vertical bar (for percentage treating)
  CVec3 vPos;// position, relative to parent
  float fPercentage;// percentage of length
  bool bEnable;// enable icon drawing
  SSpriteInfo info;// temporal structure - main bar
  //
  void CalcSpriteInfo();

public:
  CIconBar();
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void STDCALL SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void STDCALL LockBarColor() override {}
  void STDCALL UnlockBarColor() override {}
  void STDCALL SetBorderColor(DWORD dwColor) override {}
  void STDCALL ForceThinIcon() override {}
  void STDCALL SetColor(DWORD _color) override { info.color = (info.color & 0xff000000) | (_color & 0x00ffffff); }
  void STDCALL SetAlpha(BYTE alpha) override { info.color = (info.color & 0x00ffffff) | (static_cast<DWORD>(alpha) << 24); }
  void STDCALL SetSize(const CVec2 &_vSize, bool bHorizontal = true) override;
  void STDCALL SetLength(float _fPercentage) override;
  const CVec2 STDCALL GetSize() override { return vSize; }
  //
  void STDCALL Reposition(const CVec3 &vParentPos) override { info.pos = vParentPos + vPos; }
  void STDCALL Enable(bool _bEnable) override { bEnable = _bEnable; }
};

#endif // __ICONBAR_H__