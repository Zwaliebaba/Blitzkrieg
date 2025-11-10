#ifndef __MAPOBJVISITORS_H__
#define __MAPOBJVISITORS_H__

#pragma once

#include "MapObject.h"

struct SGetVisObjesVisitor : IMapObjVisitor
{
  struct SVisObjDesc
  {
    CPtr<IVisObj> pVisObj;
    EObjGameType eGameType;
    EObjVisType eVisType;
    bool bOutbound;
  };

  //
  std::list<SVisObjDesc> objects;
  //
  void Clear() { objects.clear(); }
  //
  void STDCALL VisitSprite(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) override
  {
    SVisObjDesc desc;
    desc.pVisObj = pVO;
    desc.eGameType = eGameType;
    desc.eVisType = eVisType;
    desc.bOutbound = bOutbound;
    objects.push_back(desc);
  }

  void STDCALL VisitMesh(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) override
  {
    SVisObjDesc desc;
    desc.pVisObj = pVO;
    desc.eGameType = eGameType;
    desc.eVisType = eVisType;
    desc.bOutbound = bOutbound;
    objects.push_back(desc);
  }

  void STDCALL VisitEffect(IVisObj *pVO, EObjGameType eGameType, EObjVisType eVisType, bool bOutbound = false) override
  {
    SVisObjDesc desc;
    desc.pVisObj = pVO;
    desc.eGameType = eGameType;
    desc.eVisType = eVisType;
    desc.bOutbound = bOutbound;
    objects.push_back(desc);
  }
};

#endif // __MAPOBJVISITORS_H__