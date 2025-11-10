#ifndef _AI_TYPES_H__
#define _AI_TYPES_H__

#pragma once

// ************************************************************************************************************************ //
// **
// ** diplomacy info
// **
// **
// **
// ************************************************************************************************************************ //

enum EDiplomacyInfo
{
  EDI_ENEMY = 0x01,
  EDI_FRIEND = 0x02,
  EDI_NEUTRAL = 0x04
};

#pragma pack( 1 )
struct SSegment2Trench
{
  IRefCount *pSegment;// small piece
  IRefCount *pEntrenchment;// the whole trench

  SSegment2Trench() : pSegment(nullptr), pEntrenchment(nullptr) {}
  SSegment2Trench(IRefCount *_pSegment, IRefCount *_pEntrenchment) : pSegment(_pSegment), pEntrenchment(_pEntrenchment) {}
};

struct SSoldier2Formation
{
  IRefCount *pSoldier;
  IRefCount *pFormation;

  //
  SSoldier2Formation() : pSoldier(nullptr), pFormation(nullptr) {}
  SSoldier2Formation(IRefCount *_pSoldier, IRefCount *_pFormation) : pSoldier(_pSoldier), pFormation(_pFormation) {}
};

// tile visibility update
struct SAIVisInfo
{
  DWORD x : 14;// x coord
  DWORD y : 14;// y coord
  DWORD vis : 4;// visibility: [0..5]
};

struct SAIPassabilityInfo
{
  DWORD x : 14;
  DWORD y : 14;
  DWORD pass : 4;
};

struct SMiniMapUnitInfo
{
  WORD x;
  WORD y;
  float z;
  BYTE player;

  SMiniMapUnitInfo() {}

  SMiniMapUnitInfo(const WORD _x, const WORD _y, const float _z, const BYTE _player)
    : x(_x), y(_y), z(_z), player(_player) {}
};

struct SShootArea
{
  enum EShootAreaType
  {
    ESAT_BALLISTIC = 0,
    ESAT_AA = 1,
    ESAT_LINE = 2,
    ESAT_RANGE_AREA = 3,
  };

  EShootAreaType eType;

  CVec3 vCenter3D;
  float fMinR, fMaxR;

  // angles define the cone of fire - counterclockwise
  WORD wStartAngle;
  WORD wFinishAngle;

  //
  SShootArea()
    : eType(ESAT_LINE), vCenter3D(VNULL3), fMinR(0.0f),
      fMaxR(0.0f), wStartAngle(65535), wFinishAngle(65535) {}

  const DWORD GetColor() const
  {
    static constexpr DWORD colors[] = {0xff88ff88, 0xff8888ff, 0xffff8888, 0xff88ff88};
    NI_ASSERT_T(static_cast<int>(eType) < 4, NStr::Format( "Wrong type of area (%d)", static_cast<int>(eType) ));
    return colors[eType];
  }

  const WORD GetMiniMapCircleColor() const
  {
    static constexpr WORD colors[] = {0xf0f0, 0xf00a, 0xff00, 0xf0f0};
    NI_ASSERT_T(static_cast<int>(eType) < 4, NStr::Format( "Wrong type of area (%d)", static_cast<int>(eType) ));
    return colors[eType];
  }

  const WORD GetMiniMapSectorColor() const
  {
    static constexpr WORD colors[] = {0xf0f0, 0xf00a, 0xff00, 0xf0f0};
    NI_ASSERT_T(static_cast<int>(eType) < 4, NStr::Format( "Wrong type of area (%d)", static_cast<int>(eType) ));
    return colors[eType];
  }
};

struct SShootAreas
{
  // output - sequentially, overlapping each other, first areas[0], then areas[1], etc.
  std::list<SShootArea> areas;

  virtual int STDCALL operator&(interface IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &areas);
    return 0;
  }
};
#pragma pack()

#endif // _AI_TYPES_H__