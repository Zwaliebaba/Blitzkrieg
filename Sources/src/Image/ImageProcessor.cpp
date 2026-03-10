#include "pch.h"
#include "ImageProcessor.h"
#include "ImageBMP.h"
#include "ImagePNG.h"
#include "ImageTGA.h"
#include "ImageMMP.h"

// DXT Compression/Decompression helpers (replaces s3tc.h dependency)
namespace DXTCodec
{
  // Decode a 16-bit 565 color to RGBA
  inline void DecodeColor565(WORD color, BYTE &r, BYTE &g, BYTE &b)
  {
    r = static_cast<BYTE>(((color >> 11) & 0x1F) * 255 / 31);
    g = static_cast<BYTE>(((color >> 5) & 0x3F) * 255 / 63);
    b = static_cast<BYTE>((color & 0x1F) * 255 / 31);
  }

  // Encode RGB to 16-bit 565 color
  inline WORD EncodeColor565(BYTE r, BYTE g, BYTE b)
  {
    return static_cast<WORD>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
  }

  // Decode a DXT1 block (4x4 pixels)
  void DecodeDXT1Block(const BYTE *block, DWORD *outColors, int stride, bool hasPunchthroughAlpha)
  {
    WORD c0 = *reinterpret_cast<const WORD *>(block);
    WORD c1 = *reinterpret_cast<const WORD *>(block + 2);
    DWORD indices = *reinterpret_cast<const DWORD *>(block + 4);

    BYTE r[4], g[4], b[4], a[4];
    DecodeColor565(c0, r[0], g[0], b[0]); a[0] = 255;
    DecodeColor565(c1, r[1], g[1], b[1]); a[1] = 255;

    if (c0 > c1)
    {
      r[2] = (2 * r[0] + r[1] + 1) / 3; g[2] = (2 * g[0] + g[1] + 1) / 3; b[2] = (2 * b[0] + b[1] + 1) / 3; a[2] = 255;
      r[3] = (r[0] + 2 * r[1] + 1) / 3; g[3] = (g[0] + 2 * g[1] + 1) / 3; b[3] = (b[0] + 2 * b[1] + 1) / 3; a[3] = 255;
    }
    else
    {
      r[2] = (r[0] + r[1]) / 2; g[2] = (g[0] + g[1]) / 2; b[2] = (b[0] + b[1]) / 2; a[2] = 255;
      r[3] = 0; g[3] = 0; b[3] = 0; a[3] = hasPunchthroughAlpha ? 0 : 255;
    }

    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        int idx = (indices >> (2 * (y * 4 + x))) & 0x3;
        outColors[y * stride + x] = (a[idx] << 24) | (r[idx] << 16) | (g[idx] << 8) | b[idx];
      }
    }
  }

  // Decode a DXT3 block (4x4 pixels with explicit alpha)
  void DecodeDXT3Block(const BYTE *block, DWORD *outColors, int stride)
  {
    // First 8 bytes are explicit alpha (4 bits per pixel)
    WORD alphaBlock[4];
    for (int i = 0; i < 4; ++i)
      alphaBlock[i] = *reinterpret_cast<const WORD *>(block + i * 2);

    // Decode color block (same as DXT1, but without punch-through)
    DecodeDXT1Block(block + 8, outColors, stride, false);

    // Apply explicit alpha
    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        int alphaIdx = y * 4 + x;
        BYTE alpha = static_cast<BYTE>(((alphaBlock[y] >> (x * 4)) & 0xF) * 255 / 15);
        outColors[y * stride + x] = (outColors[y * stride + x] & 0x00FFFFFF) | (alpha << 24);
      }
    }
  }

  // Decode a DXT5 block (4x4 pixels with interpolated alpha)
  void DecodeDXT5Block(const BYTE *block, DWORD *outColors, int stride)
  {
    // First 2 bytes are alpha endpoints
    BYTE a0 = block[0];
    BYTE a1 = block[1];

    // Next 6 bytes are alpha indices (3 bits per pixel, 48 bits total)
    BYTE alphas[8];
    alphas[0] = a0;
    alphas[1] = a1;
    if (a0 > a1)
    {
      alphas[2] = (6 * a0 + 1 * a1 + 3) / 7;
      alphas[3] = (5 * a0 + 2 * a1 + 3) / 7;
      alphas[4] = (4 * a0 + 3 * a1 + 3) / 7;
      alphas[5] = (3 * a0 + 4 * a1 + 3) / 7;
      alphas[6] = (2 * a0 + 5 * a1 + 3) / 7;
      alphas[7] = (1 * a0 + 6 * a1 + 3) / 7;
    }
    else
    {
      alphas[2] = (4 * a0 + 1 * a1 + 2) / 5;
      alphas[3] = (3 * a0 + 2 * a1 + 2) / 5;
      alphas[4] = (2 * a0 + 3 * a1 + 2) / 5;
      alphas[5] = (1 * a0 + 4 * a1 + 2) / 5;
      alphas[6] = 0;
      alphas[7] = 255;
    }

    // Decode alpha indices (48 bits = 16 pixels * 3 bits)
    UINT64 alphaIndices = 0;
    for (int i = 0; i < 6; ++i)
      alphaIndices |= static_cast<UINT64>(block[2 + i]) << (i * 8);

    // Decode color block
    DecodeDXT1Block(block + 8, outColors, stride, false);

    // Apply interpolated alpha
    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        int pixelIdx = y * 4 + x;
        int alphaIdx = static_cast<int>((alphaIndices >> (pixelIdx * 3)) & 0x7);
        outColors[y * stride + x] = (outColors[y * stride + x] & 0x00FFFFFF) | (alphas[alphaIdx] << 24);
      }
    }
  }

  // Calculate compressed size for DXT format
  int GetCompressedSize(int width, int height, EGFXPixelFormat format)
  {
    int blockSize = (format == GFXPF_DXT1) ? 8 : 16;
    int blocksX = (width + 3) / 4;
    int blocksY = (height + 3) / 4;
    return blocksX * blocksY * blockSize;
  }

  // Simple DXT1 compression (basic quality - finds min/max colors)
  void CompressDXT1Block(const DWORD *srcColors, int srcStride, BYTE *outBlock)
  {
    // Find min and max colors
    BYTE minR = 255, minG = 255, minB = 255;
    BYTE maxR = 0, maxG = 0, maxB = 0;
    bool hasTransparent = false;

    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        DWORD c = srcColors[y * srcStride + x];
        BYTE a = (c >> 24) & 0xFF;
        BYTE r = (c >> 16) & 0xFF;
        BYTE g = (c >> 8) & 0xFF;
        BYTE b = c & 0xFF;

        if (a < 128) { hasTransparent = true; continue; }
        if (r < minR) minR = r; if (r > maxR) maxR = r;
        if (g < minG) minG = g; if (g > maxG) maxG = g;
        if (b < minB) minB = b; if (b > maxB) maxB = b;
      }
    }

    WORD c0 = EncodeColor565(maxR, maxG, maxB);
    WORD c1 = EncodeColor565(minR, minG, minB);

    // Ensure c0 > c1 for 4-color mode, or c0 <= c1 for punch-through
    if (!hasTransparent && c0 < c1) { WORD tmp = c0; c0 = c1; c1 = tmp; }
    else if (hasTransparent && c0 > c1) { WORD tmp = c0; c0 = c1; c1 = tmp; }

    *reinterpret_cast<WORD *>(outBlock) = c0;
    *reinterpret_cast<WORD *>(outBlock + 2) = c1;

    // Build color palette
    BYTE r[4], g[4], b[4];
    DecodeColor565(c0, r[0], g[0], b[0]);
    DecodeColor565(c1, r[1], g[1], b[1]);

    if (c0 > c1)
    {
      r[2] = (2 * r[0] + r[1] + 1) / 3; g[2] = (2 * g[0] + g[1] + 1) / 3; b[2] = (2 * b[0] + b[1] + 1) / 3;
      r[3] = (r[0] + 2 * r[1] + 1) / 3; g[3] = (g[0] + 2 * g[1] + 1) / 3; b[3] = (b[0] + 2 * b[1] + 1) / 3;
    }
    else
    {
      r[2] = (r[0] + r[1]) / 2; g[2] = (g[0] + g[1]) / 2; b[2] = (b[0] + b[1]) / 2;
      r[3] = 0; g[3] = 0; b[3] = 0;
    }

    // Find best index for each pixel
    DWORD indices = 0;
    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        DWORD col = srcColors[y * srcStride + x];
        BYTE ca = (col >> 24) & 0xFF;
        BYTE cr = (col >> 16) & 0xFF;
        BYTE cg = (col >> 8) & 0xFF;
        BYTE cb = col & 0xFF;

        int bestIdx = 0;
        if (hasTransparent && ca < 128)
        {
          bestIdx = 3; // Transparent
        }
        else
        {
          int bestDist = INT_MAX;
          int numColors = (c0 > c1) ? 4 : 3;
          for (int i = 0; i < numColors; ++i)
          {
            int dr = cr - r[i], dg = cg - g[i], db = cb - b[i];
            int dist = dr * dr + dg * dg + db * db;
            if (dist < bestDist) { bestDist = dist; bestIdx = i; }
          }
        }
        indices |= (bestIdx << (2 * (y * 4 + x)));
      }
    }
    *reinterpret_cast<DWORD *>(outBlock + 4) = indices;
  }

  // Compress DXT3 block (explicit alpha)
  void CompressDXT3Block(const DWORD *srcColors, int srcStride, BYTE *outBlock)
  {
    // Encode explicit alpha (4 bits per pixel)
    for (int y = 0; y < 4; ++y)
    {
      WORD alphaRow = 0;
      for (int x = 0; x < 4; ++x)
      {
        BYTE a = (srcColors[y * srcStride + x] >> 24) & 0xFF;
        alphaRow |= ((a >> 4) & 0xF) << (x * 4);
      }
      *reinterpret_cast<WORD *>(outBlock + y * 2) = alphaRow;
    }
    // Compress color block
    CompressDXT1Block(srcColors, srcStride, outBlock + 8);
  }

  // Compress DXT5 block (interpolated alpha)
  void CompressDXT5Block(const DWORD *srcColors, int srcStride, BYTE *outBlock)
  {
    // Find min/max alpha
    BYTE minA = 255, maxA = 0;
    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        BYTE a = (srcColors[y * srcStride + x] >> 24) & 0xFF;
        if (a < minA) minA = a;
        if (a > maxA) maxA = a;
      }
    }

    outBlock[0] = maxA;
    outBlock[1] = minA;

    // Build alpha palette
    BYTE alphas[8];
    alphas[0] = maxA;
    alphas[1] = minA;
    if (maxA > minA)
    {
      alphas[2] = (6 * maxA + 1 * minA + 3) / 7;
      alphas[3] = (5 * maxA + 2 * minA + 3) / 7;
      alphas[4] = (4 * maxA + 3 * minA + 3) / 7;
      alphas[5] = (3 * maxA + 4 * minA + 3) / 7;
      alphas[6] = (2 * maxA + 5 * minA + 3) / 7;
      alphas[7] = (1 * maxA + 6 * minA + 3) / 7;
    }
    else
    {
      alphas[2] = (4 * maxA + 1 * minA + 2) / 5;
      alphas[3] = (3 * maxA + 2 * minA + 2) / 5;
      alphas[4] = (2 * maxA + 3 * minA + 2) / 5;
      alphas[5] = (1 * maxA + 4 * minA + 2) / 5;
      alphas[6] = 0;
      alphas[7] = 255;
    }

    // Find best alpha index for each pixel
    UINT64 alphaIndices = 0;
    for (int y = 0; y < 4; ++y)
    {
      for (int x = 0; x < 4; ++x)
      {
        BYTE a = (srcColors[y * srcStride + x] >> 24) & 0xFF;
        int bestIdx = 0, bestDist = INT_MAX;
        for (int i = 0; i < 8; ++i)
        {
          int dist = abs(a - alphas[i]);
          if (dist < bestDist) { bestDist = dist; bestIdx = i; }
        }
        alphaIndices |= static_cast<UINT64>(bestIdx) << ((y * 4 + x) * 3);
      }
    }

    for (int i = 0; i < 6; ++i)
      outBlock[2 + i] = static_cast<BYTE>((alphaIndices >> (i * 8)) & 0xFF);

    // Compress color block
    CompressDXT1Block(srcColors, srcStride, outBlock + 8);
  }
} // namespace DXTCodec

