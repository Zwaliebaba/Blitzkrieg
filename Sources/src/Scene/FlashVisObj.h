#ifndef __FLASHVISOBJ_H__
#define __FLASHVISOBJ_H__

#pragma once

class CFlashVisObj : public CTRefCount<IFlashVisObj>
{
  OBJECT_SERVICE_METHODS(CFlashVisObj);
  DECLARE_SERIALIZE;
  //
  CPtr<IGFXTexture> pTexture;// texture reference
  NTimer::STime timeStart;// time, flash begins
  NTimer::STime timeDuration;// flash duration
  SSpriteInfo spriteInfo;// complete sprite info
  DWORD dwAlpha;// base alpha
  //
  void SetAlpha(DWORD alpha) { spriteInfo.color = (spriteInfo.color & 0x00ffffff) | (alpha << 24); }
  //
  CFlashVisObj()
  {
    spriteInfo.color = 0xffffffff;
    spriteInfo.specular = 0xff000000;
    timeStart = timeDuration = 0;
  }

public:
  void SetTexture(IGFXTexture *_pTexture)
  {
    pTexture = _pTexture;
    spriteInfo.pTexture = _pTexture;
  }

  // drawing
  bool Draw(interface IGFX *pGFX) override { return false; }
  // visiting
  void Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
  // update state
  bool Update(const NTimer::STime &time, bool bForced = false) override;
  // placement
  void SetDirection(const int nDirection) override {}
  void SetPosition(const CVec3 &pos) override { spriteInfo.pos = pos; }
  void SetPlacement(const CVec3 &pos, const int nDir) override { SetPosition(pos); }
  const CVec3 & GetPosition() const override { return spriteInfo.pos; }
  int GetDirection() const override { return 0; }
  // opacity & color
  void SetOpacity(BYTE opacity) override { SetAlpha(opacity); }
  void SetColor(DWORD color) override { spriteInfo.color = (spriteInfo.color & 0xff000000) | (color & 0x00ffffff); }
  void SetSpecular(DWORD color) override {}
  // selection / selection test
  void Select(EVisObjSelectionState state) override {}
  EVisObjSelectionState GetSelectionState() const override { return SGVOSS_UNSELECTED; }
  bool IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) override { return false; }
  bool IsHit(const SHMatrix &matTransform, const RECT &rect) override { return false; }
  //
  void Setup(const NTimer::STime &_timeStart, const NTimer::STime &_timeDuration, const int nPower, const DWORD _dwColor) override
  {
    timeStart = _timeStart;
    timeDuration = _timeDuration;
    spriteInfo.rect.Set(-nPower / 2, -nPower / 4, +nPower / 2, +nPower / 4);
    spriteInfo.maps.Set(0, 0, 1, 1);
    spriteInfo.color = _dwColor;
    dwAlpha = (_dwColor >> 24) & 0x000000ff;
  }
};

#endif // __FLASHVISOBJ_H__