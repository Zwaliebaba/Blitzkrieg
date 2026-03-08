#ifndef __MISSIONOBJECTFACTORY_H__
#define __MISSIONOBJECTFACTORY_H__

#pragma once

#include "../Misc/BasicObjectFactory.h"

class CMissionObjectFactory : public CBasicObjectFactory
{
public:
  CMissionObjectFactory();
};

IObjectFactory * STDCALL GetMissionObjectFactory();

#endif // __MISSIONOBJECTFACTORY_H__