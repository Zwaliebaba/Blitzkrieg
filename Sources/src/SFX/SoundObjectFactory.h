#ifndef __SOUNDOBJECTFACTORY_H__
#define __SOUNDOBJECTFACTORY_H__

#include "../Misc/BasicObjectFactory.h"

class CSoundObjectFactory : public CBasicObjectFactory
{
public:
  CSoundObjectFactory();
};

IObjectFactory * STDCALL GetSFXObjectFactory();

#endif // __SOUNDOBJECTFACTORY_H__