#ifndef _ENTRENCHMENT_CREATION_INTERNAL_
#define _ENTRENCHMENT_CREATION_INTERNAL_

#pragma once
#include "RectTiles.h"

class CEntrenchmentPart;
class CCommonStaticObject;
class CEntrenchment;
class CAIUnit;
class CFence;

class CLongObjectCreation : public IRefCount
{
  DECLARE_SERIALIZE;
  float fWorkAccumulated;

public:
  CLongObjectCreation() : fWorkAccumulated(0.0f) {}
  virtual bool PreCreate(const CVec2 &vFrom, const CVec2 &vTo) = 0;

  // maximum trench size
  virtual const int GetMaxIndex() const = 0;

  // current state of construction
  virtual const int GetCurIndex() const = 0;

  // the point where the builders should stand
  virtual const CVec2 GetNextPoint(int nPlace, int nMaxPlace) const = 0;

  // places the next segment and moves the terminator
  // for the first segment sets the initial terminator
  virtual void BuildNext() { fWorkAccumulated = 0.0f; }

  // finds units that allow for further construction
  virtual void GetUnitsPreventing(std::list<CPtr<CAIUnit>> *units) = 0;

  // is there at least 1 unit that is interfering?
  virtual bool IsAnyUnitPrevent() const = 0;

  // can the next segment be built (excluding units)
  virtual bool CanBuildNext() const = 0;

  // so as not to check every time
  virtual void LockCannotBuild() = 0;

  // locks it under the segment so that no one stands on top
  virtual void LockNext() = 0;

  // line from which to remove units
  virtual CLine2 GetCurLine() = 0;

  virtual float GetPrice() = 0;

  virtual float GetBuildSpeed() = 0;

  // Do I need to read when we go from segment to segment?
  virtual bool IsCheatPath() const { return false; }

  // when work finished, engineers must say that furter building impossible
  virtual bool CannotFinish() const { return false; }

  // work accumulation
  virtual void AddWork(const float fAdd) { fWorkAccumulated += fAdd; }
  virtual float GetWorkDone() const { return fWorkAccumulated; }
};

class CEntrenchmentCreation : public CLongObjectCreation
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CEntrenchmentCreation);

  WORD GetLineAngle(const CVec2 &vBegin, const CVec2 &vEnd) const;
  float GetTrenchWidth(int nType);// 0 - section, 1 - turn

  void SplitLineToSegrments(std::vector<CVec2> *vPoints, CVec2 vBegin, CVec2 vEnd, float TRENCHWIDTH);

  CObj<CEntrenchment> pFullEntrenchment;
  std::vector<CObj<CEntrenchmentPart>> parts;

  CObj<CEntrenchmentPart> pBeginTerminator;//
  CObj<CEntrenchmentPart> pEndTerminator;// current end terminator
  CObj<CEntrenchmentPart> pNewEndTerminator;// future ultimate terminator

  std::vector<CVec2> vPoints;// trench centers
  int nCurIndex;
  WORD wAngle;
  int nPlayer;
  CLine2 line;
  bool bCannot;
  bool bSayAck;

  CTilesSet tilesUnder;// Tiles under the next segment

  // consts
  // CGDBPtr<SGDBObjectDesc> pDesc;
  // CGDBPtr<SEntrenchmentRPGStats> pRPG;
  // int dbID;
  // int nTermInd;

  //
  bool CanDig(const SEntrenchmentRPGStats *pRPG, int dbID, const CVec2 &pt, WORD angle, int nFrameIndex);
  CEntrenchmentPart *AddElement(const SEntrenchmentRPGStats *pRPG, int dbID, const CVec2 &pt, WORD angle, int nFrameIndex);
  void CreateNewEndTerminator();
  void CalcTilesUnder();

  void InitConsts();

