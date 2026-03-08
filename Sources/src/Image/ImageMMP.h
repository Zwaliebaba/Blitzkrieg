#ifndef __IMAGEMMP_H__
#define __IMAGEMMP_H__

#include "../Formats/fmtTexture.h"

class CImageDDS : public IDDSImage
{
  OBJECT_MINIMAL_METHODS(CImageDDS);
  //
  std::vector<std::vector<BYTE>> mips;
  int nSizeX, nSizeY;
  SDDSPixelFormat format;

public:
  CImageDDS(int nSizeX, int nSizeY, const SDDSPixelFormat &_format);
  // size(x, y, z:)
  int STDCALL GetSizeX(const int nMipLevel) const override { return nSizeX >> nMipLevel; }
  int STDCALL GetSizeY(const int nMipLevel) const override { return nSizeY >> nMipLevel; }
  int STDCALL GetNumMipLevels() const override { return mips.size(); }
  // format...
  const struct SDDSPixelFormat * STDCALL GetDDSFormat() const override { return &format; }
  EGFXPixelFormat STDCALL GetGFXFormat() const override;
  int STDCALL GetBPP() const override { return ::GetBPP(GetGFXFormat()); }
  //
  const void * STDCALL GetLFB(const int nMipLevel) const override { return &(mips[nMipLevel][0]); }
  void * STDCALL GetLFB(const int nMipLevel = 0) override { return &(mips[nMipLevel][0]); }
  // add next mip level
  bool AddMipLevel(const void *pData, int nLength);
  bool AddMipLevels(const IDDSImage *pImage);
  //
  void SetNumMipLevels(const int nNumLevels)
  {
    const int nBPP = GetBPP();
    mips.resize(nNumLevels);
    for (int i = 0; i < nNumLevels; ++i) mips[i].resize((nSizeX >> i) * (nSizeY >> i) * nBPP / 8);
  }

  std::vector<BYTE> &GetMipLevel(const int nLevel) { return mips[nLevel]; }
  const std::vector<BYTE> &GetMipLevel(const int nLevel) const { return mips[nLevel]; }
  // this function is needed to speed up image conversion by removing one memcpy
  std::vector<BYTE> &AddEmptyMipLevel()
  {
    mips.resize(mips.size() + 1);
    return mips.back();
  }
};

const bool GetDDSPixelFormat(EGFXPixelFormat format, SDDSPixelFormat *pFormat);

namespace NImage
{
  bool RecognizeFormatDDS(IDataStream *pStream);
  CImageDDS *LoadImageDDS(IDataStream *pStream);
  bool SaveImageAsDDS(IDataStream *pStream, const IDDSImage *pImage);
};

#endif // __IMAGEMMP_H__