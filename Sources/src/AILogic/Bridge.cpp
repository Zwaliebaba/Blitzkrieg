#include "stdafx.h"

#include "Bridge.h"
#include "AIStaticMap.h"
#include "Updater.h"
#include "UnitsIterators2.h"
#include "StaticObject.h"
#include "AIUnit.h"
#include "Cheats.h"
#include "Diplomacy.h"
#include "Statistics.h"
#include "AIWarFog.h"
#include "Formation.h"
#include "UnitSTates.h"
#include "StaticObjects.h"
#include "Scripts/Scripts.h"
#include "Graveyard.h"
#include "StaticObjectsIters.h"

extern CStaticObjects theStatObjs;
extern CGlobalWarFog theWarFog;
extern CStaticObjects theStatObjs;
extern CStaticMap theStaticMap;
extern CUpdater updater;
extern SCheats theCheats;
extern CDiplomacy theDipl;
extern CStatistics theStatistics;
extern CScripts *pScripts;
extern CGraveyard theGraveyard;

BASIC_REGISTER_CLASS(CBridgeSpan);

// **********************************************************************
// *CBridgeSpan*
// **********************************************************************

CBridgeSpan::CBridgeSpan(const SBridgeRPGStats *_pStats, const CVec2 &center, const int dbID, const float fHP, const int nFrameIndex)
  : CGivenPassabilityStObject(center, dbID, fHP, nFrameIndex), pStats(_pStats),
    bNewBuilt(false), bLocked(false), bDeletingAround(false),
    nScriptID(-1)
{
  Init();
  const CArray2D<BYTE> &passability = pStats->GetPassability(GetFrameIndex());
  unlockTypes.SetSizes(passability.GetSizeX(), passability.GetSizeY());
  unlockTypes.SetZero();
  // It is forbidden to build trenches under the entire bridge.
  CTilesSet tiles;
  GetCoveredTiles(&tiles);
  theStaticMap.AddUndigableTiles(tiles);

}

const int CBridgeSpan::GetDownX() const
{
  const int nSegment = pStats->GetSpanStats(GetFrameIndex()).nSlab;
  const SBridgeRPGStats::SSegmentRPGStats *pS = &pStats->GetSegmentStats(nSegment);
  return Max(0, static_cast<int>(GetCenter().x - pS->vOrigin.x + SConsts::TILE_SIZE / 2));
}

const int CBridgeSpan::GetUpX() const
{
  const int nSegment = pStats->GetSpanStats(GetFrameIndex()).nSlab;
  const SBridgeRPGStats::SSegmentRPGStats *pS = &pStats->GetSegmentStats(nSegment);
  return Min(theStaticMap.GetSizeX() * SConsts::TILE_SIZE, static_cast<int>(GetCenter().x - pS->vOrigin.x + SConsts::TILE_SIZE * pStats->GetPassability(nFrameIndex).GetSizeX() + SConsts::TILE_SIZE / 2));
}

const int CBridgeSpan::GetDownY() const
{
  const int nSegment = pStats->GetSpanStats(GetFrameIndex()).nSlab;
  const SBridgeRPGStats::SSegmentRPGStats *pS = &pStats->GetSegmentStats(nSegment);
  return Max(0, static_cast<int>(GetCenter().y - pS->vOrigin.y + SConsts::TILE_SIZE / 2));
}

const int CBridgeSpan::GetUpY() const
{
  const int nSegment = pStats->GetSpanStats(GetFrameIndex()).nSlab;
  const SBridgeRPGStats::SSegmentRPGStats *pS = &pStats->GetSegmentStats(nSegment);
  return Min(theStaticMap.GetSizeY() * SConsts::TILE_SIZE, static_cast<int>(GetCenter().y - pS->vOrigin.y + SConsts::TILE_SIZE * pStats->GetPassability(nFrameIndex).GetSizeY() + SConsts::TILE_SIZE / 2));
}

