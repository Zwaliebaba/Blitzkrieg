#ifndef __GENERAL_AIR_FORCE__
#define __GENERAL_AIR_FORCE__

#include "GeneralInternalInterfaces.h"
#include "AIHashFuncs.h"
#include "../Misc/FreeIDs.h"

class CAIUnit;
class CEnemyRememberer;

// manages aircraft and their departure for the entire party
class CGeneralAirForce : public IRefCount, public IEnemyEnumerator
{
  friend class CGeneralAirForceLaunchFighters;
  OBJECT_COMPLETE_METHODS(CGeneralAirForce);
  DECLARE_SERIALIZE;

  struct SSameEnemyPointPredicate
  {
    bool operator()(const CVec2 &v1, const CVec2 &v2) { return fabs2(v1 - v2) < sqr(SConsts::PLANE_GUARD_STATE_RADIUS / 2); }
  };

  int nParty;
  std::vector<int> players;// numbers of players who are under control
  CFreeIds requestsID;
  interface IEnemyContainer *pEnemyContainer;

  bool bReservedByFighters;// we are waiting for the aviation to regenerate to send fighters

  using AntiAviation = std::hash_map<int, CPtr<CEnemyRememberer>>;
  AntiAviation antiAviation;

  struct SSupportInfo
  {
    CVec2 vPoint;// where they called
    int nResistanceCellNumber;// if called to bomb a point of resistance, then > 0

    SSupportInfo() : vPoint(VNULL2), nResistanceCellNumber(-1) {}
  };

  // time by which aviation should be in a regenerated state
  std::vector<NTimer::STime> reservedTimes;


  std::list<CVec2> vFighterPoints;
  NTimer::STime timeLastCheck, timeLastFighterCheck;
  NTimer::STime checkPeriod;// next check for all aircraft except fighters
  NTimer::STime fighterCheckPeriod;// next test for fighters
public:
  using Requests = std::hash_map<int /* request ID */, SSupportInfo>;
  using RequestsByForceType = std::vector<Requests>;

private:
  RequestsByForceType requests;

  void LaunchScoutFree(int nPlayer);
  void PrepeareFighters(int nPlayer);
  void LaunchFighters(int nPlayer);
  void LaunchByRequest(int nPlayer, int nAvia, Requests *pRequest);

  void LaunchPlane(int /* SUCAviation::AIRCRAFT_TYPE */ nType, const std::list<CVec2> &vPoints, int nPlayer);

  // returns 0 if line is safe to fly.
  // otherwize returns seventy( how many planes will die while flying by this line )
  float CheckLineForSafety(const CVec2 &vStart, const CVec2 &vFinish, float fFlyHeight);
  bool IsTimePossible(int nPlayer, NTimer::STime timeToLaunch) const;// ability to launch aircraft at this time.

  void InitCheckPeriod();
  void InitFighterCheckPeriod();

public:
  CGeneralAirForce() {}
  CGeneralAirForce(int nPlayer, IEnemyContainer *pEnemyContainer);

  void Segment();

  int /* request ID */RequestForSupport(const CVec2 &vSupportCenter, enum EForceType eType, int nResistanceCellNumber = -1);
  void CancelRequest(int nRequestID, enum EForceType eType);

  void SetEnemyContainer(IEnemyContainer *_pEnemyConatainer) { pEnemyContainer = _pEnemyConatainer; }

  // IEnemyEnumerator
  bool EnumEnemy(class CAIUnit *pEnemy) override;

  void SetAAVisible(class CAIUnit *pUnit, bool bVisible);
  void DeleteAA(class CAIUnit *pUnit);
  void ReserveAviationForTimes(const std::vector<NTimer::STime> &times);

};

class CGeneralAirForceLaunchFighters : public IGeneralDelayedTask
{
  OBJECT_COMPLETE_METHODS(CGeneralAirForceLaunchFighters);
  DECLARE_SERIALIZE;

  CPtr<CGeneralAirForce> pAirForce;
  NTimer::STime timeToRun;
  int nPlayer;

public:
  CGeneralAirForceLaunchFighters() {}
  CGeneralAirForceLaunchFighters(class CGeneralAirForce *pAirForce, NTimer::STime timeToRun, int nPlayer);
  bool IsTimeToRun() const override;
  void Run() override;
};


#endif // __GENERAL_AIR_FORCE__