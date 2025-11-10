#ifndef __PLANE_PATH_H__
#define __PLANE_PATH_H__

#pragma once

#include "Path.h"

// **********************************************************************
// *CPlanePath*
// **********************************************************************

class CPlanePath : public IPath
{
  OBJECT_COMPLETE_METHODS(CPlanePath);
  DECLARE_SERIALIZE;

  CVec2 startPoint, finishPoint;
  float fStartZ;
  float fFinishZ;

public:
  CPlanePath() {}

  CPlanePath(const CVec3 &_startPoint, const CVec2 &_finishPoint)
    : startPoint(_startPoint.x, _startPoint.y), finishPoint(_finishPoint.x, _finishPoint.y),
      fStartZ(_startPoint.z), fFinishZ(_startPoint.z) {}

  CPlanePath(const CVec3 &_startPoint, const CVec3 &_finishPoint)
    : startPoint(_startPoint.x, _startPoint.y), finishPoint(_finishPoint.x, _finishPoint.y),
      fStartZ(_startPoint.z), fFinishZ(_finishPoint.z) {}

  bool IsFinished() const override { return false; }

  const CVec2 PeekPoint(int nShift) override { return VNULL2; }
  void Shift(int nShift) override {}

  const CVec2 &GetFinishPoint() const override { return finishPoint; }
  const CVec2 &GetStartPoint() const override { return startPoint; }

  virtual void SetFinishZ(const float _fFinishZ) { fFinishZ = _fFinishZ; }
  virtual void SetStartPoint(const CVec2 &_startPoint) { startPoint = _startPoint; }
  virtual float GetFinishZ() const { return fFinishZ; }
  virtual float GetStartZ() const { return fStartZ; }

  void RecoverState(const CVec2 &point, const SVector &lastKnownGoodTile) override { startPoint = point; }
  void Recalculate(const CVec2 &point, const SVector &lastKnownGoodTile) override {}

  bool CanGoBackward(interface IBasePathUnit *pUnit) override { return false; }
  bool ShouldCheckTurn() const override { return false; }
  virtual bool IsWithFormation() const { return false; }
};

class CBasePlaneSmoothPath : public ISmoothPath
{
public:
  virtual void SetAviationUnit(IAviationUnit *_pPlane) {}
};

// **********************************************************************
// *CPlaneSmoothPath*
// **********************************************************************

class CPlaneSmoothPath : public CBasePlaneSmoothPath
{
  OBJECT_COMPLETE_METHODS(CPlaneSmoothPath);
  DECLARE_SERIALIZE;

public:
  struct SMemberInfo
  {
    DECLARE_SERIALIZE;

  public:
    CVec2 vWorldPosition;
    WORD wDirection;
    float fCurvatureRadius;
    CVec2 vSpeed;

    NTimer::STime lastMoveTime;
    SMemberInfo() : lastMoveTime(-1) {}
  };


  struct SPathVisitor
  {
    WORD wCurAngle;
    CVec2 vCurPoint;
    SPathVisitor(const CVec2 &vPoint) : vCurPoint(vPoint) {}
    SPathVisitor(const WORD wAngle) : wCurAngle(wAngle) {}
  };

  //
  class CPathFraction : public IRefCount
  {
    DECLARE_SERIALIZE;
    bool bActive;

  public:
    CPathFraction() : bActive(true) {}
    // return true if the formation offset corresponds to this path fraction; 
    // world coordinates
    // offset - when it is not first fraction.
    virtual bool CalcPoint(const CVec2 &vFormationOffset/* path coordinates */, CVec2 *vPosition /* world coordinates */, WORD *wUnitDir, float *pfCurvatureRadius, CVec2 *pvSpeed, float fOffset) const = 0;
    // length in world coordinates,
    virtual float GetLength() const = 0;
    // if this fraction is being moved by
    bool IsCurrent() const;
    // sets current position
    virtual void SetCurPos(const SPathVisitor &rVisitor) = 0;
    // mark path as finished
    virtual void SetFinished(const SPathVisitor &rVisitor) = 0;
    // last segment did'n use this fraction
    void SetActive(const bool _bActive) { bActive = _bActive; }
    bool IsActive() const { return bActive; }
  };

