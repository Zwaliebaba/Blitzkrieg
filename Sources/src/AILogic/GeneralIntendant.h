#ifndef __GENERALINTENDANT_H__
#define __GENERALINTENDANT_H__

#pragma once
#include "Commander.h"
#include "StaticObjects.h"
#include "EnemyRememberer.h"

// //////////////////////////////////////////////////////////// 
class CResupplyCellInfo : public IRefCount
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CResupplyCellInfo);

  using CResupplyInfo = std::unordered_map<int/* Unique ID */, BYTE>;
  CResupplyInfo resupplyInfo;
  std::vector<float> resupplyCount;
  float fCount;// whole weight
  BYTE cMarkedUnderSupply;// indicates that this cell is under supply
  SVector vCell;// coordinates of the cell ( in GetGeneralCell )
  NTimer::STime timeLastDanger;// time when last truck was killed near this cell

  void RemoveUnitResupplyInternal(class CCommonUnit *pUnit, enum EResupplyType eType);

public:
  CResupplyCellInfo();
  void Init(const SVector &_vPos) { vCell = _vPos; }
  SVector GetCenter() const { return vCell; }
  void AddUnitResupply(class CCommonUnit *pUnit, enum EResupplyType eType);
  void RemoveUnitResupply(class CCommonUnit *pUnit, enum EResupplyType eType);

  byte RemoveUnit(class CCommonUnit *pUnit);
  void AddUnit(class CCommonUnit *pUnit, byte cRes);
  const bool IsUnitRegistered(CCommonUnit *pUnit) const;

  // for markink that this cell is under supply.
  void MarkUnderSupply(enum EResupplyType eType, bool bSupply = true);
  const bool IsMarkedUnderSupply(const enum EResupplyType eType) const { return cMarkedUnderSupply & (1 << eType); }
  // dander
  void SetDanger(NTimer::STime timeDanger);
  const bool IsDangerous() const;

  float GetNNeeded(byte cTypeMask) const;
  bool IsEmpty() const;
  float GetNNeeded(const enum EResupplyType eType) const { return resupplyCount[eType]; }

  static bool IsUnitSuitable(const class CCommonUnit *pUnit, enum EResupplyType eType);
  static void MoveUnitToCell(const class CCommonUnit *pUnit, enum EResupplyType eType);
  static void IssueCommand(class CCommonUnit *pUnit, enum EResupplyType eType, const CVec2 &vResupplyCenter);

  struct SSortByResupplyMaskPredicate
  {
    BYTE cMask;
    SSortByResupplyMaskPredicate(const BYTE cMask) : cMask(cMask) {}
    bool operator()(const CPtr<CResupplyCellInfo> &s1, const CPtr<CResupplyCellInfo> &s2) const { return s1->GetNNeeded(cMask) > s2->GetNNeeded(cMask); }
  };

  // were transport must be to cover all units with resupply.
  CVec2 CalcResupplyPos(enum EResupplyType eType) const;
};

// //////////////////////////////////////////////////////////// 
class CBuildingStorage;
using Storages = std::list<CPtr<CBuildingStorage>>;

class CGeneralTaskToDefendStorage : public IGeneralTask, public IWorkerEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskToDefendStorage);

public:
  class CWaitForChangePlayer : public IGeneralDelayedTask
  {
    DECLARE_SERIALIZE;
    OBJECT_COMPLETE_METHODS(CWaitForChangePlayer);
    CPtr<CGeneralTaskToDefendStorage> pMainTask;
    CPtr<CBuildingStorage> pStorage;
    int nParty;

  public:
    CWaitForChangePlayer() {}
    CWaitForChangePlayer(CBuildingStorage *pStorage, CGeneralTaskToDefendStorage *pMainTask, int nParty);
    bool IsTimeToRun() const override;
    void Run() override;
  };

private:
  enum ETaskState
  {
    TS_OPERATE,
    TS_START_RECAPTURE,
    TS_RECAPTURE,
    TS_FINISH_RECAPTURE,
    TS_START_REPAIR,
    TS_REPAIR,
    TS_FINISHED,
  };

  ETaskState eState;

  float fSeverity;
  int nParty;
  CPtr<CBuildingStorage> pStorage;
  CPtr<CCommonUnit> pRepairTransport;
  WORD wRequestID;

  void Recaptured();

public:
  CGeneralTaskToDefendStorage() {}
  CGeneralTaskToDefendStorage(CBuildingStorage *pStorage, int nParty);

  ETaskName GetName() const override { return ENT_DEFEND_ESTORAGE; }
  void AskForWorker(ICommander *pManager, float fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float fMinSeverity) override;
  void CancelTask(ICommander *pManager) override;
  void Segment() override;


  float GetSeverity() const override { return fSeverity; }
  bool IsFinished() const override { return eState == TS_FINISHED; }

  // IWorkerEnumerator
  bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;
  int NeedNBest(enum EForceType eType) const override;
  float EvaluateWorkerRating(CCommonUnit *pUnit, enum EForceType eType) const override;

  friend class CWaitForChangePlayer;
};

