#ifndef __COMBATESTIMATOR_H__
#define __COMBATESTIMATOR_H__
// //////////////////////////////////////////////////////////// 
#include "../Misc/HashFuncs.h"
#include "AIHashFuncs.h"
class CAIUnit;
// //////////////////////////////////////////////////////////// 
// to determine the combat situation
class CCombatEstimator
{
  DECLARE_SERIALIZE;
  float fDamage;

  struct SShellInfo
  {
    NTimer::STime time;
    float fDamage;
    //
    SShellInfo() {}

    SShellInfo(NTimer::STime time, float fDamage)
      : time(time), fDamage(fDamage) {}
  };

  using CRegisteredUnits = std::unordered_set<int>;
  CRegisteredUnits registeredMechUnits;// enemy units (not infantry) with a non-zero current speed
  CRegisteredUnits registeredInfantry;// enemy units (infantry) with non-zero current speed

  using CShellTimes = std::list<SShellInfo>;
  CShellTimes shellTimes;// shot time

public:
  CCombatEstimator();

  void Clear();
  void Segment();

  bool IsCombatSituation() const;

  void AddShell(NTimer::STime time, float fDamage);

  void AddUnit(CAIUnit *pUnit);
  void DelUnit(CAIUnit *pUnit);

};
#endif // __COMBATESTIMATOR_H__