  // arc path fraction, from angle to angle,
  class CArcPathFraction : public CPathFraction
  {
    OBJECT_COMPLETE_METHODS(CArcPathFraction);
    DECLARE_SERIALIZE;

    CCircle flyCircle;
    WORD wFrom;
    int nAngleSingn;
    float fLenght;

    WORD wCurAngle;
    void CalcLenght();

  public:
    CArcPathFraction() {}
    // angles - from center
    CArcPathFraction(const CCircle &_flyCircle, WORD wDirFrom, int _nAngleSingn);

    bool CalcPoint(const CVec2 &vFormationOffset/* path coordinates */, CVec2 *vPosition /* world coordinates */, WORD *wUnitDir, float *pfCurvatureRadius, CVec2 *pvSpeed, float fOffset) const override;
    float GetLength() const override { return fLenght; }

    void SetCurPos(const SPathVisitor &rVisitor) override;
    void SetFinished(const SPathVisitor &rVisitor) override;
  };

  // line path fraction, from point to point
  class CLinePathFraction : public CPathFraction
  {
    OBJECT_COMPLETE_METHODS(CLinePathFraction);
    DECLARE_SERIALIZE;
    CVec2 vStart;
    CVec2 vCurPoint;

    float fLength;

    void CalcLenght() { fLength = fabs(vCurPoint - vStart); }

  public:
    CLinePathFraction() {}

    CLinePathFraction(const CVec2 &_vStart)
      : vStart(_vStart), vCurPoint(_vStart), fLength(0) {}

    bool CalcPoint(const CVec2 &vFormationOffset/* path coordinates */, CVec2 *vPosition /* world coordinates */, WORD *wUnitDir, float *pfCurvatureRadius, CVec2 *pvSpeed, float fOffset) const override;
    float GetLength() const override { return fLength; }

    void SetCurPos(const SPathVisitor &rVisitor) override
    {
      vCurPoint = rVisitor.vCurPoint;
      CalcLenght();
    }

    void SetFinished(const SPathVisitor &rVisitor) override
    {
      vCurPoint = rVisitor.vCurPoint;
      CalcLenght();
    }
  };

private:
  bool bTrackHistory;// true if path history is ti be recorded.
  using CPathHistory = std::list<CPtr<CPathFraction>>;
  CPathHistory pathHistory;

  enum EHeightState
  {
    HS_HEIGHT_OK,// horizontal flight

    HS_GAIN_STRAIGHT,// gain height with constant angle
    HS_GAIN_ESCAPE,// finishing of gain height
    HS_GAIN_ENTER,// starting of gain height

    HS_DIVE_STRAIGHT,// same...
    HS_DIVE_ENTER,
    HS_DIVE_ESCAPE,
  };

  EHeightState eState;
  NTimer::STime segmentTime;

  CPtr<CPlanePath> pPath;
  interface IBasePathUnit *pUnit;
  interface IAviationUnit *pPlane;

  float fTurnRadiusMax;
  float fTurnRadiusMin;

  float fTurnR;
  float fSpeed;
  float fAngleSpeed;
  CVec2 vAngleSpeed;
  CVec2 vCurAngleSpeed;
  bool bGainHeight;// height change period
  bool bToHorisontal;// come out of a dive/pitch
  float fDistanceToChangeHeight;// distance to start altitude change

  CCircle flyCircle;
  CVec2 circePoint;
  short int angleSign;
  WORD startAngle, finishAngle;
  CVec2 dirByLine;

  bool bFinished;
  bool bByCircle;

  bool bSmoothTurn;// if true means the plane is not in combat mode
  float fVerTurnRatio;

  //
  void CompareWithBest(const CVec2 &p, CVec2 *bestPoint, WORD *wBestAngle, const CCircle &circle, short int sign);
  // new turning radius
  void SetTurnRadius(float fTurnRadius);
  float Calc2DDistanceToGo() const;// the distance remaining to the end point (as projected onto the horizontal plane)
  static bool IsHeightOK(const IBasePathUnit *pUnit, const IAviationUnit *pPlane, float fZ, float fAngleSpeed);
  void FinishPath();

public:
  bool IsInitialized() const { return pPath && pUnit && pPlane; }
  static bool IsHeightOK(const IBasePathUnit *pUnit, float fZ, float fAngleSpeed = 0.0f);
  static float CalcCriticalDistance(const CVec2 &vSpeedHorVer, float _fVerTurnRatio, float _fTurnRadius);

