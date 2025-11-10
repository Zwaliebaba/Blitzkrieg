#ifndef __ANIM_UNIT_MECH_H__
#define __ANIM_UNIT_MECH_H__

#pragma once

#include "AnimUnit.h"

class CAIUnit;

class CAnimUnitMech : public IAnimUnit
{
  OBJECT_COMPLETE_METHODS(CAnimUnitMech);
  DECLARE_SERIALIZE;

  CAIUnit *pOwner;

  struct SMovingState
  {
    DECLARE_SERIALIZE;

  public:
    enum EMovingState { EMS_STOPPED, EMS_MOVING, EMS_STOPPED_TO_MOVING, EMS_MOVING_TO_STOPPED };

    EMovingState state;
    NTimer::STime timeOfIntentionStart;

    SMovingState() : state(EMS_STOPPED), timeOfIntentionStart(0) {}
  };

  SMovingState movingState;

public:
  CAnimUnitMech() : pOwner(nullptr) {}
  void Init(class CAIUnit *pOwner) override;

  void AnimationSet(int nAnimation) override;

  void Moved() override;
  void Stopped() override;

  void Segment() override;

  void StopCurAnimation() override {}
};

#endif // __ANIM_UNIT_MECH_H__