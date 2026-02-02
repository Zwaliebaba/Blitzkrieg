#ifndef __IMAGEPROCESSOR_H__
#define __IMAGEPROCESSOR_H__

#undef LoadImage

class CImageProcessor : public IImageProcessor
{
  OBJECT_NORMAL_METHODS(CImageProcessor);

public:
  // image loading from the stream
  IImage * LoadImage(IDataStream *pStream) const override;
  IDDSImage * LoadDDSImage(IDataStream *pStream) const override;
  bool SaveImageAsPNG(IDataStream *pStream, const IImage *pImage) const override;
  bool SaveImageAsTGA(IDataStream *pStream, const IImage *pImage) const override;
  bool SaveImageAsDDS(IDataStream *pStream, const IDDSImage *pImage) const override;
  // image creation
  IImage * CreateImage(int nSizeX, int nSizeY) override;
  IImage * CreateImage(int nSizeX, int nSizeY, void *pData) override;
  // image manipulation
  IImage * CreateScale(const IImage *pImage, float fScaleFactor, EImageScaleMethod method) const override;
  IImage * CreateScale(const IImage *pImage, float fScaleX, float fScaleY, EImageScaleMethod method) const override;
  IImage * CreateScaleBySize(const IImage *pImage, int nSizeX, int nSizeY, EImageScaleMethod method) const override;
  IImage * CreateMip(const IImage *pImage, int nLevel) const override;
  IImage * CreateGammaCorrection(IImage *pSrc, float fBrightness, float fContrast, float fGamma) override;
  // image conversion
  IDDSImage * Compress(const IImage *pSrcImage, EGFXPixelFormat format) const override;
  IImage * Decompress(const IDDSImage *pSrcImage) const override;
  // generate mip-levels and compress
  IDDSImage * GenerateAndCompress(const IImage *pSrcImage, EGFXPixelFormat format, int nNumMipLevels) const override;
  // pre-multiplied alpha restoration
  void RestoreImage(IImage *pImage, const SColor &bg) override;
  // compose nNumImages to the one image. 
  IImage * ComposeImages(IImage **pImages, RECT *pRects, RECT *pRectsMain, int nNumImages, int nSizeX, int nSizeY) const override;
  // generate image
  IImage * GenerateImage(int nSizeX, int nSizeY, int nType) override;
};

#endif // __IMAGEPROCESSOR_H__