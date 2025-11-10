#ifndef __GENERAL__
#define __GENERAL__

class CGeneral;
interface IGeneralDelayedTask;

enum EResupplyType
{
  ERT_REPAIR = 0,
  ERT_RESUPPLY = 1,
  ERT_HUMAN_RESUPPLY = 2,
  ERT_MORALE = 3,
  ERT_MEDICINE = 4,

  _ERT_COUNT = 5,
};

// main logic
class CSupremeBeing
{
  DECLARE_SERIALIZE;
  using Generals = std::unordered_map<int, CPtr<CGeneral>>;
  Generals generals;
  using DelayedTasks = std::list<CPtr<IGeneralDelayedTask>>;
  DelayedTasks delayedTasks;

  std::unordered_set<int/* Link ID */> ironmans;

public:
  void Segment();
  void Clear();

  void SetUnitVisible(class CAIUnit *pUnit, int nGeneralParty, bool bVisible);

  // when anti-aircraft guns begin to attack our plane
  void SetAAVisible(class CAIUnit *pUnit, int nGeneralParty, bool bVisible);

  // creates number of generals
  // every general knows about units that are mobile reserves
  void Init(const struct SAIGeneralMapInfo &mapInfo);
  // distribute units to generals
  void GiveNewUnitsToGenerals(const std::list<class CCommonUnit *> &pUnits);

  bool IsMobileReinforcement(int nParty, int nGroup) const;
  void AddReinforcement(class CAIUnit *pUnit);
  interface IEnemyContainer *GetEnemyConatiner(int nPlayer);

  bool MustShootToObstacles(int nPlayer);
  void RegisterDelayedTask(interface IGeneralDelayedTask *pTask);

  // for areas of resistance
  void UpdateEnemyUnitInfo(class CAIUnitInfoForGeneral *pInfo,
                           NTimer::STime lastVisibleTimeDelta, const CVec2 &vLastVisiblePos,
                           NTimer::STime lastAntiArtTimeDelta, const CVec2 &vLastVisibleAntiArtCenter, float fDistToLastVisibleAntiArt);
  void UnitChangedParty(class CAIUnit *pUnit, int nNewParty);
  void UnitDied(class CAIUnitInfoForGeneral *pInfo);
  void UnitDied(class CCommonUnit *pUnit);

  void ReserveAviationForTimes(int nParty, const std::vector<NTimer::STime> &times);

  // when some unit changed position.
  void UnitChangedPosition(class CCommonUnit *pUnit, const CVec2 &vNewPos);
  void UnitAskedForResupply(class CCommonUnit *pUnit, EResupplyType eType, bool bSet);

  void AddIronman(int nScriptGroup);
  bool IsIronman(int nScriptGroup) const;

  bool IsInResistanceCircle(const CVec2 &vPoint, int nGeneralParty);
};

#endif // __GENERAL__