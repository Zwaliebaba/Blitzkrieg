#ifndef __ICONPIC_H__
#define __ICONPIC_H__

#pragma once

class CIconPic : public ISceneIconPic
{
  OBJECT_NORMAL_METHODS(CIconPic);
  DECLARE_SERIALIZE;
  //
  CVec3 vPos;// relative position
  CPtr<IGFXTexture> pTexture;// texture
  bool bEnable;// enable icon drawing
  SSpriteInfo info;// temporal structure
public:
  CIconPic();
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  // drawing
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  // position
  void STDCALL SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void STDCALL Reposition(const CVec3 &_vPos) override { info.pos = _vPos + vPos; }
  const CVec2 STDCALL GetSize() override { return pTexture == nullptr ? VNULL2 : CVec2(pTexture->GetSizeX(0), pTexture->GetSizeY(0)); }
  // color
  void STDCALL SetColor(DWORD _color) override { info.color = (info.color & 0xff000000) | (_color & 0x00ffffff); }
  void STDCALL SetAlpha(BYTE alpha) override { info.color = (info.color & 0x00ffffff) | (static_cast<DWORD>(alpha) << 24); }
  // enable this icon
  void STDCALL Enable(bool _bEnable) override { bEnable = _bEnable; }
  //
  void STDCALL SetTexture(IGFXTexture *_pTexture) override
  {
    pTexture = _pTexture;
    info.pTexture = _pTexture;
  }

  void STDCALL SetRect(const CTRect<short> &rect, const CTRect<float> &maps) override
  {
    info.rect = rect;
    info.maps = maps;
  }
};

#endif // __ICONPIC_H__