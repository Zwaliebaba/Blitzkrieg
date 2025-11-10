#ifndef __AI_UNIT_INFO_FOR_GENERAL_H__
#define __AI_UNIT_INFO_FOR_GENERAL_H__

#pragma once

class CAIUnitInfoForGeneral : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CAIUnitInfoForGeneral);
  DECLARE_SERIALIZE;

  class CAIUnit *pOwner;

  NTimer::STime lastVisibleTime;
  CVec2 vLastVisiblePosition;

  NTimer::STime lastVisibleAntiArtTime;
  CVec2 vLastVisibleAntiArtCenter;
  float fDistToLastVisibleAntiArt;

  // When is the next time to tell the general about a change in condition?
  NTimer::STime nextTimeToReportGeneral;

  float fWeight;
  CVec2 vLastRegisteredGeneralPos;

public:
  CAIUnitInfoForGeneral() : pOwner(nullptr), fWeight(0), vLastRegisteredGeneralPos(VNULL2) {}
  CAIUnitInfoForGeneral(class CAIUnit *pOwner);

  void Segment();

  void UpdateVisibility(bool bVisible);
  void UpdateAntiArtFire(NTimer::STime lastHeardTime, const CVec2 &vAntiArtCenter);

  void Die();

  class CAIUnit *GetOwner() const { return pOwner; }

  void SetWeight(const float _fWeight) { fWeight = static_cast<int>(_fWeight * 100) / 100.0f; }
  const float GetWeight() const { return fWeight; }
  void SetRegisteredPos(const CVec2 &vPos) { vLastRegisteredGeneralPos = vPos; }
  const CVec2 &GetRegisteredPos() const { return vLastRegisteredGeneralPos; }

  const NTimer::STime GetLastTimeOfVisibility() const { return Max(lastVisibleAntiArtTime, lastVisibleTime); }

  void SetLastVisibleTime(const NTimer::STime time)
  {
    lastVisibleTime = time;
    lastVisibleAntiArtTime = time;
  }

  bool IsLastVisibleAntiArt() const { return lastVisibleAntiArtTime > lastVisibleTime; }
};

#endif // __AI_UNIT_INFO_FOR_GENERAL_H__