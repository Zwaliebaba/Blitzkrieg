#ifndef __ICONTEXT_H__
#define __ICONTEXT_H__

#pragma once

class CIconText : public ISceneIconText
{
  OBJECT_NORMAL_METHODS(CIconText);
  DECLARE_SERIALIZE;
  //
  CPtr<IGFXFont> pFont;// font to draw text
  std::string szText;// text to draw
  CVec3 vPos;// relative position
  CVec3 vAbsPos;// absolute world position
  DWORD color;// modulation color
  bool bEnable;// enable to draw this icon?
public:
  // update object
  bool STDCALL Update(const NTimer::STime &time, bool bForced = false) override { return false; }
  // drawing
  bool STDCALL Draw(IGFX *pGFX) override;
  // visiting
  void STDCALL Visit(ISceneVisitor *pVisitor, int nType = -1) override;
  //
  void STDCALL SetPosition(const CVec3 &_vPos) override { vPos = _vPos; }
  void STDCALL Reposition(const CVec3 &vWorldPos) override { vAbsPos = vWorldPos + vPos; }
  const CVec2 STDCALL GetSize() override { return pFont == nullptr ? VNULL2 : CVec2(pFont->GetTextWidth(szText.c_str()), pFont->GetHeight()); }
  //
  void STDCALL SetColor(DWORD _color) override { color = (color & 0xff000000) | (_color & 0x00ffffff); }
  void STDCALL SetAlpha(BYTE alpha) override { color = (color & 0x00ffffff) | (static_cast<DWORD>(alpha) << 24); }
  void STDCALL SetFont(IGFXFont *_pFont) override { pFont = _pFont; }
  void STDCALL SetText(const char *pszText) override { szText = pszText; }

  void STDCALL Enable(bool _bEnable) override { bEnable = _bEnable; }
};

#endif // __ICONTEXT_H__