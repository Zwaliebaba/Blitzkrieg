#if !defined(__LA__Types__)
#define __LA__Types__

// #include "..\AILogic\AIConsts.h"
#include "Polygons_types.h"
#include "../Formats/FmtMap.h"


// types of tiles used for constructing tilemaps for various purposes
extern const BYTE RMGC_UNLOCKED;// 0
extern const BYTE RMGC_LOCKED;// 1

extern const BYTE RMGC_START_POINT;// 2
extern const BYTE RMGC_FINISH_POINT;// 3

extern const BYTE RMGC_INVALID_DIRECTION;// 4
extern const BYTE RMGC_HORIZONTAL_TO_ZERO;// 5
extern const BYTE RMGC_HORIZONTAL_FROM_ZERO;// 6
extern const BYTE RMGC_VERTICAL_TO_ZERO;// 7
extern const BYTE RMGC_VERTICAL_FROM_ZERO;// 8

// shifts in directions
extern const CTPoint<int> RMGC_SHIFT_POINTS[4];
// directions opposite to this one
extern const BYTE RMGC_NEGATIVE_DIRECTIONS[4];


// Get the opposite direction to the given one
inline BYTE GetNegativeDirection(BYTE nDirection)
{
  NI_ASSERT_T(( nDirection >= RMGC_HORIZONTAL_TO_ZERO ) &&
              ( nDirection <= RMGC_VERTICAL_FROM_ZERO ),
              NStr::Format( "Wrong direction: %d\n", nDirection ));
  return RMGC_NEGATIVE_DIRECTIONS[nDirection - RMGC_HORIZONTAL_TO_ZERO];
}


// Get offset point
inline CTPoint<int> GetShiftPoint(BYTE nDirection)
{
  NI_ASSERT_T(( nDirection >= RMGC_HORIZONTAL_TO_ZERO ) &&
              ( nDirection <= RMGC_VERTICAL_FROM_ZERO ),
              NStr::Format( "Wrong direction: %d\n", nDirection ));
  return RMGC_SHIFT_POINTS[nDirection - RMGC_HORIZONTAL_TO_ZERO];
}


// dot dot does not exceed array boundaries
template<class Type>
bool IsValidIndices(const Type &rLockArray, int nXPosition, int nYPosition)
{
  return ((nXPosition >= 0) &&
          (nXPosition < rLockArray.GetSizeX()) &&
          (nYPosition >= 0) &&
          (nYPosition < rLockArray.GetSizeY()));
}


template<class Type>
bool IsValidIndices(const Type &rLockArray, const CTPoint<int> &rPoint) { return IsValidIndices(rLockArray, rPoint.x, rPoint.y); }


// if returned -1 - not a single pIndices point falls into the rRect rectangle
// if they returned 0, a cut-off occurred on one of the sides
// if they returned 1 - pIndices did not change
template<class Type>
int ValidateIndices(const Type &rRect, Type *pIndices)
{
  NI_ASSERT_T(pIndices != 0,
              NStr::Format( "Wrong parameter: %x\n", pIndices ));

  pIndices->Normalize();

  // We do not consider the degenerate case
  if ((pIndices->minx >= rRect.maxx) ||
      (pIndices->miny >= rRect.maxy) ||
      (pIndices->maxx <= rRect.minx) ||
      (pIndices->maxy <= rRect.miny)) { return (-1); }

  int result = 1;
  // Defining boundaries
  if (pIndices->minx < rRect.minx)
  {
    pIndices->minx = rRect.minx;
    result = 0;
  }
  if (pIndices->miny < rRect.miny)
  {
    pIndices->miny = rRect.miny;
    result = 0;
  }
  if (pIndices->maxx > rRect.maxx)
  {
    pIndices->maxx = rRect.maxx;
    result = 0;
  }
  if (pIndices->maxy > rRect.maxy)
  {
    pIndices->maxy = rRect.maxy;
    result = 0;
  }
  return result;
}


template<class Type>
bool IsValidPointForNormalRect(const Type &rRect, int x, int y)
{
  return (x >= rRect.minx) && (x < rRect.maxx) &&
         (y >= rRect.miny) && (y < rRect.maxy);
}


template<class Type, class PointType>
bool IsValidPointForNormalRect(const Type &rRect, const PointType &rPoint) { return IsValidPointForNormalRect(rRect, rPoint.x, rPoint.y); }


