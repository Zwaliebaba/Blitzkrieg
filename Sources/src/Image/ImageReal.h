#ifndef __IMAGEREAL_H__
#define __IMAGEREAL_H__

#include "Image.h"
#include <vector>

// ************************************************************************************************************************ //
// **
// ** image processing class
// **
// **
// **
// ************************************************************************************************************************ //

class CImage : public IImage
{
  OBJECT_MINIMAL_METHODS(CImage);
  //
  using CImageData = std::vector<SColor>;
  CImageData data;
  int nSizeX;
  int nSizeY;

public:
  CImage(int _nSizeX, int _nSizeY, const std::vector<DWORD> &_data);
  CImage(int _nSizeX, int _nSizeY) : data(_nSizeX * _nSizeY), nSizeX(_nSizeX), nSizeY(_nSizeY) {}
  // access: set/get color values
  int STDCALL GetSizeX() const override { return nSizeX; }
  int STDCALL GetSizeY() const override { return nSizeY; }
  void STDCALL Set(SColor color) override;
  // set alpha components
  void STDCALL SetAlpha(BYTE alpha) override;
  bool STDCALL SetAlpha(const IImage *pAlpha) override;
  // set r,g,b components
  void STDCALL SetColor(DWORD color) override;
  bool STDCALL SetColor(const IImage *pColor) override;

  SColor Get(int nX, int nY) const { return data[nY * nSizeX + nX]; }
  void Set(int nX, int nY, DWORD dwColor) { data[nY * nSizeX + nX].color = dwColor; }
  void Set(DWORD dwColor) { std::fill(data.begin(), data.end(), dwColor); }
  void SetAlpha(int nX, int nY, DWORD dwColor) { data[nY * nSizeX + nX] = (data[nY * nSizeX + nX].color & 0x00FFFFFF) | (dwColor & 0xFF000000); }
  // direct data access
  const SColor &operator()(int nX, int nY) const { return data[nY * nSizeX + nX]; }
  SColor &operator()(int nX, int nY) { return data[nY * nSizeX + nX]; }
  const SColor *operator[](int nY) const { return &(data[nY * nSizeX]); }
  SColor *operator[](int nY) { return &(data[nY * nSizeX]); }
  // access to the image as a Linear Frame Buffer (LFB)
  const SColor * STDCALL GetLFB() const override { return &(data[0]); }
  SColor * STDCALL GetLFB() override { return &(data[0]); }
  const SColor * STDCALL GetLine(int nLine) const override { return &(data[nLine * nSizeX]); }
  SColor * STDCALL GetLine(int nLine) override { return &(data[nLine * nSizeX]); }
  // duplication
  IImage * STDCALL Duplicate() const override;
  // subimage copying
  bool STDCALL CopyFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  bool STDCALL CopyFromAB(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  // modulation
  bool STDCALL ModulateAlphaFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  bool STDCALL ModulateColorFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  // some usefull operations
  void STDCALL FlipY() override;
  void STDCALL Invert() override;
  void STDCALL InvertAlpha() override;
  void STDCALL SharpenAlpha(BYTE ref) override;
};

// ************************************************************************************************************************ //
// **
// **pixel format conversion class
// **
// **
// **
// ************************************************************************************************************************ //

struct SPixelConvertInfo
{
  DWORD dwBitDepth;// bit depth of this format
  // Alpha channel info
  DWORD dwAMask;// bit mask
  DWORD dwABits;// # of bits in mask
  DWORD dwAShift;// # of bits to shift down to canonical position
  // Red channel information
  DWORD dwRMask;// bit mask
  DWORD dwRBits;// # of bits in mask
  DWORD dwRShift;// # of bits to shift down to canonical position
  // Green channel info
  DWORD dwGMask;// bit mask
  DWORD dwGBits;// # of bits in mask
  DWORD dwGShift;// # of bits to shift down to canonical position
  // Blue channel Info
  DWORD dwBMask;// bit mask
  DWORD dwBBits;// # of bits in mask
  DWORD dwBShift;// # of bits to shift down to canonical position
  SPixelConvertInfo() {}
  SPixelConvertInfo(DWORD dwABitMask, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask) { InitMaskInfo(dwABitMask, dwRBitMask, dwGBitMask, dwBBitMask); }
  // initialization
  bool InitMaskInfo(DWORD dwABitMask, DWORD dwRBitMask, DWORD dwGBitMask, DWORD dwBBitMask);
  // color composition/decomposition (from ARGB, to ARGB)
  DWORD ComposeColor(DWORD dwColor) const;
  DWORD ComposeColorSlow(DWORD dwColor) const;
  DWORD DecompColor(DWORD dwColor) const;
};

#endif // __IMAGEREAL_H__