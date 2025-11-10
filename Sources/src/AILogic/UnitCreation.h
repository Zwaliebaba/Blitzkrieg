#ifndef _UNIT_CREATION_INTERNAL_
#define _UNIT_CREATION_INTERNAL_

#pragma once

enum EGunplaneCalledAs
{
  EGCA_GUNPLANE,
  EGCA_DIVEBOMBER,
};

class CUnitCreation
{
  DECLARE_SERIALIZE;
  CPtr<IObjectsDB> pIDB;
  // to unify the creation of aircraft
  interface IPlaneCreation
  {
    virtual const CVec2 &GetDestPoint() const = 0;
    virtual enum EActionCommand GetCommand() const = 0;
    virtual int GetNParam() const = 0;
    virtual void CalcPositions(int nMax,
                               const CVec2 &vAABBbox,
                               const CVec2 &vDirection,
                               std::vector<CVec2> *positions,
                               CVec2 *pvOffset, bool bRandom = false) = 0;

  };

  //
  class CPlaneCreation : public IPlaneCreation
  {
    CVec2 vDestPoint;
    enum EActionCommand eCmd;
    int nParam;

  public:
    CPlaneCreation(const CVec2 &vDestPoint, const enum EActionCommand eCmd, const int _nParam)
      : vDestPoint(vDestPoint), eCmd(eCmd), nParam(_nParam) {}

    const CVec2 &GetDestPoint() const override { return vDestPoint; }
    enum EActionCommand GetCommand() const override { return eCmd; }
    int GetNParam() const override { return nParam; }
  };

  // for creating small airplanes
  class CLightPlaneCreation : public CPlaneCreation
  {
  public:
    CLightPlaneCreation(const CVec2 &vDestPoint, const enum EActionCommand eCmd, const int _nParam = 0)
      : CPlaneCreation(vDestPoint, eCmd, _nParam) {}

    void CalcPositions(int nMax,
                       const CVec2 &vAABBbox,
                       const CVec2 &vDirection,
                       std::vector<CVec2> *positions,
                       CVec2 *pvOffset, bool bRandom = false) override;
  };

  // to create heavy aircraft
  class CHeavyPlaneCreation : public CPlaneCreation
  {
    bool bNeedFormation;

  public:
    CHeavyPlaneCreation(const CVec2 &vDestPoint, const enum EActionCommand eCmd, const bool _bNeedFormation = false, const int _nParam = 0)
      : CPlaneCreation(vDestPoint, eCmd, _nParam), bNeedFormation(_bNeedFormation) {}

    void CalcPositions(int nMax,
                       const CVec2 &vAABBbox,
                       const CVec2 &vDirection,
                       std::vector<CVec2> *positions,
                       CVec2 *pvOffset, bool bRandom = false) override;
  };

public:
  // for storing information about tank trenches
  struct STankPitInfo
  {
    int operator&(IDataTree &ss);
    std::vector<std::string> sandBagTankPits;
    std::vector<std::string> digTankPits;
    const char *GetRandomTankPit(const class CVec2 &vSize, bool bCanDig, float *pfResize) const;
  };

  // to store party-specific information (Russians, Germans, ...)
  struct SPartyDependentInfo
  {
    std::string szPartyName;// country name
    std::string szGeneralPartyName;// name General Side

    std::string szParatroopSoldierName;// the name of the model who replaces the parachutist
    std::string szGunCrewSquad;// artillerymen
    std::string szHeavyMGSquad;// machine gunners
    std::string szResupplyEngineerSquad;// loaders near resource trucks
    // for use in the editor, serialize here
    int operator&(IDataTree &ss)
    {
      CTreeAccessor tree = &ss;
      tree.Add("PartyName", &szPartyName);
      tree.Add("GeneralPartyName", &szGeneralPartyName);

      tree.Add("GunCrewSquad", &szGunCrewSquad);
      tree.Add("HeavyMachinegunSquad", &szHeavyMGSquad);
      tree.Add("ParatrooperSoldier", &szParatroopSoldierName);
      tree.Add("ResupplyEngineerSquad", &szResupplyEngineerSquad);
      return 0;
    }
  };

