#ifndef __IM_TOTAL_ENCYCLOPEDIA_H__
#define __IM_TOTAL_ENCYCLOPEDIA_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "../Main/RPGStats.h"

class CInterfaceUnitsEncyclopediaBase
{
  int nViewUnitsType;// index in array unitsArray
protected:
  using CUnitTypesVector = std::vector<std::vector<const SUnitBaseRPGStats *>>;
  std::vector<CUnitTypesVector> unitsArray;
  const int nSides;

  std::hash_map<int/* nWindowID */, const SGDBObjectDesc *> gdbByWindowID;
  std::hash_map<const SUnitBaseRPGStats *, int/* WindowID */, SDefaultPtrHash> windowIDs;

  void FillUnitsList(int nType, IUIShortcutBar *pSB, interface IMovieProgressHook *pProgress = nullptr, bool bFillName = false);

  CInterfaceUnitsEncyclopediaBase(const int _nSides)
    : nViewUnitsType(-1), nSides(_nSides) { unitsArray.resize(nSides); }

  int GetViewIndex() const { return nViewUnitsType; }
  const char *GetUnitNameByWindowID(int nID);
  void InitialUpdate() { nViewUnitsType = -1; }
  void SetViewIndex(const int nViewIndex) { nViewUnitsType = nViewIndex; }
};

class CInterfaceTotalEncyclopedia : public CInterfaceInterMission, protected CInterfaceUnitsEncyclopediaBase
{
  OBJECT_NORMAL_METHODS(CInterfaceTotalEncyclopedia);

  // input
  enum
  {
    ALLIES,
    GERMAN,
    USSR,
    ALL,
  };

  bool bFinished;
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceTotalEncyclopedia() override;
  CInterfaceTotalEncyclopedia() : CInterfaceInterMission("InterMission"), CInterfaceUnitsEncyclopediaBase(3), bFinished(false) {}

  void SetActiveUnitsType(int nType);
  void InitUnitLists();

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICTotalEncyclopedia : public CInterfaceCommandBase<CInterfaceTotalEncyclopedia, MISSION_INTERFACE_TOTAL_ENCYCLOPEDIA>
{
  OBJECT_NORMAL_METHODS(CICTotalEncyclopedia);

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceTotalEncyclopedia *pITE) override;
  //
  CICTotalEncyclopedia() {}
};

class CInterfaceWarehouse : public CInterfaceInterMission, protected CInterfaceUnitsEncyclopediaBase
{
  OBJECT_NORMAL_METHODS(CInterfaceWarehouse);
  NInput::CCommandRegistrator commandMsgs;

public:
  // single unit info
  class CUnitInfoItem : public IRefCount
  {
    OBJECT_NORMAL_METHODS(CUnitInfoItem);
    const int nCommanderName;// does this unit have name
    CPtr<IUIDialog> pSBItem;// sb item for this unit
    std::string szCurrentRPGStats;// RPG stats of this unit
    const int nWindowID;// window ID
  public:
    CUnitInfoItem() : nCommanderName(-1), nWindowID(-1) {}
    CUnitInfoItem(int nCommanderName, const std::string &szRPGStats, int nWindowID);
    bool IsEmpty() const { return !pSBItem; }
    void Init(IUIDialog *pSBItem, IObjectsDB *pIDB);
    // set all SBItem information
    void ApplyRPGStats(const std::string &szNewStats, IObjectsDB *pIDB);
    // send current RPG stats to player profile
    void PerformUpgrade();
    void EnableWindow(bool bEnable);
    bool OnHelpCalled(const SGameMessage &msg, std::string *pHelp);
    const int GetID() const { return nWindowID; }
    int ApplyUpgrades();
    const char *GetRPGStats() const { return szCurrentRPGStats.c_str(); }
  };