void CBridgeSpan::GetTilesForVisibilityInternal(CTilesSet *pTiles) const
{
  SRect rect;
  GetBoundRect(&rect);
  GetTilesCoveredByRectSides(rect, pTiles);
}

void CBridgeSpan::GetTilesForVisibility(CTilesSet *pTiles) const { pFullBridge->GetTilesForVisibility(pTiles); }

void CBridgeSpan::GetCoveredTiles(CTilesSet *pTiles) const
{
  SRect rect;
  GetBoundRect(&rect);
  GetTilesCoveredByRect(rect, pTiles);
}

bool CBridgeSpan::ShouldSuspendAction(const EActionNotify &eAction) const
{
  return
      !theDipl.IsEditorMode() &&
      (eAction == ACTION_NOTIFY_RPG_CHANGED ||
       eAction == ACTION_NOTIFY_UPDATE_DIPLOMACY ||
       eAction == ACTION_NOTIFY_CHANGE_FRAME_INDEX ||
       eAction == ACTION_NOTIFY_NEW_ST_OBJ && bNewBuilt);
}

void CBridgeSpan::Build()
{
  bNewBuilt = true;
  SetHitPoints(0);
  LockTiles();
  // theStatObjs.UpdateAllPartiesStorages( false, true );
  pFullBridge->SpanBuilt(this);
}

void CBridgeSpan::LockTiles(bool bInitialization)
{
  if (fHP < 0) return;// an unbuilt bridge cannot be locked

  if (bLocked) return;
  bLocked = true;

  const CVec2 vOrigin(pStats->GetOrigin(GetFrameIndex()));
  const CArray2D<BYTE> &passability = pStats->GetPassability(GetFrameIndex());

  const int nDownPointsX = GetDownX();
  const int nDownPointsY = GetDownY();

  // complete unlocking of the tiles that the bridge will change
  int nDownX = 2 * theStaticMap.GetSizeX() * SConsts::TILE_SIZE;
  int nDownY = 2 * theStaticMap.GetSizeY() * SConsts::TILE_SIZE;
  int nUpX = -1;
  int nUpY = -1;

  for (int y = 0; y < passability.GetSizeY(); ++y)
  {
    for (int x = 0; x < passability.GetSizeX(); ++x)
    {
      const SVector tile(AICellsTiles::GetTile(nDownPointsX + SConsts::TILE_SIZE * x, nDownPointsY + SConsts::TILE_SIZE * y));
      theStaticMap.AddBridgeTile(tile);
    }
  }

  for (int y = 0; y < passability.GetSizeY(); ++y)
  {
    for (int x = 0; x < passability.GetSizeX(); ++x)
    {
      if ((passability[y][x] & 0x10) || (passability[y][x] & 0x1))
      {
        const SVector tile(AICellsTiles::GetTile(nDownPointsX + SConsts::TILE_SIZE * x, nDownPointsY + SConsts::TILE_SIZE * y));

        if (theStaticMap.IsTileInside(tile))
        {
          unlockTypes[y][x] = theStaticMap.GetTileLockInfo(tile);
          theStaticMap.UnlockTile(tile, unlockTypes[y][x]);

          nDownX = Min(tile.x, nDownX);
          nDownY = Min(tile.y, nDownY);
          nUpX = Max(tile.x, nUpX);
          nUpY = Max(tile.y, nUpY);
        }
      }
    }
  }

  if (!bInitialization)
  {
    for (int i = 1; i < 16; i *= 2)
    {
      theStaticMap.UpdateMaxesForRemovedStObject
          (
              Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
              Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
              i
              );
    }
    theStaticMap.UpdateMaxesForRemovedStObject
        (
            Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
            Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
            AI_CLASS_ANY
            );
  }

  // location for those units that cannot cross the bridge
  // locating bridge tiles
  for (int y = 0; y < passability.GetSizeY(); ++y)
  {
    for (int x = 0; x < passability.GetSizeX(); ++x)
    {
      if ((passability[y][x] & 0x10) || (passability[y][x] & 0x1))
      {
        const SVector tile(AICellsTiles::GetTile(nDownPointsX + SConsts::TILE_SIZE * x, nDownPointsY + SConsts::TILE_SIZE * y));
        if (theStaticMap.IsTileInside(tile))
        {
          // for units
          if (passability[y][x] & 0x10) theStaticMap.LockTile(tile, pStats->dwAIClasses);
            // bridge
          else theStaticMap.LockTile(tile, AI_CLASS_ANY);
        }
      }
    }
  }

  if (!bInitialization)
  {
    for (int i = 1; i < 16; i *= 2)
    {
      theStaticMap.UpdateMaxesForAddedStObject
          (
              Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
              Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
              i
              );
    }
    theStaticMap.UpdateMaxesForAddedStObject
        (
            Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
            Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
            AI_CLASS_ANY
            );
  }
}

