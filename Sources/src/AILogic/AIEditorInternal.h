#ifndef __AI_EDITOR_INTERNAL_H__
#define __AI_EDITOR_INTERNAL_H__

#pragma once

#include "AILogic.h"

class CAIEditor : public IAIEditor
{
  OBJECT_NORMAL_METHODS(CAIEditor);
  DECLARE_SERIALIZE;

  CPtr<ISegmentTimer> pGameSegment;
  bool IsRectInsideOfMap(const struct SRect &unitRect) const;

public:
  void Init(const struct STerrainInfo &terrainInfo) override;
  void Clear() override;

  bool IsFormation(IRefCount *pObject) const override;
  void GetUnitsInFormation(IRefCount *pObject, IRefCount ***pUnits, int *pnLen) override;
  IRefCount * GetFormationOfUnit(IRefCount *pObject) override;

  bool AddNewObject(const SMapObjectInfo &object, IRefCount **pObject) override;
  bool AddNewEntrencment(IRefCount **segments, int nLen, IRefCount **pObject) override;
  void LoadEntrenchments(const std::vector<SEntrenchmentInfo> &entrenchments) override;

  bool MoveObject(IRefCount *pObject, short x, short y) override;
  void DeleteObject(IRefCount *pObject) override;
  void DamageObject(IRefCount *pObject, float fHP) override;

  bool TurnObject(IRefCount *pObject, WORD wDir) override;

  float GetObjectHP(IRefCount *pObject) override;
  int GetObjectScriptID(IRefCount *pObject) override;

  void HandOutLinks() override;
  IRefCount * LinkToAI(int ID) override;
  int AIToLink(IRefCount *pObj) override;

  const CVec2 & GetCenter(IRefCount *pObj) const override;
  const WORD GetDir(IRefCount *pObj) const override;

  const int GetUnitDBID(IRefCount *pObj) const override;

  // checks whether the object is inside the map
  bool IsObjectInsideOfMap(const struct SMapObjectInfo &object) const override;
  // checks whether the object is inside the map and whether it is placed on locked tiles
  bool CanAddObject(const struct SMapObjectInfo &object) const override;

  void ApplyPattern(const struct SVAPattern &rPattern) override;
  void UpdateAllHeights() override;

  // enable - disable something, returns: true - enabled, false - disabled
  bool ToggleShow(int nShowType) override;

  void UpdateTerrain(const CTRect<int> &rect, const struct STerrainInfo &terrainInfo) override;

  void RecalcPassabilityForPlayer(CArray2D<BYTE> *array, int nPlayer) override;

  void SetPlayer(IRefCount *pObj, int nPlayer) override;
  void SetDiplomacies(const std::vector<BYTE> &playerParty) override;

  void DeleteRiver(const SVectorStripeObject &river) override;
  void AddRiver(const SVectorStripeObject &river) override;
};

#endif // __AI_EDITOR_INTERNAL_H__