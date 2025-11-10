#include "stdafx.h"

#include "AIWarFog.h"
#include "StaticObject.h"
#include "../Misc/2DArrayRLEWrapper.h"

int CGlobalWarFog::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;

  // saver.Add( 1, &fogCnts );
  // saver.Add( 2, &maxVis );
  // saver.Add( 3, &minCoeff2 );
  saver.Add(4, &unitsInfo);
  saver.Add(5, &newUnitsInfo);
  saver.Add(6, &weights);
  saver.Add(7, &deletedUnits);
  saver.Add(8, &newUnits);
  saver.Add(9, &nMiniMapY);
  saver.Add(10, &miniMapSums);
  saver.Add(11, &removedObjects4Units);
  saver.Add(12, &addedObjects4Units);

  /* int nCheck = 1;
     */

  fogCnts.resize(2);
  maxVis.resize(2);
  minCoeff2.resize(2);

  int nChunk = 14;
  for (int i = 0; i < fogCnts.size(); ++i)
  {
    CArray2DRLEWrapper<WORD> wrapper(fogCnts[i], 0);
    saver.Add(nChunk++, &wrapper);
  }
  for (int i = 0; i < maxVis.size(); ++i)
  {
    CBitArray2DRLEWrapper<CArray2D4Bit> wrapper(maxVis[i], 0);
    saver.Add(nChunk++, &wrapper);
  }
  for (int i = 0; i < minCoeff2.size(); ++i)
  {
    CArray2DRLEWrapper<BYTE> wrapper(minCoeff2[i], floatToByte(1.0f));
    saver.Add(nChunk++, &wrapper);
  }

  {
    CBitArray2DRLEWrapper<CArray2D1Bit> wrapper(areasOpenTiles, 0);
    saver.Add(nChunk++, &wrapper);
  }
  saver.Add(nChunk++, &areas);

  return 0;
}

int SFogInfo::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;

  saver.Add(1, &center);
  saver.Add(2, &wUnitDir);
  saver.Add(3, &bAngleLimited);
  saver.Add(4, &pObject);
  saver.Add(5, &r);
  saver.Add(6, &wVisionAngle);
  saver.Add(7, &wMinAngle);
  saver.Add(8, &wMaxAngle);
  saver.Add(9, &bPlane);
  saver.Add(10, &fSightPower);

  return 0;
}

int CGlobalWarFog::SUnitInfo::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;

  saver.Add(1, &fogInfo);
  saver.Add(2, &nHeapPos);
  saver.Add(3, &nParty);
  /* #if !defined(_FINALRELEASE) && !defined(_BETARELEASE)
     */
  return 0;
}