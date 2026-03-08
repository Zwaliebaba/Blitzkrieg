#ifndef __ANIMOBJECTFACTORY_H__
#define __ANIMOBJECTFACTORY_H__
// //////////////////////////////////////////////////////////// 
#include "../Misc/BasicObjectFactory.h"

// //////////////////////////////////////////////////////////// 
class CAnimObjectFactory : public CBasicObjectFactory
{
public:
  CAnimObjectFactory();
};

IObjectFactory * STDCALL GetAnimObjectFactory();

// //////////////////////////////////////////////////////////// 
#endif // __ANIMOBJECTFACTORY_H__