IImage *CImageProcessor::LoadImage(IDataStream *pStream) const
{
  NI_ASSERT_T(pStream != 0, "Can't load to NULL stream");
  //
  if (NImage::RecognizeFormatPNG(pStream)) return NImage::LoadImagePNG(pStream);
  else if (NImage::RecognizeFormatBMP(pStream)) return NImage::LoadImageBMP(pStream);
  else if (NImage::RecognizeFormatTGA(pStream)) return NImage::LoadImageTGA(pStream);
  else return nullptr;
}

IDDSImage *CImageProcessor::LoadDDSImage(IDataStream *pStream) const { return NImage::LoadImageDDS(pStream); }

bool CImageProcessor::SaveImageAsPNG(IDataStream *pStream, const IImage *pImage) const { return NImage::SaveImageAsPNG(pStream, pImage); }

bool CImageProcessor::SaveImageAsTGA(IDataStream *pStream, const IImage *pImage) const { return NImage::SaveImageAsTGA(pStream, pImage); }

bool CImageProcessor::SaveImageAsDDS(IDataStream *pStream, const IDDSImage *pImage) const { return NImage::SaveImageAsDDS(pStream, pImage); }

void ImageScale(const CImage *pSrcImg, CImage *pDstImg, EImageScaleMethod method);