template<class Type>
bool IsValidPoint(const Type &rRect, int x, int y)
{
  bool bXIsValid = true;
  if (rRect.minx < rRect.maxx) { bXIsValid = (x >= rRect.minx) && (x < rRect.maxx); }
  else { bXIsValid = (x >= rRect.maxx) && (x < rRect.minx); }
  bool bYIsValid = true;
  if (rRect.miny < rRect.maxy) { bYIsValid = (y >= rRect.miny) && (y < rRect.maxy); }
  else { bYIsValid = (y >= rRect.maxy) && (y < rRect.miny); }

  return (bXIsValid && bYIsValid);
}


template<class Type, class PointType>
bool IsValidPoint(const Type &rRect, const PointType &rPoint) { return IsValidPoint(rRect, rPoint.x, rPoint.y); }


template<class Type, class PointType>
int ValidatePoint(const Type &rRect, PointType *pPoint)
{
  NI_ASSERT_T(pPoint != 0,
              NStr::Format( "Wrong parameter: %x\n", pPoint ));

  int result = 1;
  // Defining boundaries
  if (pPoint->x < rRect.minx)
  {
    pPoint->x = rRect.minx;
    result = 0;
  }
  if (pPoint->y < rRect.miny)
  {
    pPoint->y = rRect.miny;
    result = 0;
  }
  if (pPoint->x > (rRect.maxx - 1))
  {
    pPoint->x = (rRect.maxx - 1);
    result = 0;
  }
  if (pPoint->y > (rRect.maxy - 1))
  {
    pPoint->y = (rRect.maxy - 1);
    result = 0;
  }
  return result;
}


// The object is a StaticObject (has Passability)
inline bool IsObjectHasPassability(EObjGameType nObjGameType)
{
  return ((nObjGameType == SGVOGT_BUILDING) ||
          (nObjGameType == SGVOGT_FORTIFICATION) ||
          (nObjGameType == SGVOGT_TANK_PIT) ||
          (nObjGameType == SGVOGT_BRIDGE) ||
          (nObjGameType == SGVOGT_MINE) ||
          (nObjGameType == SGVOGT_OBJECT) ||
          (nObjGameType == SGVOGT_FENCE) ||
          (nObjGameType == SGVOGT_TERRAOBJ) ||
          (nObjGameType == SGVOGT_FLAG));
}


// functional collecting pairs of numbers (coordinates of points)
struct StoreTilesFunctional
{
  std::vector<CTPoint<int>> tiles;

  bool operator()(int nXIndex, int nYIndex)
  {
    tiles.push_back(CTPoint<int>(nXIndex, nYIndex));
    return true;
  }
};


// functionality that changes pLockArray to a given value
template<class Type, class PointType>
struct ModifyTilesFunctional
{
  PointType bValue;
  Type *pLockArray;

  ModifyTilesFunctional(Type *_pLockArray)
    : bValue(RMGC_LOCKED), pLockArray(_pLockArray)
  {
    NI_ASSERT_T(pLockArray != 0,
                NStr::Format( "Wrong parameter: %x\n", pLockArray ));
  }

  ModifyTilesFunctional(PointType _bValue, Type *_pLockArray)
    : bValue(_bValue), pLockArray(_pLockArray)
  {
    NI_ASSERT_T(pLockArray != 0,
                NStr::Format( "Wrong parameter: %x\n", pLockArray ));
  }

  bool operator()(int nXIndex, int nYIndex)
  {
    (*pLockArray)[nYIndex][nXIndex] = bValue;
    return true;
  }
};


// functionality that determines whether a tile of a given value exists in pLockArray
template<class Type, class PointType>
struct CheckTilesFunctional
{
  PointType bValue;
  const Type *pLockArray;
  bool isPresent;

  CheckTilesFunctional(const Type *_pLockArray)
    : bValue(RMGC_LOCKED), pLockArray(_pLockArray), isPresent(false)
  {
    NI_ASSERT_T(pLockArray != 0,
                NStr::Format( "Wrong parameter: %x\n", pLockArray ));
  }

