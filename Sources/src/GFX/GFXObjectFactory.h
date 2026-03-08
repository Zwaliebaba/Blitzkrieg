#ifndef __GFXOBJECTFACTORY_H__
#define __GFXOBJECTFACTORY_H__

#pragma once

#include "../Misc/BasicObjectFactory.h"

class CGFXObjectFactory : public CBasicObjectFactory
{
public:
  CGFXObjectFactory();
};

extern CGFXObjectFactory theGFXObjectFactory;

class CGFXModuleChecker : public IModuleChecker
{
public:
  // check module functionality - return some kind of 'grade' for this module
  int STDCALL CheckFunctionality() const override;
  // set module functionality limits
  void STDCALL SetModuleFunctionalityLimits() const override;
};

#endif // __GFXOBJECTFACTORY_H__