  // to store general information (independent of the party)
  struct SCommonInfo
  {
    std::vector<std::string> antitankObjects;// anti-tank barriers
    std::string szAPFence;// anti-personnel wire fence
    std::string szMineAT;// anti-tank mine
    std::string szMineAP;// anti-personnel mine
    std::string szEntrenchment;// trench
    int operator&(IDataTree &ss);
  };

  //
  struct SFeedBack
  {
    int eEnable, eDisable;

    SFeedBack(const int eEnable, const int eDisable)
      : eEnable(eEnable), eDisable(eDisable) {}
  };

  std::vector<SFeedBack> feedbacks;

  //
  struct SLocalInGameUnitCreationInfo
  {
    DECLARE_SERIALIZE;

  public:
    struct SPlaneInfo
    {
      DECLARE_SERIALIZE;

    public:
      std::string szName;// this 3 parameters enables planes
      int nFormation;// due to map.
      int nPlanes;

      bool bEnabledScript;// for activation/deactivation by script
      SPlaneInfo() : nFormation(0), nPlanes(0), bEnabledScript(true) {}
    };

    std::vector<SPlaneInfo> planes;

    std::string szParatrooper;// name of parachutists
    int nParadropSquadCount;// number of parachute squads
    NTimer::STime timeLastCall;// last plane call
    NTimer::STime timeRelax;// aircraft call interval

    std::string szPartyName;// country name
    int nLastCalledAviaType;// last called aviation type

    // this parameter can disable all planes for player.
    std::vector<CVec2> vAppearPoints;// points of possible appearance (airfield coordinates)

    SLocalInGameUnitCreationInfo &operator=(const struct SUnitCreation &rSUnitCreation);
    SLocalInGameUnitCreationInfo(const struct SUnitCreation &rSUnitCreation);
    void Copy(const struct SUnitCreation &rSUnitCreation);
    SLocalInGameUnitCreationInfo() {}
  };

private:
  int nAviationCallNumeber;
  bool bInit;// for delaying initialization untill segment
  // this is for local player only
  bool bMainButtonDisabled;

  std::vector<BYTE> bForceDisabled;// aviation does not turn on at all

  std::vector<SLocalInGameUnitCreationInfo> inGameUnits;
  std::vector<BYTE> bLockedFlags;
  std::vector<CVec2> vLockedAppearPoints;

  // consts
  // read from xml, no need to save
  std::vector<SPartyDependentInfo> partyDependentInfo;
  SCommonInfo commonInfo;
  STankPitInfo tankPitInfo;

  // position of a bomber with a given number (in bounding boxes)
  void CalcPositionsForHeavyPlanes(int nMax, const CVec2 &box, const CVec2 &direction, std::vector<CVec2> *positions, CVec2 *offset, bool bRandom = false) const;

  void DisableMainAviationButton(NTimer::STime time);
  void EnableAviationButtons(bool bInit = false);

  void InitPlanePath(class CCommonUnit *pUnit, const CVec3 &vAppearPoint, const CVec3 &vGoToPoint);

  void InitConsts();
  const SPartyDependentInfo &GetPartyDependentInfo(int nDipl) const;
  void RegisterAviationCall(int nPlayer, int nAviaType);
  void CallPlane(int nPlayer,
                 int /* SUCAviation::AIRCRAFT_TYPE */ nAviaType,
                 WORD wGroupID,
                 IPlaneCreation *pCreation);

  bool IsAviaEnabledScript(int nPlayer, int /* SUCAviation::AIRCRAFT_TYPE */nAvia) const;
  bool IsAviaEnabledMapParameters(int nPlayer, int /* SUCAviation::AIRCRAFT_TYPE */nAvia) const;

public:
  CUnitCreation();
  // for the editor
  void Init();
  // for the game
  void Init(const struct SUnitCreationInfo &info);
  void Clear();

  const SLocalInGameUnitCreationInfo::SPlaneInfo &GetPlaneInfo(const int nPlayer, const int nAvia) { return inGameUnits[nPlayer].planes[nAvia]; }
  // 
  bool IsAviaEnabled(int nPlayer, int /* SUCAviation::AIRCRAFT_TYPE */nAvia) const;