void CBridgeSpan::UnlockTiles(bool bInitialization)
{
  if (fHP < 0) return;// an unbuilt bridge cannot be locked
  if (!bLocked) return;
  bLocked = false;

  const CVec2 vOrigin(pStats->GetOrigin(GetFrameIndex()));
  const CArray2D<BYTE> &passability = pStats->GetPassability(GetFrameIndex());

  // unlocking tiles where certain classes of units can cross the bridge
  int nDownX = 2 * theStaticMap.GetSizeX() * SConsts::TILE_SIZE;
  int nDownY = 2 * theStaticMap.GetSizeY() * SConsts::TILE_SIZE;
  int nUpX = -1;
  int nUpY = -1;
  for (int y = 0; y < passability.GetSizeY(); ++y)
  {
    for (int x = 0; x < passability.GetSizeX(); ++x)
    {
      const SVector tile(AICellsTiles::GetTile(GetDownX() + SConsts::TILE_SIZE * x, GetDownY() + SConsts::TILE_SIZE * y));
      if (theStaticMap.IsTileInside(tile))
      {
        if (passability[y][x] & 0x10)
        {
          theStaticMap.UnlockTile(tile, pStats->dwAIClasses);
          theStaticMap.RemoveBridgeTile(tile);

          nDownX = Min(tile.x, nDownX);
          nDownY = Min(tile.y, nDownY);
          nUpX = Max(tile.x, nUpX);
          nUpY = Max(tile.y, nUpY);
        }

        theGraveyard.FreeBridgeTile(tile);
      }
    }
  }

  if (!bInitialization)
  {
    theStaticMap.UpdateMaxesForRemovedStObject
        (
            Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
            Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
            pStats->dwAIClasses
            );
  }

  // restoring the location of tiles that the bridge changed
  for (int y = 0; y < passability.GetSizeY(); ++y)
  {
    for (int x = 0; x < passability.GetSizeX(); ++x)
    {
      if (unlockTypes[y][x] != 0)
      {
        const SVector tile(AICellsTiles::GetTile(GetDownX() + SConsts::TILE_SIZE * x, GetDownY() + SConsts::TILE_SIZE * y));

        if (theStaticMap.IsTileInside(tile)) theStaticMap.LockTile(tile, unlockTypes[y][x]);
      }
    }
  }

  const SVector downTile = AICellsTiles::GetTile(GetDownX(), GetDownY());
  const SVector upTile =
      AICellsTiles::GetTile(GetDownX() + SConsts::TILE_SIZE * passability.GetSizeX(),
                            GetDownY() + SConsts::TILE_SIZE * passability.GetSizeY());

  theStaticMap.UpdateTerrainPassabilityRect(downTile.x, downTile.y, upTile.x, upTile.y, false);

  nDownX = Min(downTile.x, nDownX);
  nDownY = Min(downTile.y, nDownY);
  nUpX = Max(upTile.x, nUpX);
  nUpY = Max(upTile.y, nUpY);

  if (!bInitialization)
  {
    for (int i = 1; i < 16; i *= 2)
    {
      theStaticMap.UpdateMaxesForAddedStObject
          (
              Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
              Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
              Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
              i
              );
    }
    theStaticMap.UpdateMaxesForAddedStObject
        (
            Max(nDownX - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpX + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeX() - 1),
            Max(nDownY - SConsts::MAX_UNIT_TILE_RADIUS - 1, 0),
            Min(nUpY + SConsts::MAX_UNIT_TILE_RADIUS + 1, theStaticMap.GetSizeY() - 1),
            AI_CLASS_ANY
            );

    // theStatObjs.UpdateAllPartiesStorages( false, false );
  }
}

