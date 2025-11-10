#ifndef __ANTI_ARTILLERY_H__
#define __ANTI_ARTILLERY_H__

#pragma once

#include "UpdatableObject.h"
#include "LinkObject.h"

class CRevealCircle : public CLinkObject
{
  OBJECT_COMPLETE_METHODS(CRevealCircle);
  DECLARE_SERIALIZE;

  CCircle circle;

public:
  CRevealCircle() {}
  CRevealCircle(const CVec2 &center, const float fR) : circle(center, fR) { SetUniqueId(); }
  CRevealCircle(const CCircle &_circle) : circle(_circle) { SetUniqueId(); }

  void GetRevealCircle(CCircle *pCircle) const override { *pCircle = circle; }

  const bool IsVisible(const BYTE party) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }
};

class CAntiArtillery : public CLinkObject
{
  OBJECT_COMPLETE_METHODS(CAntiArtillery);
  DECLARE_SERIALIZE;

  float fMaxRadius;
  int nParty;

  NTimer::STime lastScan;
  // time of the last shot heard and the last circle sent from this artillery for each side
  std::vector<NTimer::STime> lastShotTime;
  std::vector<NTimer::STime> lastRevealCircleTime;

  // distance to the nearest enemy (counts only for enemies)
  std::vector<float> closestEnemyDist2;
  std::vector<CVec2> lastHeardPos;
  std::vector<BYTE> nHeardShots;
  std::vector<CVec2> lastRevealCenter;

  //
  void Scan(const CVec2 &center);

public:
  CAntiArtillery() {}
  explicit CAntiArtillery(class CAIUnit *pOwner);

  void SetParty(const int _nParty) { nParty = _nParty; }

  void Init(float fMaxRadius, int nParty);
  void Fired(float fGunRadius, const CVec2 &center);

  // bOwnerVisible - whether the owner is visible to the player
  void Segment(bool bOwnerVisible);

  const CCircle GetRevealCircle(int nParty) const;
  const NTimer::STime GetLastHeardTime(int nParty) const;

  //
  const bool IsVisible(const BYTE party) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }

  //
  friend struct SAntiArtillerySort;
  friend class CAntiArtilleryManager;
};

#endif // __ANTI_ARTILLERY_H__