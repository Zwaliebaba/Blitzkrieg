#include "pch.h"
#include "ImagePNG.h"
#include <png.h>

enum EBMMTypes
{
  BMM_NO_TYPE,
  BMM_PALETTED,
  BMM_TRUE_32,
  BMM_GRAY_8
};

bool NImage::RecognizeFormatPNG(IDataStream *pStream)
{
  BYTE signature[8];
  int nCounter = pStream->Read(signature, 8);
  pStream->Seek(-nCounter, STREAM_SEEK_CUR);
  if (nCounter != 8) return false;
  return png_sig_cmp(signature, 0, 8) == 0;
}

void PNGReadFunction(png_structp png_ptr, png_bytep data, png_size_t length)
{
  IDataStream *pStream = reinterpret_cast<IDataStream *>(png_get_io_ptr(png_ptr));
  int check = pStream->Read(data, length);
  if (check != length) png_error(png_ptr, "Read Error");
}

void PNGWriteFunction(png_structp png_ptr, png_bytep data, png_size_t length)
{
  IDataStream *pStream = reinterpret_cast<IDataStream *>(png_get_io_ptr(png_ptr));
  int check = pStream->Write(data, length);
  if (check != length) png_error(png_ptr, "Write Error");
}

void PNGFlushFunction(png_structp png_ptr) {}

CImage *NImage::LoadImagePNG(IDataStream *pStream)
{
  png_structp png = nullptr;
  png_infop info = nullptr;
  png_bytep *row_pointers = nullptr;
  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0;
  png_size_t rowbytes = 0;
  int channels = 0;

  //
  png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png == nullptr) return nullptr;
  //
  info = png_create_info_struct(png);
  if (info == nullptr)
  {
    png_destroy_read_struct(&png, nullptr, nullptr);
    return nullptr;
  }

  if (setjmp(png_jmpbuf(png)))
  {
    if (row_pointers)
    {
      for (png_uint_32 i = 0; i < height; i++)
      {
        if (row_pointers[i]) free(row_pointers[i]);
      }
      free(row_pointers);
    }
    png_destroy_read_struct(&png, &info, nullptr);
    return nullptr;
  }

  png_set_read_fn(png, pStream, PNGReadFunction);
  png_read_info(png, info);

  // Get image info using accessor functions
  width = png_get_image_width(png, info);
  height = png_get_image_height(png, info);
  bit_depth = png_get_bit_depth(png, info);
  color_type = png_get_color_type(png, info);
  interlace_type = png_get_interlace_type(png, info);

  std::vector<DWORD> image(width * height);

  // expand grayscale images to the full 8 bits
  // expand images with transparency to full alpha channels
  if ((color_type == PNG_COLOR_TYPE_PALETTE && bit_depth < 8) ||
      (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) ||
      png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_expand(png);

  int nNumPasses = 1;
  if (interlace_type) nNumPasses = png_set_interlace_handling(png);

  if (bit_depth == 16) png_set_swap(png);

  png_read_update_info(png, info);

  // Re-read info after update
  bit_depth = png_get_bit_depth(png, info);
  color_type = png_get_color_type(png, info);
  rowbytes = png_get_rowbytes(png, info);
  channels = png_get_channels(png, info);

  // determine type
  int bmtype = BMM_NO_TYPE;
  if (bit_depth != 1)
  {
    switch (color_type)
    {
      case PNG_COLOR_TYPE_PALETTE:
        bmtype = BMM_PALETTED;
        break;
      case PNG_COLOR_TYPE_RGB:
      case PNG_COLOR_TYPE_RGB_ALPHA:
        switch (bit_depth)
        {
          case 2:
          case 4:
          case 16:
            // Not allowed
            break;
          case 8:
            bmtype = BMM_TRUE_32;
            break;
        }
        break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
      case PNG_COLOR_TYPE_GRAY:
        switch (bit_depth)
        {
          case 2:
          case 4:
          case 16:
            break;
          case 8:
            bmtype = BMM_GRAY_8;
            break;
        }
        break;
    }
  }
  if (bmtype == BMM_NO_TYPE)
  {
    png_destroy_read_struct(&png, &info, nullptr);
    return nullptr;
  }

  row_pointers = static_cast<png_bytep *>(malloc(height * sizeof(png_bytep)));
  for (png_uint_32 i = 0; i < height; i++)
    row_pointers[i] = static_cast<png_bytep>(malloc(rowbytes));

  png_read_image(png, row_pointers);

  // decompress image to the ARGB format
  switch (bmtype)
  {
    case BMM_PALETTED:
    {
      if (bit_depth == 8)
      {
        png_colorp palette;
        int num_palette;
        png_get_PLTE(png, info, &palette, &num_palette);

        for (png_uint_32 iy = 0; iy < height; iy++)
        {
          for (png_uint_32 ix = 0; ix < width; ix++)
          {
            int idx = row_pointers[iy][ix];
            DWORD dwColor = 0xFF000000 |
                            (static_cast<DWORD>(palette[idx].red) << 16) |
                            (static_cast<DWORD>(palette[idx].green) << 8) |
                            static_cast<DWORD>(palette[idx].blue);
            image[iy * width + ix] = dwColor;
          }
        }
      }
    }
    break;
    case BMM_TRUE_32:
    {
      DWORD r, g, b, a;
      for (png_uint_32 iy = 0; iy < height; iy++)
      {
        for (png_uint_32 ix = 0; ix < rowbytes;)
        {
          r = row_pointers[iy][ix++];
          g = row_pointers[iy][ix++];
          b = row_pointers[iy][ix++];
          a = (channels == 4 ? row_pointers[iy][ix++] : 255);
          image[iy * width + (ix / channels - 1)] = (a << 24) | (r << 16) | (g << 8) | b;
        }
      }
    }
    break;
    case BMM_GRAY_8:
    {
      DWORD color, alpha;
      for (png_uint_32 iy = 0; iy < height; iy++)
      {
        for (png_uint_32 ix = 0; ix < rowbytes;)
        {
          color = row_pointers[iy][ix++];
          alpha = channels == 2 ? row_pointers[iy][ix++] : 255;
          image[iy * width + (ix / channels - 1)] = (alpha << 24) | (color << 16) | (color << 8) | color;
        }
      }
    }
    break;
  }

  png_read_end(png, info);

  for (png_uint_32 i = 0; i < height; i++) free(row_pointers[i]);
  free(row_pointers);
  png_destroy_read_struct(&png, &info, nullptr);

  return new CImage(width, height, image);
}