class CGeneralTaskToResupplyCell : public IGeneralTask, public IWorkerEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskToResupplyCell);

  CPtr<CResupplyCellInfo> pCell;
  CVec2 vResupplyCenter;
  int nParty;
  enum EResupplyType eResupplyType;
  CPtr<CCommonUnit> pResupplyTransport;
  bool bFinished;
  float fSeverity;
  NTimer::STime timeNextCheck;
  class CGeneralIntendant *pCells;

public:
  CGeneralTaskToResupplyCell() : pCells(nullptr) {}
  CGeneralTaskToResupplyCell(CResupplyCellInfo *pCell, int nParty, enum EResupplyType eType, class CGeneralIntendant *pCells);

  ETaskName GetName() const override { return ETN_RESUPPLYCELL; }
  void AskForWorker(ICommander *pManager, float fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float fMinSeverity) override;
  void CancelTask(ICommander *pManager) override;
  void Segment() override;


  float GetSeverity() const override { return fSeverity; }
  bool IsFinished() const override { return bFinished; }

  // IWorkerEnumerator
  bool EnumWorker(CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;
  int NeedNBest(enum EForceType eType) const override;
  float EvaluateWorkerRating(CCommonUnit *pUnit, enum EForceType eType) const override;
};

class CGeneralTaskCheckCellDanger : public IGeneralDelayedTask
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskCheckCellDanger);
  CPtr<IGeneralTask> pTask;
  CPtr<CResupplyCellInfo> pCell;
  CPtr<ICommander> pCommander;
  enum EResupplyType eResupplyType;

public:
  CGeneralTaskCheckCellDanger() {}

  CGeneralTaskCheckCellDanger(interface IGeneralTask *_pTask, class CResupplyCellInfo *_pCell, enum EResupplyType _eResupplyType, interface ICommander *_pCommander)
    : pTask(_pTask), pCell(_pCell), pCommander(_pCommander), eResupplyType(_eResupplyType) {}

  bool IsTimeToRun() const override { return pTask->IsFinished() || pCell->IsDangerous() || pCell->GetNNeeded(eResupplyType) == 0; }

  void Run() override { if (!pTask->IsFinished()) pTask->CancelTask(pCommander); }
};

// manipulate with storages and resupply trucks
class CGeneralIntendant : public CCommander
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralIntendant);

public:
  struct SVectorHash
  {
    int operator()(const SVector &v) const { return (v.x << 16) && v.y; }
  };

private:
  CArray2D<CPtr<CResupplyCellInfo>> cells;

  using ResupplyCells = std::unordered_map<SVector, CPtr<CResupplyCellInfo>, SVectorHash>;
  ResupplyCells cellsWithRequests;

  // artillery without crew.
  using CFreeArtillery = std::unordered_map</* Unique ID */ int, CPtr<CEnemyRememberer>>;
  CFreeArtillery freeArtillery;

  // storages (tasks to defend storages)
  CommonUnits resupplyTrucks;
  using CPosition = std::pair<CVec2, WORD>;
  std::vector<CPosition> vPositions;
  int nCurPosition;
  bool bInitedByParcel;

  int nParty;
  class CCommander *pGeneral;

  const bool IsUnitRegistered(CCommonUnit *pUnit) const;
  ResupplyCells::iterator GetCell(const CVec2 &vPos);
  void DeleteForgottenArtillery();

public:
  CGeneralIntendant() : pGeneral(nullptr) {}
  CGeneralIntendant(int nPlayer, CCommander *pGeneral);

  void Init();
  void AddReiforcePositions(const struct SAIGeneralParcelInfo &_patchInfo);
  void AddReiforcePosition(const CVec2 &vPos, WORD wDirection);

  void Give(CCommonUnit *pWorker) override;
  float GetMeanSeverity() const override { return 0; }
  void EnumWorkers(EForceType eType, IWorkerEnumerator *pEnumerator) override;
  void Segment() override;

  int RequestForSupport(const CVec2 &vSupportCenter, enum EForceType eType) override;
  void CancelRequest(int nRequestID, enum EForceType eType) override;

  void UnitDead(class CCommonUnit *pUnit);
  void UnitChangedPosition(class CCommonUnit *pUnit, const CVec2 &vNewPos);
  void UnitAskedForResupply(class CCommonUnit *pUnit, enum EResupplyType eType, bool bSet);
  void SetArtilleryVisible(const CAIUnit *pArtillery, bool bVisible);

  void MarkCellsDangerous(const SVector &vCell);
};

#endif // __GENERALINTENDANT_H__