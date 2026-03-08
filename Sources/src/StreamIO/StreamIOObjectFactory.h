#ifndef __STREAMIOOBJECTFACTORY_H__
#define __STREAMIOOBJECTFACTORY_H__

#include "../Misc/BasicObjectFactory.h"

class CStreamIOObjectFactory : public CBasicObjectFactory
{
public:
  CStreamIOObjectFactory();
};

IObjectFactory * STDCALL GetStreamIOObjectFactory();

#endif // __STREAMIOOBJECTFACTORY_H__