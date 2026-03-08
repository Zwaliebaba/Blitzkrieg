#ifndef __UIOBJECTFACTORY_H__
#define __UIOBJECTFACTORY_H__

#include "../Misc/BasicObjectFactory.h"

class CUIObjectFactory : public CBasicObjectFactory
{
public:
  CUIObjectFactory();
};

IObjectFactory * STDCALL GetUIObjectFactory();

#endif // __UIOBJECTFACTORY_H__