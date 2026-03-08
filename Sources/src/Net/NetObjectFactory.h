#ifndef __NETOBJECTFACTORY_H__
#define __NETOBJECTFACTORY_H__

#pragma once

#include "../Misc/BasicObjectFactory.h"

class CNetObjectFactory : public CBasicObjectFactory
{
public:
  CNetObjectFactory();
};

IObjectFactory * STDCALL GetNetObjectFactory();

#endif // __NETOBJECTFACTORY_H__