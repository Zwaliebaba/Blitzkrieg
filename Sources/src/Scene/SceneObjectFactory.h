#ifndef __SCENEOBJECTFACTORY_H__
#define __SCENEOBJECTFACTORY_H__

#pragma once

#include "../Misc/BasicObjectFactory.h"

class CSceneObjectFactory : public CBasicObjectFactory
{
public:
  CSceneObjectFactory();
};

IObjectFactory * STDCALL GetSceneObjectFactory();

#endif // __SCENEOBJECTFACTORY_H__