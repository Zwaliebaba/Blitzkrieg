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
  int GetSizeX() const override { return nSizeX; }
  int GetSizeY() const override { return nSizeY; }
  void Set(SColor color) override;
  // set alpha components
  void SetAlpha(BYTE alpha) override;
  bool SetAlpha(const IImage *pAlpha) override;
  // set r,g,b components
  void SetColor(DWORD color) override;
  bool SetColor(const IImage *pColor) override;

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
  const SColor * GetLFB() const override { return &(data[0]); }
  SColor * GetLFB() override { return &(data[0]); }
  const SColor * GetLine(int nLine) const override { return &(data[nLine * nSizeX]); }
  SColor * GetLine(int nLine) override { return &(data[nLine * nSizeX]); }
  // duplication
  IImage * Duplicate() const override;
  // subimage copying
  bool CopyFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  bool CopyFromAB(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  // modulation
  bool ModulateAlphaFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  bool ModulateColorFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) override;
  // some usefull operations
  void FlipY() override;
  void Invert() override;
  void InvertAlpha() override;
  void SharpenAlpha(BYTE ref) override;
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