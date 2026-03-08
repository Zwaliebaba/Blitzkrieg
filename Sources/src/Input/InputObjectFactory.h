#ifndef __INPUTOBJECTFACTORY_H__
#define __INPUTOBJECTFACTORY_H__

#include "../Misc/BasicObjectFactory.h"

class CInputObjectFactory : public CBasicObjectFactory
{
public:
  CInputObjectFactory();
};

IObjectFactory * STDCALL GetInputObjectFactory();

#endif // __INPUTOBJECTFACTORY_H__