IImage *CImageProcessor::CreateScale(const IImage *pImage, float fScaleFactor, EImageScaleMethod method) const
{
  auto pScale = new CImage(pImage->GetSizeX() * fScaleFactor, pImage->GetSizeY() * fScaleFactor);
  ImageScale(static_cast<const CImage *>(pImage), pScale, method);
  return pScale;
}

IImage *CImageProcessor::CreateScale(const IImage *pImage, float fScaleX, float fScaleY, EImageScaleMethod method) const
{
  auto pScale = new CImage(pImage->GetSizeX() * fScaleX, pImage->GetSizeY() * fScaleY);
  ImageScale(static_cast<const CImage *>(pImage), pScale, method);
  return pScale;
}

IImage *CImageProcessor::CreateScaleBySize(const IImage *pImage, int nSizeX, int nSizeY, EImageScaleMethod method) const
{
  auto pScale = new CImage(nSizeX, nSizeY);
  ImageScale(static_cast<const CImage *>(pImage), pScale, method);
  return pScale;
}

IImage *CImageProcessor::CreateMip(const IImage *pImage, int nLevel) const { return CreateScale(pImage, 1.0 / static_cast<double>(1UL << nLevel), ISM_LANCZOS3); }

IDDSImage *CompressDXTN(const IImage *pImage, EGFXPixelFormat format)
{
  SDDSPixelFormat ddsformat;
  GetDDSPixelFormat(format, &ddsformat);

  int width = pImage->GetSizeX();
  int height = pImage->GetSizeY();
  int blocksX = (width + 3) / 4;
  int blocksY = (height + 3) / 4;
  int blockSize = (format == GFXPF_DXT1) ? 8 : 16;
  int compressedSize = blocksX * blocksY * blockSize;

  // Create output DDS image
  auto pImageMMP = new CImageDDS(width, height, ddsformat);
  std::vector<BYTE> &outdata = pImageMMP->AddEmptyMipLevel();
  outdata.resize(compressedSize);

  const DWORD *srcPixels = reinterpret_cast<const DWORD *>(pImage->GetLFB());
  BYTE *dstBlock = &outdata[0];

  // Temporary buffer for edge blocks that extend beyond image bounds
  DWORD blockPixels[16];

  for (int by = 0; by < blocksY; ++by)
  {
    for (int bx = 0; bx < blocksX; ++bx)
    {
      // Extract 4x4 block (with clamping at edges)
      for (int y = 0; y < 4; ++y)
      {
        for (int x = 0; x < 4; ++x)
        {
          int srcX = Min(bx * 4 + x, width - 1);
          int srcY = Min(by * 4 + y, height - 1);
          blockPixels[y * 4 + x] = srcPixels[srcY * width + srcX];
        }
      }

      // Compress the block based on format
      switch (format)
      {
        case GFXPF_DXT1:
          DXTCodec::CompressDXT1Block(blockPixels, 4, dstBlock);
          break;
        case GFXPF_DXT2:
        case GFXPF_DXT3:
          DXTCodec::CompressDXT3Block(blockPixels, 4, dstBlock);
          break;
        case GFXPF_DXT4:
        case GFXPF_DXT5:
          DXTCodec::CompressDXT5Block(blockPixels, 4, dstBlock);
          break;
      }

      dstBlock += blockSize;
    }
  }

  return pImageMMP;
}