  // unit's class info
  class CUnitClassInfo : public IRefCount
  {
    OBJECT_NORMAL_METHODS(CUnitClassInfo);
    std::vector<CPtr<CUnitInfoItem>> units;
    int nSBIndex;// current bar
  public:
    CUnitClassInfo() : nSBIndex(-1) {}
    void AddItem(const std::string &szRPGStats, int nCommanderNumber, int nWindowID);
    void Init(IUIShortcutBar *pSB, int nSBIndex, EUnitRPGClass eType, IObjectsDB *pIDB);
    bool IsEmpty() const { return units.empty(); }
    void Expand(IUIShortcutBar *pSB, bool bExpand, bool bDisableCollapsed, bool bNotify);
    // return true if there is item with this window id
    bool OnSelectionChanged(IUIShortcutBar *pSB);
    const int GetID() const { return nSBIndex; }
    void SelectFirstItem(IUIShortcutBar *pSB) const;
    bool OnHelpCalled(const SGameMessage &msg, std::string *pHelp);
    int ApplyUpgrades();
    CUnitInfoItem *GetUnit(int nIndex);
    const char *GetRPGStats(int nIndex);
  };

  // common pane
  class CUnitsPane
  {
    int nSelectedBar;
    bool bExpanded;
    int nExpandedNumber;// class number that currently expanded
  protected:
    std::vector<CPtr<CUnitClassInfo>> classes;
    CPtr<IUIShortcutBar> pSB;

    virtual int GetNUnits() const = 0;// total number of units in pane
    virtual const std::string &GetUnitStats(int nUnitIndex) const = 0;// getting RPG statistics
    virtual int GetScenarioNumber(int nIndex) const = 0;// scenario number of this unit
    virtual bool IsDisableCollapsed() const = 0;// should we disable collapsed items

    CUnitInfoItem *GetCurrentUnitPtr();

  public:
    CUnitsPane() : nSelectedBar(-1), bExpanded(false), nExpandedNumber(-1) {}
    void Init(IUIShortcutBar *_pSB, int *pWindowIDs, IObjectsDB *pDB);
    bool OnHelpCalled(const SGameMessage &msg, std::string *pHelp);
    // return RPG stats of currently selected unit or 0 if no selected unit
    const char *GetCurrentUnit();
    bool OnSelectionChanged(int *pNewType);
    void ExpandClass(EUnitRPGClass eType);
    void ExpandIndex(int nTypeIndex);
  };

  // container for player's units
  class CPlayerUnitsPane : public CUnitsPane
  {
  protected:
    int GetNUnits() const override;
    const std::string &GetUnitStats(int nUnitIndex) const override;
    int GetScenarioNumber(int nIndex) const override;
    bool IsDisableCollapsed() const override;

  public:
    CPlayerUnitsPane() {}
    void SetUpgrade(const std::string &szNewStats, IObjectsDB *pDB);
    int ApplyUpgrades();
  };

  // container for Depot units
  class CDepotUnitsPane : public CUnitsPane
  {
  protected:
    int GetNUnits() const override;
    const std::string &GetUnitStats(int nUnitIndex) const override;
    int GetScenarioNumber(int nIndex) const override;
    bool IsDisableCollapsed() const override;

  public:
    CDepotUnitsPane() {}
  };

private:
  CPlayerUnitsPane playerPane;
  CDepotUnitsPane depotPane;

  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceWarehouse() override;
  CInterfaceWarehouse() : CInterfaceInterMission("InterMission"), CInterfaceUnitsEncyclopediaBase(2) {}
  void EnableButton(int nButtonID, bool bEnable) const;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICWarehouse : public CInterfaceCommandBase<CInterfaceWarehouse, MISSION_INTERFACE_WAREHOUSE>
{
  OBJECT_NORMAL_METHODS(CICWarehouse);

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceWarehouse *pITE) override { pML->PushInterface(pITE); }
  //
  CICWarehouse() {}
};

#endif		// __IM_TOTAL_ENCYCLOPEDIA_H__