bool NImage::SaveImageAsPNG(IDataStream *pStream, const IImage *pImage)
{
  png_structp png = nullptr;
  png_infop info = nullptr;
  png_bytep *row_pointers = nullptr;

  png_uint_32 width = pImage->GetSizeX();
  png_uint_32 height = pImage->GetSizeY();
  int bit_depth = 8;
  int color_type = PNG_COLOR_TYPE_RGB_ALPHA;
  int channels = 4;
  png_size_t rowbytes = width * channels;

  png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png == nullptr) return false;

  info = png_create_info_struct(png);
  if (info == nullptr)
  {
    png_destroy_write_struct(&png, nullptr);
    return false;
  }

  if (setjmp(png_jmpbuf(png)))
  {
    if (row_pointers)
    {
      for (png_uint_32 i = 0; i < height; i++)
        if (row_pointers[i]) free(row_pointers[i]);
      free(row_pointers);
    }
    png_destroy_write_struct(&png, &info);
    return false;
  }

  png_set_write_fn(png, pStream, PNGWriteFunction, PNGFlushFunction);

  // Set image attributes using png_set_IHDR
  png_set_IHDR(png, info, width, height, bit_depth, color_type,
               PNG_INTERLACE_ADAM7, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  row_pointers = static_cast<png_bytep *>(malloc(height * sizeof(png_bytep)));
  for (png_uint_32 i = 0; i < height; i++)
    row_pointers[i] = static_cast<png_bytep>(malloc(rowbytes));

  // store data in the PNG structure
  const SColor *pColors = pImage->GetLFB();
  for (png_uint_32 iy = 0; iy < height; ++iy)
  {
    for (png_uint_32 ix = 0; ix < rowbytes;)
    {
      SColor color = pColors[iy * width + ix / 4];
      row_pointers[iy][ix++] = color.r;
      row_pointers[iy][ix++] = color.g;
      row_pointers[iy][ix++] = color.b;
      row_pointers[iy][ix++] = color.a;
    }
  }

  png_write_info(png, info);
  png_set_swap(png);
  png_write_image(png, row_pointers);
  png_write_end(png, info);

  for (png_uint_32 i = 0; i < height; ++i) free(row_pointers[i]);
  free(row_pointers);
  png_destroy_write_struct(&png, &info);

  return true;
}