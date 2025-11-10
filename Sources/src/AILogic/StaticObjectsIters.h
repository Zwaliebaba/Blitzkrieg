#ifndef __STATIC_OBJECTS_ITERS__
#define __STATIC_OBJECTS_ITERS__

#pragma once

#include "StaticObjects.h"

//
// Iterators are not saved!
// 

extern CStaticObjects theStatObjs;

template<bool bOnlyContainers>
class CStObjIter
{
  int y, x;
  int downX, downY, upX, upY;

  CStaticObjects::StaticObjectsAreaMap::CDataList::iterator iter;

  //
  void NextXY();
  void IterateToNextCell();

  CStObjIter(const CStObjIter &iter);
  CStObjIter &operator=(const CStObjIter &iter);

protected:
  CStaticObjects::StaticObjectsAreaMap &GetAreaMap();
  int GetCellSize() const;

public:
  CStObjIter() : y(0), x(0), downX(0), downY(0), upX(0), upY(0) {}
  CStObjIter(int downX, int upX, int downY, int upY);
  ~CStObjIter() { theStatObjs.SetIterCreated(false); }

  void Iterate();
  bool IsFinished() const;

  void Reset()
  {
    theStatObjs.SetIterCreated(false);
    Init(downX, upX, downY, upY);
  }

  void Init(int _downX, int _upX, int _downY, int _upY);

  class CExistingObject *operator*();
};

// very slow!
template<bool bOnlyContainers>
class CStObjGlobalIter : public CStObjIter<bOnlyContainers>
{
  CStObjGlobalIter(const CStObjGlobalIter &iter);
  CStObjGlobalIter &operator=(const CStObjGlobalIter &iter);

public:
  CStObjGlobalIter();
};

template<bool bOnlyContainers>
class CStObjCircleIter : public CStObjIter<bOnlyContainers>
{
  CStObjCircleIter(const CStObjCircleIter &iter);
  CStObjCircleIter &operator=(const CStObjCircleIter &iter);

public:
  CStObjCircleIter(const CVec2 &vCenter, float fR);
};

// iterator by mines. 
// within a given radius from the center and are invisible to this player.
// if bAllMines, then all mines are taken, regardless of the player and visibility
// CRAP {
// uses a standard iterator, selecting mines from it. 
// CRAP}
class CMinesIter : protected CStObjCircleIter<false>
{
  int nParty;
  const bool bAllMines;

  void IterateToNextMine();

public:
  // iterate over mines invisible nParty
  CMinesIter(const CVec2 &vCenter, float fR, int nParty, bool bAllMines = false);
  void Iterate();
  bool IsFinished() const;
  void Reset();
  class CMineStaticObject *operator->();
  class CMineStaticObject *operator*();
};

//
// implementation
//

// **********************************************************************
// *CStObjIter*
// **********************************************************************

template<bool bOnlyContainers>
CStaticObjects::StaticObjectsAreaMap &CStObjIter<bOnlyContainers>::GetAreaMap() { return theStatObjs.GetAreaMap(); }

template<>
inline CStaticObjects::StaticObjectsAreaMap &CStObjIter<true>::GetAreaMap() { return theStatObjs.GetContainersAreaMap(); }

template<bool bOnlyContainers>
int CStObjIter<bOnlyContainers>::GetCellSize() const { return SConsts::STATIC_OBJ_CELL; }

template<>
inline int CStObjIter<true>::GetCellSize() const { return SConsts::STATIC_CONTAINER_OBJ_CELL; }

template<bool bOnlyContainers>
CStObjIter<bOnlyContainers>::CStObjIter<bOnlyContainers>(const int downX, const int upX, const int downY, const int upY) { Init(downX, upX, downY, upY); }

template<bool bOnlyContainers>
void CStObjIter<bOnlyContainers>::Init(const int _downX, const int _upX, const int _downY, const int _upY)
{
  downX = Clamp(_downX, 0, GetAreaMap().GetSizeX() - 1);
  upX = Clamp(_upX, 0, GetAreaMap().GetSizeX() - 1);
  downY = Clamp(_downY, 0, GetAreaMap().GetSizeY() - 1);
  upY = Clamp(_upY, 0, GetAreaMap().GetSizeY() - 1);

#if !defined(_FINALRELEASE) && !defined(_BETARELEASE)
  NI_ASSERT_T(!theStatObjs.IsIterCreated(), "Can't create two iterators concurrently");
  theStatObjs.SetIterCreated(true);
#endif // #if !defined(_FINALRELEASE) && !defined(_BETARELEASE)

  CExistingObject::UpdateGlobalMark();

  y = downY;
  x = downX;
  if (GetAreaMap().GetSizeX() > 0 && GetAreaMap().GetSizeY() > 0 && !GetAreaMap()[y][x].empty()) iter = GetAreaMap()[y][x].begin();
  else IterateToNextCell();

  if (!IsFinished()) (*iter)->SetGlobalUpdated();
}

template<bool bOnlyContainers>
void CStObjIter<bOnlyContainers>::IterateToNextCell()
{
  do { NextXY(); } while (!IsFinished() && GetAreaMap()[y][x].empty());

  if (!IsFinished()) iter = GetAreaMap()[y][x].begin();
}

template<bool bOnlyContainers>
void CStObjIter<bOnlyContainers>::NextXY()
{
  if (++x > upX)
  {
    ++y;
    x = downX;
  }
}

template<bool bOnlyContainers>
bool CStObjIter<bOnlyContainers>::IsFinished() const { return y > upY; }

template<bool bOnlyContainers>
void CStObjIter<bOnlyContainers>::Iterate()
{
  do { if (++iter == GetAreaMap()[y][x].end()) IterateToNextCell(); } while (!IsFinished() && (*iter)->IsGlobalUpdated());

  if (!IsFinished()) (*iter)->SetGlobalUpdated();
}

template<bool bOnlyContainers>
CExistingObject *CStObjIter<bOnlyContainers>::operator*()
{
  NI_ASSERT_T(!IsFinished(), "Can't perform operator *");
  return *iter;
}

// **********************************************************************
// *CStObjGlobalIter*
// **********************************************************************

template<bool bOnlyContainers>
CStObjGlobalIter<bOnlyContainers>::CStObjGlobalIter<bOnlyContainers>()
  : CStObjIter<bOnlyContainers>(0, GetAreaMap().GetSizeX() - 1, 0, GetAreaMap().GetSizeY() - 1) {}

// **********************************************************************
// *CStObjCircleIter*
// **********************************************************************

template<bool bOnlyContainers>
CStObjCircleIter<bOnlyContainers>::CStObjCircleIter<bOnlyContainers>(const CVec2 &vCenter, const float fR)
{
  const int nBigCellSize = SConsts::TILE_SIZE * GetCellSize();

  const int nMinX = Max(0, static_cast<int>((vCenter.x - fR) / nBigCellSize));
  const int nMaxX = Min(GetAreaMap().GetSizeX() - 1, static_cast<int>((vCenter.x + fR) / nBigCellSize));
  const int nMinY = Max(0, static_cast<int>((vCenter.y - fR) / nBigCellSize));
  const int nMaxY = Min(GetAreaMap().GetSizeY() - 1, static_cast<int>((vCenter.y + fR) / nBigCellSize));

  CStObjIter<bOnlyContainers>::Init(nMinX, nMaxX, nMinY, nMaxY);
}

#endif // __STATIC_OBJECTS_ITERS__