#ifndef __AI_GROUP_LOGIC_H__
#define __AI_GROUP_LOGIC_H__

#pragma once

#include "SegmentedObjects.h"
#include "UnitsSegments.h"

#include "../Common/Actions.h"
#include "../Misc/FreeIDs.h"

class CCommonUnit;
class CAIUnit;

class CGroupLogic : public IRefCount
{
  OBJECT_NORMAL_METHODS(CGroupLogic);
  DECLARE_SERIALIZE;

  CFreeIds groupIds;
  std::hash_set<int> registeredGroups;
  CQueuesSet<CPtr<CCommonUnit>> groupUnits;
  std::list<CPtr<CCommonUnit>> followingUnits;

  // 0 - all units, except of infantry; 
  std::vector<NSegmObjs::CContainer<CPtr<CCommonUnit>>> segmUnits;
  NSegmObjs::CContainer<CPtr<CCommonUnit>> freezeUnits;
  NSegmObjs::CContainer<CPtr<CAIUnit>> firstPathUnits;
  NSegmObjs::CContainer<CPtr<CAIUnit>> secondPathUnits;

  NTimer::STime lastSegmTime;

  struct SAmbushInfo
  {
    int nUniqueId;
    CVec2 vAmbushCenter;
    WORD wAmbushDir;
    bool bGivenCommandToRestore;

    SAmbushInfo() : nUniqueId(-1), vAmbushCenter(VNULL2), wAmbushDir(0), bGivenCommandToRestore(false) {}

    SAmbushInfo(const int _nUniqueId)
      : nUniqueId(_nUniqueId), vAmbushCenter(-1.0f, -1.0f), wAmbushDir(0), bGivenCommandToRestore(false) {}
  };

  using CAmbushGroups = std::list<std::list<SAmbushInfo>>;
  CAmbushGroups ambushGroups;
  std::hash_set<int> ambushUnits;
  NTimer::STime lastAmbushCheck;

  //
  void DelGroup(int nGroup);
  void DivideBySubGroups(const SAIUnitCmd &command, int nGroup);

  // unit speeds in follow
  void SegmentFollowingUnits();
  // handling units stuck due to collisions
  void StayTimeSegment();

  void ProcessGridCommand(const CVec2 &vGridCenter, const CVec2 &vGridDir, int nGroup, bool bPlaceInQueue);

  void EraseFromAmbushGroups(const SAIUnitCmd &command, WORD wGroup);
  void CreateAmbushGroup(WORD wGroup);
  void ProcessAmbushGroups();
  void SetToAmbush(CAmbushGroups::iterator &iter);

  //
  static WORD GetGroupNumberByID(WORD wID);
  static WORD GetSpecialGroupNumberByID(WORD wID);
  static WORD GetIdByGroupNumber(WORD wGroup);
  static WORD GetPlayerByGroupNumber(WORD wGroup);

public:
  CGroupLogic() {}
  void Init();
  void Clear() { DestroyContents(); }

  const WORD GenerateGroupNumber();
  void RegisterGroup(IRefCount **pUnitsBuffer, int nLen, WORD wGroup);
  void UnregisterGroup(WORD wGroup);

  void DelUnitFromGroup(class CCommonUnit *pUnit);
  void AddUnitToGroup(class CCommonUnit *pUnit, int nGroup);
  void DelUnitFromSpecialGroup(class CCommonUnit *pUnit);

  void GroupCommand(const SAIUnitCmd &command, WORD wGroup, bool bPlaceInQueue);
  void UnitCommand(const SAIUnitCmd &command, class CCommonUnit *pGroupUnit, bool bPlaceInQueue);
  void InsertUnitCommand(const SAIUnitCmd &command, class CCommonUnit *pUnit);
  void PushFrontUnitCommand(const SAIUnitCmd &command, class CCommonUnit *pUnit);

  // send updates to shoot areas for all units in the group
  void UpdateAllAreas(int nGroup, EActionNotify eAction);

  void Segment();

  void CreateSpecialGroup(WORD wGroup);
  void UnregisterSpecialGroup(WORD wSpecialGroup);

  int BeginGroup(const int nGroup) const { return groupUnits.begin(nGroup); }
  int EndGroup() const { return groupUnits.end(); }
  int Next(const int nIter) const { return groupUnits.GetNext(nIter); }
  class CCommonUnit *GetGroupUnit(const int nIter) const { return groupUnits.GetEl(nIter); }

  void AddFollowingUnit(class CCommonUnit *pUnit);

  void RegisterSegments(class CCommonUnit *pUnit, bool bInitialization, bool bAllInfo);
  void RegisterPathSegments(class CAIUnit *pUnit, bool bInitialization);

  void UnregisterSegments(class CCommonUnit *pUnit);

  void UnitSetToAmbush(class CCommonUnit *pUnit);
};

const CVec2 GetGoPointByCommand(const SAIUnitCmd &cmd);

#endif // __AI_GROUP_LOGIC_H__