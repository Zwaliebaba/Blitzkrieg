#pragma once

#include "../GFX/GFXTypes.h"

#undef LoadImage

enum
{
  IMAGE_BASE_VALUE = 0x10070000,
  IMAGE_IMAGE = IMAGE_BASE_VALUE + 1,
  IMAGE_PROCESSOR = IMAGE_BASE_VALUE + 2,

  IMAGE_FORCE_DWORD = 0x7fffffff
};

enum EImageScaleMethod
{
  ISM_FILTER = 1,
  ISM_BOX = 2,
  ISM_TRIANGLE = 3,
  ISM_BELL = 4,
  ISM_BSPLINE = 5,
  ISM_LANCZOS3 = 6,
  ISM_MITCHELL = 7,

  ISM_FORCE_DWORD = 0x7fffffff
};

enum EImageGenerationType
{
  IGT_WHITE = 1,
  IGT_BLACK = 2,
  IGT_CHECKER = 3,
  IGT_SHADOW_INDEX1 = 4,
  IGT_SHADOW_INDEX2 = 5,

  IGT_FORCE_DWORD = 0x7fffffff
};

struct SColor
{
  union
  {
    DWORD color;

    struct
    {
      DWORD b : 8;
      DWORD g : 8;
      DWORD r : 8;
      DWORD a : 8;
    };
  };

  //
  SColor() {}
  SColor(DWORD dwColor) : color(dwColor) {}
  SColor(BYTE _a, BYTE _r, BYTE _g, BYTE _b) : b(_b), g(_g), r(_r), a(_a) {}
  //
  operator DWORD() const { return color; }
};

interface IImage : IRefCount
{
  virtual int GetSizeX() const = 0;
  virtual int GetSizeY() const = 0;
  virtual void Set(SColor color) = 0;
  // set alpha components
  virtual void SetAlpha(BYTE alpha) = 0;
  virtual bool SetAlpha(const IImage *pAlpha) = 0;
  // set r,g,b components
  virtual void SetColor(DWORD color) = 0;
  virtual bool SetColor(const IImage *pColor) = 0;
  //
  virtual const SColor * GetLFB() const = 0;
  virtual SColor * GetLFB() = 0;
  virtual const SColor * GetLine(int nLine) const = 0;
  virtual SColor * GetLine(int nLine) = 0;
  // duplication
  virtual IImage * Duplicate() const = 0;
  // subimage copying
  virtual bool CopyFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) = 0;
  virtual bool CopyFromAB(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) = 0;
  // modulation
  virtual bool ModulateAlphaFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) = 0;
  virtual bool ModulateColorFrom(IImage *pSrc, const RECT *pSrcRect, int nPosX, int nPosY) = 0;
  //
  virtual void FlipY() = 0;
  virtual void Invert() = 0;
  virtual void InvertAlpha() = 0;
  virtual void SharpenAlpha(BYTE ref) = 0;
};

interface IDDSImage : IRefCount
{
  // size(x, y, z:)
  virtual int GetSizeX(int nMipLevel = 0) const = 0;
  virtual int GetSizeY(int nMipLevel = 0) const = 0;
  virtual int GetNumMipLevels() const = 0;
  // format...
  virtual const struct SDDSPixelFormat * GetDDSFormat() const = 0;
  virtual EGFXPixelFormat GetGFXFormat() const = 0;
  virtual int GetBPP() const = 0;
  //
  virtual const void * GetLFB(int nMipLevel = 0) const = 0;
  virtual void * GetLFB(int nMipLevel = 0) = 0;
};

interface IImageProcessor : IRefCount
{
  enum { tidTypeID = IMAGE_PROCESSOR };

