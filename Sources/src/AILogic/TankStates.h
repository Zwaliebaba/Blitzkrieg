#ifndef __TANK_STATES_H__
#define __TANK_STATES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "StatesFactory.h"
#include "RectTiles.h"
#include "UnitStates.h"
#include "CLockWithUnlockPossibilities.h"

// //////////////////////////////////////////////////////////// 
class CTankStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CTankStatesFactory);

  static CPtr<CTankStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();
  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
#endif // __TANK_STATES_H__