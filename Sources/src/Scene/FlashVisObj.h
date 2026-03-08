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
  bool STDCALL Draw(interface IGFX *pGFX) override { return false; }
  // visiting
  void STDCALL Visit(interface ISceneVisitor *pVisitor, int nType = -1) override;
  // update state
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override;
  // placement
  void STDCALL SetDirection(const int nDirection) override {}
  void STDCALL SetPosition(const CVec3 &pos) override { spriteInfo.pos = pos; }
  void STDCALL SetPlacement(const CVec3 &pos, const int nDir) override { SetPosition(pos); }
  const CVec3 & STDCALL GetPosition() const override { return spriteInfo.pos; }
  int STDCALL GetDirection() const override { return 0; }
  // opacity & color
  void STDCALL SetOpacity(BYTE opacity) override { SetAlpha(opacity); }
  void STDCALL SetColor(DWORD color) override { spriteInfo.color = (spriteInfo.color & 0xff000000) | (color & 0x00ffffff); }
  void STDCALL SetSpecular(DWORD color) override {}
  // selection / selection test
  void STDCALL Select(EVisObjSelectionState state) override {}
  EVisObjSelectionState STDCALL GetSelectionState() const override { return SGVOSS_UNSELECTED; }
  bool STDCALL IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) override { return false; }
  bool STDCALL IsHit(const SHMatrix &matTransform, const RECT &rect) override { return false; }
  //
  void STDCALL Setup(const NTimer::STime &_timeStart, const NTimer::STime &_timeDuration, const int nPower, const DWORD _dwColor) override
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