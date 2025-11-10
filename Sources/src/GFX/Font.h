#ifndef __FONT_H__
#define __FONT_H__

#pragma once

#include "../Formats/fmtFont.h"
#include "GFXHelper.h"
#include "GFXTextVisitors.h"

class CFont : public IGFXFont
{
  OBJECT_COMPLETE_METHODS(CFont);
  DECLARE_SERIALIZE;
  SHARED_RESOURCE_METHODS(nRefData.a, "Font.Metrics");
  //
  SFontFormat format;// all the font format data
  CPtr<IGFXBaseTexture> pTexture;// font texture
  // visit text
  template<class TChar, class TVisitor>
  const float VisitText(const TChar *pszStringBegin, const TChar *pszStringEnd, float sx, const float sy, TVisitor &visitor) const
  {
    TChar tLastChar = 0;
    // visit all letters
    while (pszStringBegin != pszStringEnd)
    {
      TChar c = *pszStringBegin++;
      //
      const SFontFormat::SCharDesc &character = format.GetChar(c);
      // character's pre-space
      sx += character.fA;
      // add core to this pair
      sx += format.GetKern(tLastChar, c);
      //
      visitor(character, sx, sy);
      //
      sx += character.fB + character.fC;
      tLastChar = c;
    }
    return sx;
  }

  // get precise width of the string
  template<class TChar>
  const float GetTextWidthLocal(const TChar *pszString, int nCounter) const { return VisitText(pszString, pszString + nCounter, 0, 0, CTextWidthVisitor()); }

public:
  CFont() {}
  //
  bool Init(const SFontFormat &_format, IGFXTexture *_pTexture)
  {
    format = _format;
    pTexture = _pTexture;
    return true;
  }

  //
  const SFontFormat & STDCALL GetFormat() const override { return format; }
  //
  void STDCALL SwapData(ISharedResource *pResource) override;
  // font measurements
  int STDCALL GetHeight() const override { return format.metrics.nHeight; }
  int STDCALL GetLineSpace() const override { return format.GetLineSpace(); }
  int STDCALL GetAscent() const override { return format.metrics.nAscent; }
  int STDCALL GetDescent() const override { return format.metrics.nDescent; }
  int STDCALL GetTextWidth(const char *pszString, int nCounter = 2000000000) const override { return GetTextWidthLocal(pszString, nCounter); }
  virtual int STDCALL GetTextWidth(const wchar_t *pszString, int nCounter = 2000000000) const { return GetTextWidthLocal(pszString, nCounter); }
  virtual int STDCALL EstimateTextWidth(const char *pszString) const;
  // fill geometry data for string (w/o any special characters)
  bool FillGeometryData(const char *pszString, float sx, float sy,
                        DWORD dwColor, DWORD dwSpecular,
                        std::vector<SGFXLVertex> &vertices, std::vector<WORD> &indices) const;
  bool FillGeometryData(const wchar_t *pszString, float sx, float sy,
                        DWORD dwColor, DWORD dwSpecular,
                        std::vector<SGFXLVertex> &vertices, std::vector<WORD> &indices) const;
  //
  IGFXBaseTexture *GetTexture() { return pTexture; }
  // internal container clearing
  void STDCALL ClearInternalContainer() override {}
  bool STDCALL Load(bool bPreLoad = false) override;
};

#endif // __FONT_H__