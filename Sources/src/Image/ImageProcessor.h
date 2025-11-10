#ifndef __IMAGEPROCESSOR_H__
#define __IMAGEPROCESSOR_H__

class CImageProcessor : public IImageProcessor
{
  OBJECT_NORMAL_METHODS(CImageProcessor);

public:
  // image loading from the stream
  IImage * STDCALL LoadImage(IDataStream *pStream) const override;
  IDDSImage * STDCALL LoadDDSImage(IDataStream *pStream) const override;
  bool STDCALL SaveImageAsPNG(IDataStream *pStream, const IImage *pImage) const override;
  bool STDCALL SaveImageAsTGA(IDataStream *pStream, const IImage *pImage) const override;
  bool STDCALL SaveImageAsDDS(IDataStream *pStream, const IDDSImage *pImage) const override;
  // image creation
  IImage * STDCALL CreateImage(int nSizeX, int nSizeY) override;
  IImage * STDCALL CreateImage(int nSizeX, int nSizeY, void *pData) override;
  // image manipulation
  IImage * STDCALL CreateScale(const IImage *pImage, float fScaleFactor, EImageScaleMethod method) const override;
  IImage * STDCALL CreateScale(const IImage *pImage, float fScaleX, float fScaleY, EImageScaleMethod method) const override;
  IImage * STDCALL CreateScaleBySize(const IImage *pImage, int nSizeX, int nSizeY, EImageScaleMethod method) const override;
  IImage * STDCALL CreateMip(const IImage *pImage, int nLevel) const override;
  IImage * STDCALL CreateGammaCorrection(IImage *pSrc, float fBrightness, float fContrast, float fGamma) override;
  // image conversion
  IDDSImage * STDCALL Compress(const IImage *pSrcImage, EGFXPixelFormat format) const override;
  IImage * STDCALL Decompress(const IDDSImage *pSrcImage) const override;
  // generate mip-levels and compress
  IDDSImage * STDCALL GenerateAndCompress(const IImage *pSrcImage, EGFXPixelFormat format, int nNumMipLevels) const override;
  // pre-multiplied alpha restoration
  void STDCALL RestoreImage(IImage *pImage, const SColor &bg) override;
  // compose nNumImages to the one image. 
  IImage * STDCALL ComposeImages(IImage **pImages, RECT *pRects, RECT *pRectsMain, int nNumImages, int nSizeX, int nSizeY) const override;
  // generate image
  IImage * STDCALL GenerateImage(int nSizeX, int nSizeY, int nType) override;
};

#endif // __IMAGEPROCESSOR_H__