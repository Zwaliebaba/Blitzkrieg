#ifndef __COLLISION_INTERNAL_H__
#define __COLLISION_INTERNAL_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "Collision.h"
// //////////////////////////////////////////////////////////// 
class CPathUnit;
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CollisionsCollector*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
struct SUnitsPair
{
  DECLARE_SERIALIZE;

public:
  CPtr<CPathUnit> pUnit1;
  CPtr<CPathUnit> pUnit2;
  int nCollideType;

  SUnitsPair() {}

  SUnitsPair(class CPathUnit *_pUnit1, class CPathUnit *_pUnit2, const int _nCollideType)
    : pUnit1(_pUnit1), pUnit2(_pUnit2), nCollideType(_nCollideType) {}

  friend bool operator <(const SUnitsPair &pair1, const SUnitsPair &pair2);
};

// //////////////////////////////////////////////////////////// 
bool operator <(const SUnitsPair &pair1, const SUnitsPair &pair2);

// //////////////////////////////////////////////////////////// 
class CCollisionsCollector : public IRefCount
{
  OBJECT_NORMAL_METHODS(CCollisionsCollector);
  DECLARE_SERIALIZE;

  std::priority_queue<SUnitsPair> collisions;

public:
  void AddCollision(class CPathUnit *pUnit1, class CPathUnit *pUnit2, int nCollideType);
  void HandOutCollisions();
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// * Collisions for units and regular ground vehicles *
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CCollision : public ICollision
{
  DECLARE_SERIALIZE;

protected:
  class CPathUnit *pUnit;
  CPtr<CAIUnit> pPushUnit;
  int nPriority;

public:
  CCollision() : pUnit(nullptr) {}
  CCollision(class CPathUnit *pUnit, class CAIUnit *pPushUnit, int nPriority);

  bool IsSolved() override;

  // returns whether the unit should be stopped
  int FindCandidates() override;

  int GetPriority() const override { return nPriority; }

  class CAIUnit *GetPushUnit() const override { return pPushUnit; }
};

// //////////////////////////////////////////////////////////// 
class CFreeOfCollisions : public CCollision
{
  OBJECT_COMPLETE_METHODS(CFreeOfCollisions);
  DECLARE_SERIALIZE;

public:
  CFreeOfCollisions() {}
  explicit CFreeOfCollisions(class CPathUnit *pUnit, class CAIUnit *pPushUnit);

  bool IsSolved() override;
  interface IPath *GetPath() const override { return nullptr; }

  void Segment() override {}

  ECollisionName GetName() const override { return ECN_FREE; }
};

// //////////////////////////////////////////////////////////// 
class CGivingPlaceCollision : public CCollision
{
  OBJECT_COMPLETE_METHODS(CGivingPlaceCollision);
  DECLARE_SERIALIZE;

  CVec2 vDir;
  CVec2 finishPoint;
  NTimer::STime timeToFinish;

  bool IsPathSolved();

public:
  CGivingPlaceCollision() {}
  CGivingPlaceCollision(class CPathUnit *pUnit, class CAIUnit *pPushUnit, const CVec2 &vDir, float fDist, int nPriority);

  bool IsSolved() override;
  interface IPath *GetPath() const override;

  void Segment() override;

  ECollisionName GetName() const override { return ECN_GIVE_PLACE; }
};

// //////////////////////////////////////////////////////////// 
class CGivingPlaceRotateCollision : public CCollision
{
  OBJECT_COMPLETE_METHODS(CGivingPlaceRotateCollision);
  DECLARE_SERIALIZE;

  WORD wDir;
  bool bTurned;

public:
  CGivingPlaceRotateCollision() {}
  CGivingPlaceRotateCollision(class CPathUnit *pUnit, class CAIUnit *pPushUnit, const CVec2 &vDir, int nPriority);

  bool IsSolved() override { return bTurned || CCollision::IsSolved(); }
  interface IPath *GetPath() const override { return nullptr; }

  void Segment() override;

  ECollisionName GetName() const override { return ECN_GIVE_PLACE_ROTATE; }
};

// //////////////////////////////////////////////////////////// 
class CWaitingCollision : public CCollision
{
  OBJECT_NORMAL_METHODS(CWaitingCollision);
  DECLARE_SERIALIZE;

  NTimer::STime finishTime;
  bool bLock;

public:
  CWaitingCollision() {}
  CWaitingCollision(class CPathUnit *pUnit, class CAIUnit *pPushUnit, bool bLock);
  virtual ~CWaitingCollision();

  bool IsSolved() override;
  interface IPath *GetPath() const override { return nullptr; }

  void Segment() override {}

  ECollisionName GetName() const override { return ECN_WAIT; }

  void Finish();
};

// //////////////////////////////////////////////////////////// 
class CStopCollision : public CCollision
{
  OBJECT_NORMAL_METHODS(CStopCollision);
  DECLARE_SERIALIZE;

  NTimer::STime finishTime;

public:
  CStopCollision() {}
  CStopCollision(class CPathUnit *pUnit);

  bool IsSolved() override;
  interface IPath *GetPath() const override { return nullptr; }

  void Segment() override {}
  ECollisionName GetName() const override { return ECN_STOP; }
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// * Collisions for airplanes *
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CPlaneCollision : public ICollision
{
  OBJECT_COMPLETE_METHODS(CPlaneCollision);
  DECLARE_SERIALIZE;

public:
  int FindCandidates() override { return 0; }

  bool IsSolved() override { return false; }
  int GetPriority() const override { return 0; }

  interface IPath *GetPath() const override { return nullptr; }
  class CAIUnit *GetPushUnit() const override { return nullptr; }

  void Segment() override {}

  ECollisionName GetName() const override { return ECN_FREE; }
};

// //////////////////////////////////////////////////////////// 
#endif // __COLLISION_INTERNAL_H__