IDDSImage *CompressRGBA(const IImage *pImage, EGFXPixelFormat format)
{
  SPixelConvertInfo pci;
  SDDSPixelFormat ddsformat;
  GetDDSPixelFormat(format, &ddsformat);
  switch (format)
  {
    case GFXPF_ARGB8888:
      pci.InitMaskInfo(0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
      break;
    case GFXPF_ARGB1555:
      pci.InitMaskInfo(0x00008000, 0x00007c00, 0x000003e0, 0x0000001f);
      break;
    case GFXPF_ARGB4444:
      pci.InitMaskInfo(0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f);
      break;
    case GFXPF_ARGB0565:
      pci.InitMaskInfo(0x00000000, 0x0000f800, 0x000007e0, 0x0000001f);
      break;
    default:
      return nullptr;
  }
  //
  int nSizeX = pImage->GetSizeX();
  int nSizeY = pImage->GetSizeY();
  int nBPP = GetBPP(format);

  auto pImageMMP = new CImageDDS(nSizeX, nSizeY, ddsformat);
  std::vector<BYTE> &outdata = pImageMMP->AddEmptyMipLevel();
  outdata.resize(nSizeX * nSizeY * nBPP / 8);

  auto pSrc = reinterpret_cast<const DWORD *>(pImage->GetLFB());
  // std::vector<BYTE> buffer( nSizeX * nSizeY * nBPP / 8 );
  if (nBPP == 16)
  {
    auto pDst = reinterpret_cast<WORD *>(&(outdata[0]));
    for (int i = 0; i < nSizeX * nSizeY; ++i, ++pDst) *pDst = pci.ComposeColorSlow(pSrc[i]);
    // pImageMMP->AddMipLevel( &(outdata[0]), outdata.size() );
    return pImageMMP;
  }
  if (nBPP == 32)
  {
    auto pDst = reinterpret_cast<DWORD *>(&(outdata[0]));
    memcpy(pDst, pSrc, nSizeX * nSizeY * nBPP / 8);
    // pImageMMP->AddMipLevel( &(buffer[0]), buffer.size() );
    return pImageMMP;
  }
  // unsuccessfull load - destroy image
  delete pImageMMP;
  return nullptr;
}

IDDSImage *CImageProcessor::Compress(const IImage *pImage, EGFXPixelFormat format) const
{
  if ((format >= GFXPF_DXT1) && (format <= GFXPF_DXT5)) return CompressDXTN(pImage, format);
  if ((format >= GFXPF_ARGB8888) || (format <= GFXPF_ARGB0565)) return CompressRGBA(pImage, format);
  // CRAP{ still not all formats are realized
  return nullptr;
  // CRAP}
}

IImage *CImageProcessor::Decompress(const IDDSImage *pImage) const
{
  int width = pImage->GetSizeX(0);
  int height = pImage->GetSizeY(0);
  EGFXPixelFormat format = pImage->GetGFXFormat();

  // ARGB8888 is uncompressed - just copy
  if (format == GFXPF_ARGB8888)
  {
    auto pDstImage = new CImage(width, height);
    memcpy(pDstImage->GetLFB(), pImage->GetLFB(), width * height * sizeof(SColor));
    return pDstImage;
  }

  // Handle uncompressed formats by converting
  if (format == GFXPF_ARGB4444 || format == GFXPF_ARGB1555 || format == GFXPF_ARGB0565)
  {
    auto pDstImage = new CImage(width, height);
    DWORD *dstPixels = reinterpret_cast<DWORD *>(pDstImage->GetLFB());
    const void *srcData = pImage->GetLFB(0);

    SPixelConvertInfo pci;
    switch (format)
    {
      case GFXPF_ARGB1555:
        pci.InitMaskInfo(0x00008000, 0x00007c00, 0x000003e0, 0x0000001f);
        break;
      case GFXPF_ARGB4444:
        pci.InitMaskInfo(0x0000f000, 0x00000f00, 0x000000f0, 0x0000000f);
        break;
      case GFXPF_ARGB0565:
        pci.InitMaskInfo(0x00000000, 0x0000f800, 0x000007e0, 0x0000001f);
        break;
    }

    const WORD *srcPixels = reinterpret_cast<const WORD *>(srcData);
    for (int i = 0; i < width * height; ++i)
      dstPixels[i] = pci.DecompColor(srcPixels[i]);

    return pDstImage;
  }

  // Handle DXT compressed formats
  if (format >= GFXPF_DXT1 && format <= GFXPF_DXT5)
  {
    auto pDstImage = new CImage(width, height);
    DWORD *dstPixels = reinterpret_cast<DWORD *>(pDstImage->GetLFB());
    const BYTE *srcBlock = reinterpret_cast<const BYTE *>(pImage->GetLFB(0));

    int blocksX = (width + 3) / 4;
    int blocksY = (height + 3) / 4;
    int blockSize = (format == GFXPF_DXT1) ? 8 : 16;

    // Temporary buffer for decoded block
    DWORD blockPixels[16];

    for (int by = 0; by < blocksY; ++by)
    {
      for (int bx = 0; bx < blocksX; ++bx)
      {
        // Decode block based on format
        switch (format)
        {
          case GFXPF_DXT1:
            DXTCodec::DecodeDXT1Block(srcBlock, blockPixels, 4, true);
            break;
          case GFXPF_DXT2:
          case GFXPF_DXT3:
            DXTCodec::DecodeDXT3Block(srcBlock, blockPixels, 4);
            break;
          case GFXPF_DXT4:
          case GFXPF_DXT5:
            DXTCodec::DecodeDXT5Block(srcBlock, blockPixels, 4);
            break;
        }

        // Copy decoded pixels to destination (handling edge cases)
        for (int y = 0; y < 4; ++y)
        {
          int dstY = by * 4 + y;
          if (dstY >= height) break;
          for (int x = 0; x < 4; ++x)
          {
            int dstX = bx * 4 + x;
            if (dstX >= width) break;
            dstPixels[dstY * width + dstX] = blockPixels[y * 4 + x];
          }
        }

        srcBlock += blockSize;
      }
    }

    return pDstImage;
  }

  // Unknown format
  return nullptr;
}

// generate mip-levels and compress
IDDSImage *CImageProcessor::GenerateAndCompress(const IImage *pSrcImage, EGFXPixelFormat format, int nNumMipLevels) const
{
  SDDSPixelFormat ddsformat;
  GetDDSPixelFormat(format, &ddsformat);
  auto pResultMMP = new CImageDDS(pSrcImage->GetSizeX(), pSrcImage->GetSizeY(), ddsformat);

  CPtr<IDDSImage> pMMP = Compress(pSrcImage, format);
  pResultMMP->AddMipLevels(pMMP);
  for (int i = 1; i < nNumMipLevels; ++i)
  {
    CPtr<IImage> pScaled = CreateMip(pSrcImage, i);
    CPtr<IDDSImage> pMMP = Compress(pScaled, format);
    pResultMMP->AddMipLevels(pMMP);
  }
  //
  return pResultMMP;
}

IImage *CImageProcessor::CreateImage(int nSizeX, int nSizeY) { return new CImage(nSizeX, nSizeY); }

IImage *CImageProcessor::CreateImage(int nSizeX, int nSizeY, void *pData)
{
  auto pImage = new CImage(nSizeX, nSizeY);
  memcpy(pImage->GetLFB(), pData, nSizeX * nSizeY * 4);
  return pImage;
}

void CImageProcessor::RestoreImage(IImage *pImage, const SColor &bg)
{
  // c0 * alpha + bg * (1 - alpha) = c1 => c0 = ( c1 - bg * (1 - alpha) ) / alpha
  // c0 = c0, if alpha == 0
  SColor *pColors = pImage->GetLFB();
  float fBGr = static_cast<float>(bg.r), fBGg = static_cast<float>(bg.g), fBGb = static_cast<float>(bg.b);
  for (int i = 0; i < pImage->GetSizeX() * pImage->GetSizeY(); ++i)
  {
    if (pColors[i].a != 0)
    {
      float fAlpha = static_cast<float>(pColors[i].a) / 255.0f;
      float fValue = (static_cast<float>(pColors[i].r) - fBGr * (1.0f - fAlpha)) / fAlpha;
      pColors[i].r = static_cast<BYTE>(Max(0.0f, Min(fValue, 255.0f)));
      fValue = (static_cast<float>(pColors[i].g) - fBGg * (1.0f - fAlpha)) / fAlpha;
      pColors[i].g = static_cast<BYTE>(Max(0.0f, Min(fValue, 255.0f)));
      fValue = (static_cast<float>(pColors[i].b) - fBGb * (1.0f - fAlpha)) / fAlpha;
      pColors[i].b = static_cast<BYTE>(Max(0.0f, Min(fValue, 255.0f)));
    }
  }
}

IImage *CImageProcessor::GenerateImage(int nSizeX, int nSizeY, int nType)
{
  IImage *pImage = nullptr;
  switch (nType)
  {
    case IGT_WHITE:
      pImage = CreateImage(nSizeX, nSizeY);
      pImage->Set(bit_cast<SColor>(0xffffffff));
      break;
    case IGT_BLACK:
      pImage = CreateImage(nSizeX, nSizeY);
      pImage->Set(bit_cast<SColor>(0xff000000));
      break;
    case IGT_CHECKER:
      pImage = CreateImage(nSizeX, nSizeY);
      for (int i = 0; i < nSizeY; ++i)
      {
        SColor *pColors = pImage->GetLine(i);
        bool bOddY = ((i / (nSizeY / 16)) & 1) != 0;
        for (int j = 0; j < nSizeX; ++j)
        {
          bool bOddX = ((j / (nSizeX / 16)) & 1) != 0;
          pColors[j] = (bOddX == bOddY) ? 0xffffffff : 0xff000000;
        }
      }
      break;
    case IGT_SHADOW_INDEX1:
      NI_ASSERT_T(0, "still not realized");
      break;
    case IGT_SHADOW_INDEX2:
      NI_ASSERT_T(0, "still not realized");
      break;
  }
  return pImage;
}

inline BYTE GetGammaCorrection(BYTE val, float fBrightness, float fPower, float fA, float fB)
{
  const float fVal = static_cast<float>(val) / 255.0f;
  const float fGammaValue = pow(fVal, fPower);
  const float fContrastValue = Clamp(fA * fGammaValue + fB, 0.0f, 1.0f);
  const float fResult = Clamp(fContrastValue + fBrightness, 0.0f, 1.0f);
  return static_cast<BYTE>(fResult * 255.0f);
}

IImage *CImageProcessor::CreateGammaCorrection(IImage *pSrc, float fBrightness, float fContrast, float fGamma)
{
  if ((fBrightness == 0) && (fContrast == 0) && (fGamma == 0)) return CreateImage(pSrc->GetSizeX(), pSrc->GetSizeY(), pSrc->GetLFB());
  //
  IImage *pDst = CreateImage(pSrc->GetSizeX(), pSrc->GetSizeY());
  // build ramp from the brightness, contrast and gamma values
  // y = a*x + b
  // 
  fBrightness = Clamp(fBrightness, -1.0f, 1.0f) * 0.5f;// to avoid complete dark and complete white values
  fContrast = Clamp(fContrast, -1.0f, 1.0f) * 0.5f;
  fGamma = Clamp(fGamma, -1.0f, 1.0f) * 0.5f;
  // calculate equation parameters for Y = A*X + B
  // contrast: a*x + b
  // if contrast < 0, then a = 1/a (tilt <45 degrees)
  float fA = 1.0f + 4.0f * fabs(fContrast);
  if (fContrast < 0) fA = 1.0f / fA;
  float fB = 0.5f * (1.0f - fA);
  // gamma: x^power
  float fPower = 1;
  {
    if (fGamma > 0) fPower = 1.0f / (5.0f * fGamma + 1);
    else if (fGamma < 0) fPower = 1.0f / (0.5f * fGamma + 1);
  }
  // brightness: x + b
  // 
  for (int i = 0; i != pSrc->GetSizeY(); ++i)
  {
    SColor *pDstColor = pDst->GetLine(i);
    SColor *pSrcColor = pSrc->GetLine(i);
    for (int j = 0; j != pSrc->GetSizeX(); ++j)
    {
      pDstColor[j].a = pSrcColor[j].a;
      pDstColor[j].r = GetGammaCorrection(pSrcColor[j].r, fBrightness, fPower, fA, fB);
      pDstColor[j].g = GetGammaCorrection(pSrcColor[j].g, fBrightness, fPower, fA, fB);
      pDstColor[j].b = GetGammaCorrection(pSrcColor[j].b, fBrightness, fPower, fA, fB);
    }
  }
  //
  return pDst;
}