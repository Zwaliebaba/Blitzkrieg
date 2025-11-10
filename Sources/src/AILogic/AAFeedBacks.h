#ifndef __AAFEEDBACKS_H__
#define __AAFEEDBACKS_H__

#pragma once

class CAAFeedBacks
{
  DECLARE_SERIALIZE;
  using CTargetList = std::list<int/* Planes unique id */>;
  using CAAFeedBacksList = std::hash_map<int/* AA Unique ID */, CTargetList>;


  CAAFeedBacksList feedbacks;

  void SendFeedBack(class CAIUnit *pAA) const;

public:
  void Clear();
  void Fired(class CAIUnit *pAA, class CAIUnit *pTarget);
  void PlaneDeleted(class CAIUnit *pTarget);
};

#endif // __AAFEEDBACKS_H__