  virtual void SetAviationUnit(IAviationUnit *_pPlane, IBasePathUnit *_pPathUnit);

  CPlaneSmoothPath() : bTrackHistory(false), pUnit(nullptr), pPlane(nullptr) {}
  CPlaneSmoothPath(float fTurnRadiusMin, float fTurnRadiusMax, float fSpeed, float fVerTurnRatio, bool _bTrackHistory = false);

  bool Init(interface IBasePathUnit *pPathUnit, interface IAviationUnit *pAviationUnit, IPath *pPath, bool bSmoothTurn, bool bCheckTurn = true) override;
  bool Init(interface IBasePathUnit *pUnit, interface IPath *pPath, bool bSmoothTurn = true, bool bCheckTurn = true) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override;
  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override;

  const CVec2 &GetFinishPoint() const override { return pPath->GetFinishPoint(); }

  bool IsFinished() const override;

  void Stop() override { bFinished = true; }

  const CVec3 GetPoint(NTimer::STime timeDiff) override;
  float &GetSpeedLen() override { return fSpeed; }

  void NotifyAboutClosestThreat(interface IBasePathUnit *pUnit, const float fDist) override {}
  void SlowDown() override {}

  //
  virtual bool TurnToDir(const WORD &newDir) { return true; }

  bool CanGoBackward() const override { return false; }
  bool CanGoForward() const override { return true; }
  void GetNextTiles(std::list<SVector> *pTiles) override {}

  CVec2 GetShift(const int nToShift) const override
  {
    NI_ASSERT_T(false, "Wrong call");
    return VNULL2;
  }

  IMemento *GetMemento() const override;
  float GetCurvatureRadius() const override;
  CVec2 GetCurvatureCenter() const override;

  bool IsWithFormation() const override { return false; }
  void SetOwner(interface IBasePathUnit *pUnit) override;
  IBasePathUnit *GetOwner() const override;

  // calsulates world coordinates form formation coordinates
  void CalculateMemberInfo(const CVec2 &vFormationOffset, SMemberInfo *pMemberInfo) const;
  void ClearUnisedHistory();
};

class CPlanesFormation;

// path for planes, that formation with horizontal shift.
class CPlaneInFormationSmoothPath : public CBasePlaneSmoothPath
{
  OBJECT_COMPLETE_METHODS(CPlaneInFormationSmoothPath);
  DECLARE_SERIALIZE;

  CPlanesFormation *pFormation;
  class CAviation *pOwner;

public:
  CPlaneInFormationSmoothPath() : pOwner(nullptr) {}
  void Init(class CAviation *_pOwner);

  // forward to formation path
  const CVec2 &GetFinishPoint() const override;
  bool IsFinished() const override;
  const CVec3 GetPoint(NTimer::STime timeDiff) override;
  float &GetSpeedLen() override;
  float GetCurvatureRadius() const override;
  CVec2 GetCurvatureCenter() const override;


  // need to correct work of CBasePathUnit
  bool CanGoBackward() const override;
  void SetOwner(interface IBasePathUnit *pUnit) override;
  IBasePathUnit *GetOwner() const override;

  // empty functions
  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn, bool bCheckTurn = true) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override
  {
    NI_ASSERT_T(false, "wrong call");
    return false;
  }

  void NotifyAboutClosestThreat(interface IBasePathUnit *pCollUnit, const float fDist) override { NI_ASSERT_T(false, "wrong call"); }
  void SlowDown() override { NI_ASSERT_T(false, "wrong call"); }

  bool CanGoForward() const override
  {
    NI_ASSERT_T(false, "wrong call");
    return true;
  }

  void GetNextTiles(std::list<SVector> *pTiles) override { NI_ASSERT_T(false, "wrong call"); }

  CVec2 GetShift(const int nToShift) const override
  {
    NI_ASSERT_T(false, "wrong call");
    return VNULL2;
  }

  IMemento *GetMemento() const override
  {
    NI_ASSERT_T(false, "wrong call");
    return nullptr;
  }

  bool IsWithFormation() const override
  {
    NI_ASSERT_T(false, "wrong call");
    return true;
  }

  void Stop() override { NI_ASSERT_T(false, "wrong call"); }
};

#endif // __PLANE_PATH_H__