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
  bool Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  //
  bool Draw(IGFX *pGFX) override;
  // visiting
  void Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void LockBarColor() override;
  void UnlockBarColor() override;
  void SetBorderColor(DWORD dwColor) override;
  void ForceThinIcon() override;
  void SetColor(DWORD _color) override;
  void SetAlpha(BYTE alpha) override;
  void SetSize(const CVec2 &_vSize, bool bHorizontal = true) override;
  void SetLength(float _fPercentage) override;
  const CVec2 GetSize() override { return vSize; }
  //
  void Reposition(const CVec3 &vParentPos) override;
  void Enable(bool _bEnable) override { bEnable = _bEnable; }
};

#endif // __ICONHPBAR_H__