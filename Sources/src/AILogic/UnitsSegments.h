#ifndef __UNITS_SEGMENTS_H__
#define __UNITS_SEGMENTS_H__

#pragma once

class CCommonUnit;
class CAIUnit;

class CFreezeSegments
{
  class CCommonUnit *pUnit;

public:
  using TObjType = CCommonUnit *;

  CFreezeSegments() : pUnit(nullptr) {}

  bool ShouldBeUnregistered() const { return false; }

  void SetSegmentObject(CCommonUnit *pUnit);
  bool Check();
  const NTimer::STime ProcessSegment();
};

class CStateSegments
{
  class CCommonUnit *pUnit;

  bool bCheck;
  bool bIsValid;

public:
  using TObjType = CCommonUnit *;

  CStateSegments() : pUnit(nullptr), bCheck(false) {}

  bool ShouldBeUnregistered() const;

  void SetSegmentObject(CCommonUnit *pUnit);
  bool Check();
  const NTimer::STime ProcessSegment();
};

class CFirstPathSegments
{
  class CAIUnit *pUnit;

public:
  using TObjType = CAIUnit *;

  CFirstPathSegments() : pUnit(nullptr) {}

  bool ShouldBeUnregistered() const { return false; }

  void SetSegmentObject(CAIUnit *pUnit);
  bool Check();
  const NTimer::STime ProcessSegment();
};

class CSecondPathSegments
{
  class CAIUnit *pUnit;

public:
  using TObjType = CAIUnit *;

  CSecondPathSegments() : pUnit(nullptr) {}

  bool ShouldBeUnregistered() const { return false; }

  void SetSegmentObject(CAIUnit *pUnit);
  bool Check();
  const NTimer::STime ProcessSegment();
};

class CStayTimeSegments
{
  class CAIUnit *pUnit;

public:
  using TObjType = CAIUnit *;

  CStayTimeSegments() : pUnit(nullptr) {}

  void SetSegmentObject(CAIUnit *pUnit);
  bool Check();
  void ProcessSegment();
};

#endif // __UNITS_SEGMENTS_H__