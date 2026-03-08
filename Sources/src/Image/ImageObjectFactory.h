#ifndef __IMAGEOBJECTFACTORY_H__
#define __IMAGEOBJECTFACTORY_H__

#include "../Misc/BasicObjectFactory.h"

class CImageObjectFactory : public CBasicObjectFactory
{
public:
  CImageObjectFactory();
};

IObjectFactory * STDCALL GetImageObjectFactory();

#endif // __IMAGEOBJECTFACTORY_H__