void CBridgeSpan::SetHitPoints(const float fNewHP)
{
  if (fHP == 0 && fNewHP != 0)
  {
    // theStatObjs.UpdateAllPartiesStorages( false, true );
    LockTiles();
    /* if ( nScriptID != -1 )
         */
  }
  else if (fHP > 0 && fNewHP == 0) UnlockTiles();

  if (fHP != fNewHP)
  {
    fHP = Min(fNewHP, GetStats()->fMaxHP);
    updater.Update(ACTION_NOTIFY_RPG_CHANGED, this);
  }
}

void CBridgeSpan::Die(const float fDamage)
{
  if (bDeletingAround) return;

  SRect boundRect;
  GetBoundRect(&boundRect);
  theGraveyard.DelKilledUnitsFromRect(boundRect, nullptr);

  UnlockTiles();

  updater.Update(ACTION_NOTIFY_RPG_CHANGED, this);
  fHP = 0.0f;

  // kill the units standing on this span
  const CVec2 rectCenter((GetDownX() + GetUpX()) * 0.5f, (GetDownY() + GetUpY()) * 0.5f);
  const CVec2 vAABBHalfSize((GetUpX() - GetDownX()) * 0.5f + SConsts::MAX_UNIT_TILE_RADIUS, (GetUpY() - GetDownY()) * 0.5f + SConsts::MAX_UNIT_TILE_RADIUS);

  std::list<CAIUnit *> deadUnits;
  theStaticMap.MemMode();
  theStaticMap.SetMode(ELM_STATIC);
  for (CUnitsIter<0, 1> iter(0, ANY_PARTY, rectCenter, Max(vAABBHalfSize.x, vAABBHalfSize.y)); !iter.IsFinished(); iter.Iterate())
  {
    CAIUnit *pUnit = *iter;
    if (pUnit->GetZ() <= 0)
    {
      const SRect unitRect(pUnit->GetUnitRect());

      if (boundRect.IsIntersected(unitRect))
      {
        if (IsRectOnLockedTiles(unitRect, AI_CLASS_ANY) || theStaticMap.IsLocked(pUnit->GetTile(), AI_CLASS_ANY)) deadUnits.push_back(pUnit);
        else if (pUnit->IsInFormation() && pUnit->GetFormation()->GetState())
        {
          EUnitStateNames eName = pUnit->GetFormation()->GetState()->GetName();
          if (EUSN_REPAIR_BRIDGE == eName || eName == EUSN_BUILD_LONGOBJECT) deadUnits.push_back(pUnit);
        }
      }
    }
  }
  std::list<CExistingObject *> deadObjects;
  for (CStObjCircleIter<false> iter(rectCenter, vAABBHalfSize.x + vAABBHalfSize.y); !iter.IsFinished(); iter.Iterate())
  {
    CExistingObject *pObj = *iter;
    if (pObj->IsAlive())
    {
      SRect objectRect;
      pObj->GetBoundRect(&objectRect);
      if (boundRect.IsIntersected(objectRect) || theStaticMap.IsLocked(AICellsTiles::GetTile(pObj->GetCenter()), AI_CLASS_ANY)) deadObjects.push_back(pObj);
    }
  }

  theStaticMap.RestoreMode();

  for (auto iter = deadUnits.begin(); iter != deadUnits.end(); ++iter)
  {
    CAIUnit *pUnit = *iter;
    theStatistics.UnitDead(pUnit);
    pUnit->Disappear();
  }

  bDeletingAround = true;
  for (auto it = deadObjects.begin(); it != deadObjects.end(); ++it)
  {
    CExistingObject *pObj = *it;
    if (pObj->IsAlive()) pObj->Die(0);
  }
  bDeletingAround = false;
}

