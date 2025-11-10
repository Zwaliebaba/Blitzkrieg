#ifndef __IN_TRANSPORT_STATES_H__
#define __IN_TRANSPORT_STATES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "UnitStates.h"
#include "StatesFactory.h"

// //////////////////////////////////////////////////////////// 
class CInTransportStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CInTransportStatesFactory);

  static CPtr<CInTransportStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();
  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;
  bool CanCommandBeExecuted(class CAICommand *pCommand) override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CSoldierRestOnBoardState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierRestOnBoardState);
  DECLARE_SERIALIZE;

  class CSoldier *pSoldier;

public:
  static IUnitState *Instance(class CSoldier *pSoldier, class CMilitaryCar *pTransport);

  CSoldierRestOnBoardState() : pSoldier(nullptr) {}
  CSoldierRestOnBoardState(class CSoldier *pSoldier, class CMilitaryCar *pTransport);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_ON_BOARD; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
#endif // __IN_TRANSPORT_STATES_H__