  // returns number (in new units) of this unit
  int AddNewUnit(const std::string &name, IObjectsDB *pIDB, float fHPFactor, int x, int y, int z, WORD dir, BYTE player, bool bInitialization, bool IsEditor = false, bool bSendToWorld = true) const;
  int AddNewUnit(const SUnitBaseRPGStats *pStats, float fHPFactor, int x, int y, int z, WORD dbID, WORD _dir, BYTE player, EObjVisType eVisType, bool bInitialization = false, bool bSendToWorld = true, bool IsEditor = false) const;
  void GetCentersOfAllFormationUnits(const SSquadRPGStats *pStats, const CVec2 &vFormCenter, WORD wFormDir, int nFormation, int nUnits, std::list<CVec2> *pCenters) const;
  class CCommonUnit *AddNewFormation(const SSquadRPGStats *pStats, int nFormation, float fHP, float x, float y, float z, WORD wDir, int nDiplomacy, bool bInitialization = false, bool bSendToWorld = true, int nUnits = -1) const;
  class CCommonUnit *CreateSingleUnitFormation(class CSoldier *pSoldier) const;

  // specific functions
  void CallBombers(const struct SAIUnitCmd &unitCmd, WORD wGroupID, int nDipl);
  void CallFighters(const struct SAIUnitCmd &unitCmd, WORD wGroupID, int nDipl);
  void CallScout(const struct SAIUnitCmd &unitCmd, WORD wGroupID, int nDipl);
  void CallParadroppers(const struct SAIUnitCmd &unitCmd, WORD wGroupID, int nDipl);
  void CallShturmoviks(const SAIUnitCmd &unitCmd, WORD wGroupID, int nDipl);

  void CreateMine(enum SMineRPGStats::EType nType, const class CVec2 &vPoint, int nDipl);
  class CFormation *CreateParatroopers(const class CVec3 &where, class CAIUnit *pPlane, int nScriptID) const;
  class CFormation *CreateResupplyEngineers(class CAITransportUnit *pUnit) const;
  CFormation *CreateCrew(class CArtillery *pUnit, IObjectsDB *_pIDB = nullptr, int nUnits = -1, CVec3 vPos = CVec3(-1, -1, -1), int nPlayer = -1, bool bImmidiateAttach = true) const;

  // send Yura a formation so that it can be selected and in general so that it exists on the client
  void SendFormationToWorld(CFormation *pUnit) const;

  IObjectsDB *GetObjectDB() { return pIDB; }

  void LockAppearPoint(int nPlayer, bool bLocked);
  CVec2 GetRandomAppearPoint(int nPlayer, bool bLeave = false) const;
  const SMechUnitRPGStats *GetPlaneStats(int nPlayer, int /* SUCAviation::AIRCRAFT_TYPE */ nAvia) const;
  const float GetPlaneFlyHeight(int nPlayer, int nAvia);
  const NTimer::STime GetPlaneRegenerateTime(int nPlayer) const;
  int GetNParadropers(int nPlayer) const;

  int GetParadropperDBID(int nPlayer) const;
  const char *GetWireFenceName() const;
  const char *GetRandomAntitankObjectName() const;
  const char *GetEntrenchmentName() const;

  const char *GetRandomTankPit(const class CVec2 &vSize, bool bCanDig, float *pfResize) const;

  void PlaneLandedSafely(int nPlayer, int /* SUCAviation::AIRCRAFT_TYPE */ nAvia);
  void Segment();

  // for script activation
  void EnableAviationScript(int nPlayer, int nAvia);
  void DisableAviationScript(int nPlayer, int nAvia);
  // for inquiry from script
  int GetLastCalledAviation(int nPlayer) const;

  // for weather.
  void BadWeatherStarted();

  // calculates the point at which the aircraft's course will intersect with the edge of the map (during departure)
  const CVec2 GetFirstIntercectWithMap(int nPlayer);

  bool IsAntiTank(const SHPObjectRPGStats *pStats) const;
  bool IsAPFence(const SHPObjectRPGStats *pStats) const;
};

#endif // _UNIT_CREATION_INTERNAL_