  // image loading from the stream
  virtual IImage * LoadImage(IDataStream *pStream) const = 0;
  virtual IDDSImage * LoadDDSImage(IDataStream *pStream) const = 0;
  virtual bool SaveImageAsPNG(IDataStream *pStream, const IImage *pImage) const = 0;
  virtual bool SaveImageAsTGA(IDataStream *pStream, const IImage *pImage) const = 0;
  virtual bool SaveImageAsDDS(IDataStream *pStream, const IDDSImage *pImage) const = 0;
  // image creation
  virtual IImage * CreateImage(int nSizeX, int nSizeY) = 0;
  virtual IImage * CreateImage(int nSizeX, int nSizeY, void *pData) = 0;
  // image manipulation
  virtual IImage * CreateScale(const IImage *pImage, float fScaleFactor, EImageScaleMethod method) const = 0;
  virtual IImage * CreateScale(const IImage *pImage, float fScaleX, float fScaleY, EImageScaleMethod method) const = 0;
  virtual IImage * CreateScaleBySize(const IImage *pImage, int nSizeX, int nSizeY, EImageScaleMethod method) const = 0;
  virtual IImage * CreateMip(const IImage *pImage, int nLevel) const = 0;
  virtual IImage * CreateGammaCorrection(IImage *pSrc, float fBrightness, float fContrast, float fGamma) = 0;
  // image conversion
  virtual IDDSImage * Compress(const IImage *pSrcImage, EGFXPixelFormat format) const = 0;
  virtual IImage * Decompress(const IDDSImage *pSrcImage) const = 0;
  // generate mip-levels and compress
  virtual IDDSImage * GenerateAndCompress(const IImage *pSrcImage, EGFXPixelFormat format, int nNumMipLevels) const = 0;
  // pre-multiplied alpha restoration
  virtual void RestoreImage(IImage *pImage, const SColor &bg) = 0;
  // compose nNumImages to the one image. 
  virtual IImage * ComposeImages(IImage **pImages, RECT *pRects, RECT *pRectsMain, int nNumImages, int nSizeX = 0, int nSizeY = 0) const = 0;
  // generate image
  virtual IImage * GenerateImage(int nSizeX, int nSizeY, int nType) = 0;
};

inline IImageProcessor *GetImageProcessor() { return GetSingleton<IImageProcessor>(); }

template<class TColor = SColor, class TImage = IImage, class TImagePtr = CPtr<TImage>>
class CTImageAccessor
{
  TImagePtr pImage;
  std::vector<TColor *> rows;
  //
  void Set(TImage *_pImage)
  {
    pImage = _pImage;
    if (pImage)
    {
      rows.resize(pImage->GetSizeY());
      TColor *pColors = reinterpret_cast<TColor *>(pImage->GetLFB());
      const int nSizeX = pImage->GetSizeX();
      for (int i = 0; i < rows.size(); ++i) rows[i] = pColors + i * nSizeX;
    }
  }

public:
  CTImageAccessor() {}
  CTImageAccessor(TImage *_pImage) { Set(_pImage); }
  // image assigning and extracting
  const CTImageAccessor &operator=(TImage *_pImage)
  {
    Set(_pImage);
    return *this;
  }

  const CTImageAccessor &operator=(const CTImageAccessor &accessor)
  {
    Set(accessor.pImage);
    return *this;
  }

  operator TImage *() const { return pImage; }
  TImage *operator->() const { return pImage; }
  // comparison operators
  bool operator==(const CTImageAccessor &ptr) const { return (pImage == ptr.pImage); }
  bool operator==(TImage *pNewObject) const { return (pImage == pNewObject); }
  bool operator!=(const CTImageAccessor &ptr) const { return (pImage != ptr.pImage); }
  bool operator!=(TImage *pNewObject) const { return (pImage != pNewObject); }
  //
  const TColor *operator[](int nY) const { return rows[nY]; }
  TColor *operator[](int nY) { return rows[nY]; }
  const TColor &operator()(int nX, int nY) const { return rows[nY][nX]; }
  TColor &operator()(int nX, int nY) { return rows[nY][nX]; }
};

using CImageAccessor = CTImageAccessor<>;
using CImagePtrAccessor = CTImageAccessor<SColor, IImage, IImage *>;

enum ECompressionType
{
  COMPRESSION_DXT,
  COMPRESSION_LOW_QUALITY,
  COMPRESSION_HIGH_QUALITY,
};

