#ifndef __ART_ROCKET_STATES_H__
#define __ART_ROCKET_STATES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "StatesFactory.h"
#include "UnitStates.h"

// //////////////////////////////////////////////////////////// 
class CArtRocketStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CArtRocketStatesFactory);

  static CPtr<CArtRocketStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();

  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CArtRocketAttackGroundState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CArtRocketAttackGroundState);
  DECLARE_SERIALIZE;

  enum EAttackGroundState { EAGS_ROTATING, EAGS_FIRING };

  EAttackGroundState eState;

  class CArtillery *pArtillery;

  CVec2 point;
  bool bFired;
  bool bFinished;
  WORD wDirToRotate;

public:
  static IUnitState *Instance(class CArtillery *pArtillery, const CVec2 &point);

  CArtRocketAttackGroundState() : pArtillery(nullptr) {}
  CArtRocketAttackGroundState(class CArtillery *pArtillery, const CVec2 &point);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override { return point; }

  bool IsAttacksUnit() const override { return false; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
};

// //////////////////////////////////////////////////////////// 
#endif // __ART_ROCKET_STATES_H__