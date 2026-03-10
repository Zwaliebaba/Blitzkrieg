#include "pch.h"

#include "ImageObjectFactory.h"

#include "ImageReal.h"
#include "ImageMMP.h"
#include "ImageProcessor.h"

CImageObjectFactory theImageObjectFactory;
CImageObjectFactory::CImageObjectFactory() { REGISTER_CLASS(this, IMAGE_PROCESSOR, CImageProcessor); }

// ************************************************************************************************************************ //
// **
// ** module descriptor and additional procedures
// **
// **
// **
// **
// ************************************************************************************************************************ //

IObjectFactory * STDCALL GetImageObjectFactory() { return &theImageObjectFactory; }