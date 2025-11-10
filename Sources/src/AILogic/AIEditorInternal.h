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
  void STDCALL Init(const struct STerrainInfo &terrainInfo) override;
  void STDCALL Clear() override;

  bool STDCALL IsFormation(IRefCount *pObject) const override;
  void STDCALL GetUnitsInFormation(IRefCount *pObject, IRefCount ***pUnits, int *pnLen) override;
  IRefCount * STDCALL GetFormationOfUnit(IRefCount *pObject) override;

  bool STDCALL AddNewObject(const SMapObjectInfo &object, IRefCount **pObject) override;
  bool STDCALL AddNewEntrencment(IRefCount **segments, int nLen, IRefCount **pObject) override;
  void STDCALL LoadEntrenchments(const std::vector<SEntrenchmentInfo> &entrenchments) override;

  bool STDCALL MoveObject(IRefCount *pObject, short x, short y) override;
  void STDCALL DeleteObject(IRefCount *pObject) override;
  void STDCALL DamageObject(IRefCount *pObject, float fHP) override;

  bool STDCALL TurnObject(IRefCount *pObject, WORD wDir) override;

  float STDCALL GetObjectHP(IRefCount *pObject) override;
  int STDCALL GetObjectScriptID(IRefCount *pObject) override;

  void STDCALL HandOutLinks() override;
  IRefCount * STDCALL LinkToAI(int ID) override;
  int STDCALL AIToLink(IRefCount *pObj) override;

  const CVec2 & STDCALL GetCenter(IRefCount *pObj) const override;
  const WORD STDCALL GetDir(IRefCount *pObj) const override;

  const int STDCALL GetUnitDBID(IRefCount *pObj) const override;

  // checks whether the object is inside the map
  bool STDCALL IsObjectInsideOfMap(const struct SMapObjectInfo &object) const override;
  // checks whether the object is inside the map and whether it is placed on locked tiles
  bool STDCALL CanAddObject(const struct SMapObjectInfo &object) const override;

  void STDCALL ApplyPattern(const struct SVAPattern &rPattern) override;
  void STDCALL UpdateAllHeights() override;

  // enable - disable something, returns: true - enabled, false - disabled
  bool STDCALL ToggleShow(int nShowType) override;

  void STDCALL UpdateTerrain(const CTRect<int> &rect, const struct STerrainInfo &terrainInfo) override;

  void STDCALL RecalcPassabilityForPlayer(CArray2D<BYTE> *array, int nPlayer) override;

  void STDCALL SetPlayer(IRefCount *pObj, int nPlayer) override;
  void STDCALL SetDiplomacies(const std::vector<BYTE> &playerParty) override;

  void STDCALL DeleteRiver(const SVectorStripeObject &river) override;
  void STDCALL AddRiver(const SVectorStripeObject &river) override;
};

#endif // __AI_EDITOR_INTERNAL_H__