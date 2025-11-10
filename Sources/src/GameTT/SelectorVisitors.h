#ifndef __SELECTORVISITORS_H__
#define __SELECTORVISITORS_H__

#pragma once

#include "../Common/MapObject.h"

class CGetActionsSelectiorVisitor : public ISelectorVisitor
{
  const IMapObj::EActionsType eActions;
  CUserActions *pActions;

public:
  CGetActionsSelectiorVisitor(const IMapObj::EActionsType _eActions, CUserActions *_pActions)
    : eActions(_eActions), pActions(_pActions) {}

  //
  void STDCALL VisitMapObject(struct SMapObject *pMO) const override
  {
    CUserActions actions;
    pMO->GetActions(&actions, eActions);
    *pActions |= actions;
  }
};

class CGetActionsExceptSelectiorVisitor : public ISelectorVisitor
{
  const IMapObj::EActionsType eActions;
  CUserActions *pActions;
  const IMapObj *pException;

public:
  CGetActionsExceptSelectiorVisitor(const IMapObj::EActionsType _eActions, CUserActions *_pActions, const IMapObj *_pException)
    : eActions(_eActions), pActions(_pActions), pException(_pException) {}

  //
  void STDCALL VisitMapObject(struct SMapObject *pMO) const override
  {
    if (pMO != pException)
    {
      CUserActions actions;
      pMO->GetActions(pActions, eActions);
      *pActions |= actions;
    }
  }
};

class CCollectObjectsSelectiorVisitor : public ISelectorVisitor
{
  mutable CMapObjectsList objects;

public:
  void STDCALL VisitMapObject(struct SMapObject *pMO) const override { objects.push_back(pMO); }
  //
  CMapObjectsList &GetObjects() { return objects; }
};

#endif // __SELECTORVISITORS_H__