#ifndef __UNIT_STATES_H__
#define __UNIT_STATES_H__

#pragma once

#include "../Common/Actions.h"

enum EUnitStateNames
{
  // Do not change the numbering! 
  EUSN_ERROR = 0,
  EUSN_REST = 1,// resting
  EUSN_WAIT_FOR_PASSENGER = 2,// waiting for passenger to load
  EUSN_REST_ON_BOARD = 3,// located inside the unit
  EUSN_LAND = 4,// unloaded from the unit
  EUSN_ENTER_TRANSPORT = 5,// loaded into transport
  EUSN_ENTER = 6,// enters the building
  EUSN_ENTER_ENTRENCHMENT = 7,// enters the trench
  EUSN_REST_IN_BUILDING = 8,// located inside the building
  EUSN_REST_ENTRENCHMENT = 9,// is inside the trench
  EUSN_GO_OUT = 10,// leaves the building
  EUSN_SWARM = 11,// swarm to the point
  EUSN_GO_OUT_ENTRENCHMENT = 12,// running out of a trench
  EUSN_ATTACK_STAT_OBJECT = 13,// object attack
  EUSN_PARADE = 14,// line up in formation
  EUSN_AMBUSH = 15,// ambush
  EUSN_RANGING = 16,// shooting at a targeted area
  EUSN_BUILD_FENCE = 18,// fence construction in progress
  EUSN_BUILD_ENTRENCHMENT = 19,// trench construction
  EUSN_ATTACK_UNIT = 21,// unit attack
  EUSN_ATTACK_UNIT_IN_BUILDING = 22,// attacking a unit in a building
  EUSN_WAIT_TO_FORM = 23,// waiting to gather formation
  EUSN_BEING_TOWED = 24,// are being dragged
  EUSN_GUN_CREW_STATE = 25,// servicing the gun
  EUSN_DIVE_BOMBING = 26,// dive attack
  EUSN_PARTROOP = 27,// dropped from a parachute
  EUSN_FLY_DEAD = 28,// dead plane flies away
  EUSN_BUILD_LONGOBJECT = 29,// construction of a long object (formation)
  EUSN_REPAIR_BRIDGE = 30,
  EUSN_CLEAR_MINE = 31,// engineers are looking for mines
  EUSN_MOVE = 32,
  EUSN_USE_SPYGLASS = 33,
  EUSN_BOMBARDMANET = 34,// suppressive fire
  EUSN_REPAIR_BUILDING = 35,
  EUSN_REPAIR_UNIT = 36,
  EUSN_RESUPPLY_UNIT = 37,
  EUSN_HUMAN_RESUPPLY = 38,
  EUSN_BUILD_BRIDGE = 39,
  EUSN_PLACE_ANTITANK = 40,
  EUSN_PLACE_MINE = 41,
  EUSN_MOVE_TO_RESUPPLY_CELL = 42,// special state for general's transports
  EUSN_TURN_TO_POINT = 43,
  EUSN_USE = 44,
  EUSN_GUN_CAPTURE = 45,
  EUSN_MOVE_TO_GRID = 46,
  EUSN_HOOK_ARTILLERY = 47,
};

inline bool IsRestState(const EUnitStateNames eStateName)
{
  return
      eStateName == EUSN_REST || eStateName == EUSN_REST_ON_BOARD ||
      eStateName == EUSN_REST_ENTRENCHMENT || eStateName == EUSN_REST_IN_BUILDING;
}

inline bool IsRestCommand(const EActionCommand command)
{
  return
      command == ACTION_COMMAND_IDLE_BUILDING ||
      command == ACTION_COMMAND_IDLE_TRENCH ||
      command == ACTION_COMMAND_IDLE_TRANSPORT ||
      command == ACTION_COMMAND_GUARD;
}

enum ETryStateInterruptResult
{
  TSIR_YES_IMMIDIATELY,
  TSIR_YES_WAIT,
  TSIR_YES_MANAGED_ALREADY,
  TSIR_NO_COMMAND_INCOMPATIBLE,
};

interface IUnitState : IRefCount
{
  virtual void Segment() = 0;
  virtual EUnitStateNames GetName() { return EUSN_ERROR; }
  virtual ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) = 0;

  // state in which we purposefully run to kill someone
  virtual bool IsAttackingState() const = 0;
  // destination for state
  virtual const CVec2 GetPurposePoint() const = 0;
};

interface IUnitAttackingState : IUnitState
{
  virtual bool IsAttacksUnit() const = 0;
  virtual class CAIUnit *GetTargetUnit() const = 0;
};

#define CONVERT_OBJECT( CType, pObjTo, pObjFrom, msg )	\
	if ( pObjFrom == 0 ) break;														\
	CType *pObjTo = dynamic_cast<CType*>(pObjFrom);				\
	NI_ASSERT_T( pObjTo != 0, msg );											\
	if ( pObjTo == 0 ) break;

#define CONVERT_OBJECT_PTR( CType, pObjTo, pObjFrom, msg )		\
	if ( pObjFrom == 0 ) break;																	\
	CType *pObjTo = dynamic_cast_ptr<CType*>(pObjFrom);					\
	NI_ASSERT_T( pObjTo != 0, msg );														\
	if ( pObjTo == 0 ) break;

#endif // __UNIT_STATES_H__