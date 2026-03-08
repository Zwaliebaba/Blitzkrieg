#ifndef __SPRITEVISOBJ_H__
#define __SPRITEVISOBJ_H__

#pragma once

#include "ObjVisObj.h"

inline DWORD SetOpacity(DWORD original, BYTE opacity) { return (static_cast<DWORD>(opacity) << 24) | (original & 0x00ffffff); }
inline DWORD SetColor(DWORD original, DWORD color) { return (color & 0x00ffffff) | (original & 0xff000000); }

class CSpriteVisObj : public CTObjVisObj<CTRefCount<ISpriteVisObj>>
{
  OBJECT_SERVICE_METHODS(CSpriteVisObj);
  DECLARE_SERIALIZE;
  //
  CPtr<ISpriteAnimation> pAnim;// animation system
  NTimer::STime nNextIdle;// next idle time
  static DWORD dwIdleData;// idle animation data
  CPtr<IGFXTexture> pTexture;// texture
  //
  SSpriteInfo info;// temp sprite info
  SComplexSpriteInfo info2;// temp complex sprite info
  bool bComplexSprite;
  //
  void RepositionIcons() override;
  void RepositionIconsLocal(DWORD placement);
  void RetrieveSpriteInfo();
  //
  ~CSpriteVisObj() override {}

public:
  CSpriteVisObj();
  //
  bool Init(IGFXTexture *_pTexture, ISpriteAnimation *_pAnimation);
  // update state
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override;
  // scale
  void STDCALL SetScale(float sx, float sy, float sz) override {}
  // placement
  void STDCALL SetDirection(const int _nDirection) override
  {
    SetDir(_nDirection);
    pAnim->SetDirection(GetDir());
  }

  void STDCALL SetPosition(const CVec3 &_pos) override
  {
    SetPos(_pos);
    info.pos = _pos;
  }

  void STDCALL SetPlacement(const CVec3 &pos, const int nDir) override
  {
    SetPosition(pos);
    SetDirection(nDir);
  }

  // opacity
  void STDCALL SetOpacity(BYTE opacity) override
  {
    info.color = ::SetOpacity(info.color, opacity);
    info2.color = ::SetOpacity(info2.color, opacity);
  }

  void STDCALL SetColor(DWORD color) override
  {
    info.color = ::SetColor(info.color, color);
    info2.color = ::SetColor(info2.color, color);
  }

  void STDCALL SetSpecular(DWORD color) override
  {
    info.specular = ::SetColor(info.specular, color);
    info2.specular = ::SetColor(info2.specular, color);
  }

  // animation
  void STDCALL SetAnimation(const int nAnim) override { pAnim->SetAnimation(nAnim); }

  IAnimation * STDCALL GetAnimation() override { return pAnim; }
  // selection / selection test
  bool STDCALL IsHit(const SHMatrix &matTransform, const CVec2 &point, CVec2 *pShift) override;
  bool STDCALL IsHit(const SHMatrix &matTransform, const RECT &rect) override;
  // drawing
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  const SSpriteInfo * STDCALL GetSpriteInfo() const override { return &info; }
  IGFXTexture * STDCALL GetTexture() const override { return pTexture; }
};

#endif // __SPRITEVISOBJ_H__