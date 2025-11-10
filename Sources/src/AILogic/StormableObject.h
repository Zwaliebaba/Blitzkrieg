#ifndef __STORMABLE_OBJECT_H__
#define __STORMABLE_OBJECT_H__

#pragma once
// //////////////////////////////////////////////////////////// 
class CSoldier;
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// *CStormableObject*
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CStormableObject
{
  DECLARE_SERIALIZE;

  CListsSet<CPtr<CSoldier>> attackers;// attackers for each side
  std::vector<int> nAttackers;// number of attackers for each side
  int nActiveAttackers;// the number of attackers already participating in the battle
  bool bAttackers;// are there attackers

  std::vector<NTimer::STime> startTimes;

  NTimer::STime lastSegment;

  //
  void Combat(class CSoldier *pAttacker, class CSoldier *pDefender);
  const float GetDamage(class CBasicGun *pGun, class CSoldier *pTarget) const;

  void MakeDefenders(int nParty);
  void DelFromAttackers(CSoldier *pUnit);
  bool FindInAttackers(CSoldier *pUnit) const;

protected:
  virtual void AddSoldier(class CSoldier *pUnit) = 0;
  virtual void DelSoldier(class CSoldier *pUnit, bool bFillEmptyFireplace) = 0;
  virtual void SoldierDamaged(class CSoldier *pUnit) = 0;

public:
  CStormableObject() : attackers(SAIConsts::MAX_NUM_OF_PLAYERS), nAttackers(SAIConsts::MAX_NUM_OF_PLAYERS), nActiveAttackers(0), bAttackers(false), startTimes(SAIConsts::MAX_NUM_OF_PLAYERS), lastSegment(0) {}

  void AddInsider(class CSoldier *pUnit);
  void DelInsider(class CSoldier *pUnit);
  void InsiderDamaged(class CSoldier *pUnit);

  bool Segment();

  bool IsUnitsInside() const { return bAttackers || GetNDefenders() != 0; }
  const int GetNAttackers(const int nPlayer) const { return nAttackers[nPlayer]; }
  const int GetNFriendlyAttackers(int nPlayer) const;
  bool IsAnyAttackers() const { return bAttackers; }

  virtual const int GetNDefenders() const = 0;
  virtual class CSoldier *GetUnit(int n) const = 0;
  virtual const BYTE GetPlayer() const = 0;
  const bool IsAnyInsiderVisible() const;
};

// //////////////////////////////////////////////////////////// 
#endif // __STORMABLE_OBJECT_H__