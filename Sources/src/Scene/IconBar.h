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
  bool Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool Draw(IGFX *pGFX) override;
  // visiting
  void Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void LockBarColor() override {}
  void UnlockBarColor() override {}
  void SetBorderColor(DWORD dwColor) override {}
  void ForceThinIcon() override {}
  void SetColor(DWORD _color) override { info.color = (info.color & 0xff000000) | (_color & 0x00ffffff); }
  void SetAlpha(BYTE alpha) override { info.color = (info.color & 0x00ffffff) | (static_cast<DWORD>(alpha) << 24); }
  void SetSize(const CVec2 &_vSize, bool bHorizontal = true) override;
  void SetLength(float _fPercentage) override;
  const CVec2 GetSize() override { return vSize; }
  //
  void Reposition(const CVec3 &vParentPos) override { info.pos = vParentPos + vPos; }
  void Enable(bool _bEnable) override { bEnable = _bEnable; }
};

#endif // __ICONBAR_H__