void CBridgeSpan::TakeDamage(const float fDamage, const bool bFromExplosion, const int nPlayerOfShoot, CAIUnit *pShotUnit)
{
  if (bFromExplosion && fHP > 0 && pFullBridge->CanTakeDamage() && !theCheats.GetImmortals(0))
  {
    fHP = Max(0.0f, fHP - fDamage);

    if (theCheats.GetFirstShoot(theDipl.GetNParty(nPlayerOfShoot)) == 1) fHP = 0;

    if (fHP == 0) Die(fDamage);
    else
    {
      WasHit();
      updater.Update(ACTION_NOTIFY_RPG_CHANGED, this);
    }

    pFullBridge->DamageTaken(this, fDamage, bFromExplosion, nPlayerOfShoot, pShotUnit);
  }
}

void CBridgeSpan::TakeEditorDamage(const float fDamage)
{
  // it makes sense to inflict this damage
  if (fDamage > 0 && fHP > 0 || fDamage < 0 && fHP < GetStats()->fMaxHP)
  {
    const float fOldHP = fHP;

    fHP -= fDamage;
    if (fHP < 0) fHP = 0;
    if (fHP > GetStats()->fMaxHP) fHP = GetStats()->fMaxHP;

    if (fOldHP == 0 && fHP > 0) LockTiles();
    if (fOldHP != 0 && fHP == 0) UnlockTiles();

    // updater.Update( ACTION_NOTIFY_RPG_CHANGED, this );
  }
}

bool CBridgeSpan::IsPointInside(const CVec2 &point) const
{
  SRect boundRect;
  GetBoundRect(&boundRect);

  return boundRect.IsPointInside(point);
}

void CBridgeSpan::SetFullBrige(CFullBridge *_pFullBridge) { pFullBridge = _pFullBridge; }

// **********************************************************************
// *CFullBridge*
// **********************************************************************

CFullBridge::SSpanLock::SSpanLock(CBridgeSpan *pSpan, const WORD wDir)
  : pSpan(pSpan)
{
  // find tiles. 

  SRect rect;
  pSpan->GetBoundRect(&rect);
  GetTilesCoveredBySide(rect, &tiles, wDir + 65535 / 2);

  // unlock,
  for (CTilesSet::const_iterator it = tiles.begin(); it != tiles.end(); ++it)
  {
    const BYTE lockInfo = theStaticMap.GetTileLockInfo(*it);
    formerTiles.push_back(lockInfo);
    theStaticMap.UnlockTile(*it, lockInfo);
  }
  // lock for everyone
  theStaticMap.UpdateMaxesByTiles(tiles, AI_CLASS_ANY, true);
}

void CFullBridge::SSpanLock::Unlock()
{
  // unlock for everyone
  theStaticMap.UpdateMaxesByTiles(tiles, AI_CLASS_ANY, false);
  // lock it as it was
  std::list<BYTE>::const_iterator lockedIter = formerTiles.begin();
  BYTE aiClasses = 0;
  for (CTilesSet::const_iterator it = tiles.begin(); it != tiles.end(); ++it)
  {
    theStaticMap.LockTile(*it, *lockedIter);
    aiClasses |= *lockedIter;
    ++lockedIter;
  }

  SVector vMin, vMax;
  theStaticMap.CalcMaxesBoundsByTiles(tiles, &vMin, &vMax);
  theStaticMap.UpdateMaxesForAddedStObject(vMin.x, vMax.x, vMin.y, vMax.y, aiClasses);
  // forget the tiles
  tiles.clear();
  formerTiles.clear();

}