  CheckTilesFunctional(PointType _bValue, const Type *_pLockArray)
    : bValue(_bValue), pLockArray(_pLockArray), isPresent(false)
  {
    NI_ASSERT_T(pLockArray != 0,
                NStr::Format( "Wrong parameter: %x\n", pLockArray ));
  }

  bool operator()(int nXIndex, int nYIndex)
  {
    if ((*pLockArray)[nYIndex][nXIndex] == bValue) { isPresent = true; }
    return (!isPresent);
  }
};


// applying functionals for the pLockArray field, the functionals are listed above
// some functionals can change the array
template<class TYPE>
bool ApplyTilesInObjectsPassability(const CTRect<int> &rRect,// limits of applicability of functionals
                                    const SMapObjectInfo *pMapObjectInfo,// index to an array of objects
                                    int nMapObjectInfoCount,// number of objects
                                    TYPE &rApplyFunctional,// functional
                                    bool isIgnoreInvalidIndices = false)// skip objects beyond the edges of the map
{
  IObjectsDB *pIDB = GetSingleton<IObjectsDB>();
  if (!pIDB) { return false; }

  for (int nObjectIndex = 0; nObjectIndex < nMapObjectInfoCount; ++nObjectIndex)
  {
    const SGDBObjectDesc *pGDBObjectDesc = pIDB->GetDesc(pMapObjectInfo[nObjectIndex].szName.c_str());
    if ((pGDBObjectDesc != nullptr) &&
        (IsObjectHasPassability(pGDBObjectDesc->eGameType)))
    {
      const SStaticObjectRPGStats *pStaticObjectRPGStats = NGDB::GetRPGStats<SStaticObjectRPGStats>(pMapObjectInfo[nObjectIndex].szName.c_str());
      const CVec2 &rOrigin = pStaticObjectRPGStats->GetOrigin(pMapObjectInfo[nObjectIndex].nFrameIndex);
      const CArray2D<BYTE> &rPassability = pStaticObjectRPGStats->GetPassability(pMapObjectInfo[nObjectIndex].nFrameIndex);
      CTPoint<int> start((pMapObjectInfo[nObjectIndex].vPos.x - rOrigin.x + (SAIConsts::TILE_SIZE / 2.0)) / SAIConsts::TILE_SIZE,
                         (pMapObjectInfo[nObjectIndex].vPos.y - rOrigin.y + (SAIConsts::TILE_SIZE / 2.0)) / SAIConsts::TILE_SIZE);

      CTRect<int> indices(start.x, start.y, start.x + rPassability.GetSizeX(), start.y + rPassability.GetSizeY());
      int result = ValidateIndices(rRect, &indices);
      // there are no tiles
      if (result < 0)
      {
        if (isIgnoreInvalidIndices)
        {
          // skip the object, move on to the next one
          continue;
        }
        // return an error
        return false;
      }
      // passability goes beyond the boundaries of the array
      if ((result < 1) && !isIgnoreInvalidIndices)
      {
        // return an error
        return false;
      }
      // run through the tiles
      for (int nXIndex = indices.minx; nXIndex < indices.maxx; ++nXIndex) { for (int nYIndex = indices.miny; nYIndex < indices.maxy; ++nYIndex) { if (rPassability[nYIndex - start.y][nXIndex - start.x] != RMGC_UNLOCKED) { if (!rApplyFunctional(nXIndex, nYIndex)) { return false; } } } }
    }
  }
  return true;
}

template<class TYPE>
bool ApplyTilesInObjectsPassability(const CTRect<int> &rRect, const SMapObjectInfo &rMapObjectInfo, TYPE &rApplyFunctional, bool isIgnoreInvalidIndices = false) { return ApplyTilesInObjectsPassability(rRect, &rMapObjectInfo, 1, rApplyFunctional, isIgnoreInvalidIndices); }