public:
  CEntrenchmentCreation() {}
  CEntrenchmentCreation(int nPlayer);

  static bool SearchTrenches(const CVec2 &vCenter, const SRect &rectToTest);


  bool PreCreate(const CVec2 &vFrom, const CVec2 &vTo) override;
  CLine2 GetCurLine() override { return line; }
  const int GetMaxIndex() const override;
  const int GetCurIndex() const override;
  const CVec2 GetNextPoint(int nPlace, int nMaxPlace) const override;
  void BuildNext() override;
  void GetUnitsPreventing(std::list<CPtr<CAIUnit>> *units) override;
  bool IsAnyUnitPrevent() const override;
  bool CanBuildNext() const override;
  void LockNext() override;
  float GetPrice() override;
  void LockCannotBuild() override { bCannot = true; }
  float GetBuildSpeed() override { return SConsts::ENGINEER_ENTRENCH_LENGHT_PER_QUANT; }
  bool CannotFinish() const override { return bSayAck; }
};

class CFenceCreation : public CLongObjectCreation
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CFenceCreation);

  // copied from Kostya
  struct APointHelper
  {
    std::vector<CVec2> m_points;
    APointHelper() {}

    bool operator()(long x, long y)
    {
      m_points.push_back(CVec2(x, y));
      return true;
    }
  };

  std::vector<CObj<CFence>> fenceSegements;// segments
  std::vector<CVec2> vPoints;// positions

  int nCurIndex;
  int nPlayer;
  void InitConsts();
  CTilesSet tilesUnder;
  bool isXConst;// the fence goes along the x-coordinate
  CLine2 line;
  bool bCannot;
  bool bSayAck;

  void CalcTilesUnder();
  bool IsCegmentToBeBuilt(class CFence *pObj) const;

public:
  CFenceCreation() {}
  CFenceCreation(int nPlayer);

  bool PreCreate(const CVec2 &vFrom, const CVec2 &vTo) override;
  CLine2 GetCurLine() override { return line; }
  const int GetMaxIndex() const override;
  const int GetCurIndex() const override;
  const CVec2 GetNextPoint(int nPlace, int nMaxPlace) const override;
  void BuildNext() override;
  void GetUnitsPreventing(std::list<CPtr<CAIUnit>> *units) override;
  bool IsAnyUnitPrevent() const override;
  bool CanBuildNext() const override;
  void LockNext() override;
  float GetPrice() override;
  void LockCannotBuild() override { bCannot = true; }
  float GetBuildSpeed() override { return SConsts::ENGINEER_FENCE_LENGHT_PER_QUANT; }
};

class CFullBridge;
class CBridgeSpan;

class CBridgeCreation : public CLongObjectCreation
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CBridgeCreation);

  // for sorting
  struct SBridgeSpanSort
  {
    bool operator()(const CObj<CBridgeSpan> &s1, const CObj<CBridgeSpan> &s2);
  };

  CObj<CFullBridge> pFullBridge;
  std::vector<CObj<CBridgeSpan>> spans;
  CVec2 vStartPoint;
  CLine2 line;
  int nCurIndex;
  WORD wDir;

  void UnlockTiles();
  void LockTiles();

public:
  CBridgeCreation() {}
  CBridgeCreation(class CFullBridge *pBridge, class CCommonUnit *pUnit);

  static CVec2 SortBridgeSpans(std::vector<CObj<CBridgeSpan>> *spans, class CCommonUnit *pUnit);

  // specific
  const CVec2 &GetStartPoint() const;// where to send the truck
  bool IsFirstSegmentBuilt() const;

  // common
  CLine2 GetCurLine() override;
  const int GetMaxIndex() const override;
  const int GetCurIndex() const override;
  const CVec2 GetNextPoint(int nPlace, int nMaxPlace) const override;
  void BuildNext() override;
  float GetPrice() override;
  float GetBuildSpeed() override;

  //
  bool IsAnyUnitPrevent() const override { return false; }
  bool CanBuildNext() const override { return true; }
  void LockNext() override {}
  void LockCannotBuild() override {}
  bool PreCreate(const CVec2 &vFrom, const CVec2 &vTo) override { return true; }
  void GetUnitsPreventing(std::list<CPtr<CAIUnit>> *units) override {}
  bool IsCheatPath() const override { return true; }
};

#endif // _ENTRENCHMENT_CREATION_INTERNAL_