// **********************************************************************
// *CFullBridge*
// **********************************************************************

BASIC_REGISTER_CLASS(CFullBridge);

void CFullBridge::AddSpan(CBridgeSpan *pSpan)
{
  if (pSpan->GetHitPoints() < 0.0f) projectedSpans.push_back(pSpan);
  else { spans.push_back(pSpan); }
  ++nSpans;
}

void CFullBridge::SpanBuilt(CBridgeSpan *pSpan)
{
  for (auto it = projectedSpans.begin(); it != projectedSpans.end(); ++it)
  {
    if (*it == pSpan)
    {
      projectedSpans.erase(it);
      AddSpan(pSpan);
      return;
    }
  }
}

const float CFullBridge::GetHPPercent() const
{
  NI_ASSERT_T(!spans.empty(), "no spans");
  return (*spans.begin())->GetHitPoints() / (*spans.begin())->GetStats()->fMaxHP;
}

bool CFullBridge::CanTakeDamage() const { return true; }

void CFullBridge::DamageTaken(CBridgeSpan *pDamagedSpan, const float fDamage, const bool bFromExplosion, const int nPlayerOfShoot, CAIUnit *pShotUnit)
{
  if (!bGivingDamage)
  {
    bGivingDamage = true;
    const float fNewHPPercent = pDamagedSpan->GetHitPoints() / pDamagedSpan->GetStats()->fMaxHP;

    for (auto iter = spans.begin(); iter != spans.end(); ++iter)
    {
      CBridgeSpan *pSpan = *iter;
      if (pSpan != pDamagedSpan)
      {
        // distribute damage to everyone to equalize the percentage of all HP
        const float fCurMaxHP = pSpan->GetStats()->fMaxHP;
        const float fCurHPPercent = pSpan->GetHitPoints() / fCurMaxHP;
        if (fCurHPPercent > fNewHPPercent)
        {
          const float fDamage = (fCurHPPercent - fNewHPPercent) * fCurMaxHP;
          pSpan->TakeDamage(fDamage, bFromExplosion, nPlayerOfShoot, pShotUnit);
        }
      }
    }

    bGivingDamage = false;
    theStatistics.ObjectDestroyed(nPlayerOfShoot);
  }
}

void CFullBridge::UnlockAllSpans()
{
  for (auto it = lockedSpans.begin(); it != lockedSpans.end();)
  {
    (*it)->Unlock();
    it = lockedSpans.erase(it);
  }
}

void CFullBridge::LockSpan(CBridgeSpan *pSpan, const WORD wDir) { lockedSpans.push_back(new SSpanLock(pSpan, wDir)); }

void CFullBridge::UnlockSpan(CBridgeSpan *pSpan)
{
  for (auto it = lockedSpans.begin(); it != lockedSpans.end();)
  {
    if ((*it)->GetSpan() == pSpan)
    {
      (*it)->Unlock();
      it = lockedSpans.erase(it);
    }
    else ++it;
  }
}

const int CFullBridge::GetNSpans() const { return nSpans; }

void CFullBridge::EnumSpans(std::vector<CObj<CBridgeSpan>> *pSpans)
{
  for (auto it = spans.begin(); it != spans.end(); ++it) pSpans->push_back(*it);
  for (auto it = projectedSpans.begin(); it != projectedSpans.end(); ++it) pSpans->push_back(*it);
}

const bool CFullBridge::IsVisible(const BYTE cParty) const
{
  CTilesSet tiles;
  GetTilesForVisibility(&tiles);

  for (CTilesSet::const_iterator iter = tiles.begin(); iter != tiles.end(); ++iter) { if (theWarFog.IsTileVisible(*iter, cParty)) return true; }

  return false;
}

void CFullBridge::GetTilesForVisibility(CTilesSet *pTiles) const { for (auto it = spans.begin(); it != spans.end(); ++it) (*it)->GetTilesForVisibilityInternal(pTiles); }