// application of functionals for all tiles included in the polygon, the functionals are listed above
template<class TYPE, class PolygonType, class PointType>
bool ApplyTilesInPolygon(const CTRect<int> &rRect,// limits of applicability of functionals
                         const PolygonType &rPolygon,// polygon
                         float fSide,// tile side
                         TYPE &rApplyFunctional)// functional
{
  CTRect<float> boundingBox = GetPolygonBoundingBox(rPolygon);
  CTRect<int> indices((boundingBox.minx + (fSide / 2.0f)) / fSide,
                      (boundingBox.miny + (fSide / 2.0f)) / fSide,
                      (boundingBox.maxx + (fSide / 2.0f)) / fSide,
                      (boundingBox.maxy + (fSide / 2.0f)) / fSide);
  if (ValidateIndices(rRect, &indices) < 0) { return false; }

  // run through the tiles
  for (int nXIndex = indices.minx; nXIndex < indices.maxx; ++nXIndex)
  {
    for (int nYIndex = indices.miny; nYIndex < indices.maxy; ++nYIndex)
    {
      CVec2 vTileCenter((nXIndex * fSide) + (fSide / 2.0f),
                        (nYIndex * fSide) + (fSide / 2.0f));
      PointType tileCenter = GetPointType(vTileCenter, static_cast<PointType *>(nullptr));
      if (ClassifyPolygon(rPolygon, tileCenter) != CP_OUTSIDE) { if (!rApplyFunctional(nXIndex, nYIndex)) { return false; } }
    }
  }
  return true;
}

// CRAP}


// application of functionals for all tiles included in the rectangle (including roads)
template<class TYPE>
bool ApplyTilesInRange(const CTRect<int> &rRect,// limits of applicability of functionals
                       int nMinX,// rectangle borders
                       int nMinY,//
                       int nMaxX,//
                       int nMaxY,//
                       TYPE &rApplyFunctional)// functional
{
  CTRect<int> indices(nMinX, nMinY, nMaxX + 1, nMaxY + 1);
  if (ValidateIndices(rRect, &indices) < 0) { return false; }

  // run through the tiles
  for (int nXIndex = indices.minx; nXIndex < indices.maxx; ++nXIndex) { for (int nYIndex = indices.miny; nYIndex < indices.maxy; ++nYIndex) { if (!rApplyFunctional(nXIndex, nYIndex)) { return false; } } }
  return true;
}

template<class TYPE>
bool ApplyTilesInRange(const CTRect<int> &rRect, const CTPoint<int> &rMin, const CTPoint<int> &rMax, TYPE &rApplyFunctional) { return ApplyTilesInRange(rRect, rMin.x, rMin.y, rMax.x, rMax.y, rApplyFunctional) }

template<class TYPE>
bool ApplyTilesInRange(const CTRect<int> &rRect, const CTRect<int> &range, TYPE &rApplyFunctional) { return ApplyTilesInRange(rRect, range.minx, range.miny, range.maxx, range.maxy, rApplyFunctional); }

// CRAP{optimize collecting tiles in a circle

// there are no local tiles in the radius
template<class TYPE>
bool ApplyTilesInCircle(const CTRect<int> &rRect,// limits of applicability of functionals
                        int nXPosition,// center of the circle
                        int nYPosition,//
                        int nRadius,// circle radius
                        TYPE &rApplyFunctional)// functional
{
  CTRect<int> indices(nXPosition - nRadius, nYPosition - nRadius, nXPosition + nRadius + 1, nYPosition + nRadius + 1);
  if (ValidateIndices(rRect, &indices) < 0) { return false; }

  // run through the tiles
  for (int nXIndex = indices.minx; nXIndex < indices.maxx; ++nXIndex) { for (int nYIndex = indices.miny; nYIndex < indices.maxy; ++nYIndex) { if ((sqr(nXIndex - nXPosition) + sqr(nYIndex - nYPosition)) <= sqr(nRadius)) { if (!rApplyFunctional(nXIndex, nYIndex)) { return false; } } } }
  return true;
}

// CRAP}

template<class TYPE>
bool ApplyTilesInCircle(const CTRect<int> &rRect, const CTPoint<int> &rCenter, int nRadius, TYPE &rApplyFunctional) { return ApplyTilesInCircle(rRect, rCenter.x, rCenter.y, nRadius, rApplyFunctional); }


// path search, returns the number of points added to the array pPoints array
// the array changes while the function is running
int FindPath(const CTPoint<int> &rStartPoint,// starting point (labeled RMGC_FINISH_POINT)
             const CTPoint<int> &rFinishPoint,// starting point (labeled RMGC_START_POINT)
             CArray2D<BYTE> *pLockArray,// tile array
             std::vector<CTPoint<int>> *pPointList);// an array of points where the path is added

#endif // #if !defined(__LA__Types__)

/* *

 */

/* *

 */