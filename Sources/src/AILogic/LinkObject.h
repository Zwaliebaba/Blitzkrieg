#ifndef __LINK_OBJECT_H__
#define __LINK_OBJECT_H__

#pragma once

#include "UpdatableObject.h"

class CLinkObject : public IUpdatableObj
{
  DECLARE_SERIALIZE;

  static std::vector<CPtr<CLinkObject>> link2object;
  static std::list<int> deletedObjects;
  static std::list<int> deletedUniqueObjects;

  static std::unordered_map<int, CPtr<CLinkObject>> unitsID2object;
  static int nCurUniqueID;

  int nLink;
  int nUniqueID;

public:
  CLinkObject();
  CLinkObject(const int _nLink) { SetLink(_nLink); }
  virtual ~CLinkObject();

  void SetUniqueId();
  void SetLink(int _nLink);
  const int GetLink() const { return nLink; }
  // will the object be remembered in unitsID2Object
  void Mem2UniqueIdObjs();

  const int GetUniqueId() const override
  {
    /* NI_ASSERT_T( nUniqueID > 0, "Unique id isn't set" ); */
    return nUniqueID;
  }

  static void Clear();
  static void ClearLinks();
  static CLinkObject *GetObjectByLink(int nLink);
  static void Segment();
  // crashes if an incorrect nUniqueID is passed
  static CLinkObject *GetObjectByUniqueId(int nUniqueID);

  // returns 0 if an incorrect nUniqueID is passed
  static CLinkObject *GetObjectByUniqueIdSafe(const int nUniqueID)
  {
    NI_ASSERT_T(nUniqueID > 0, "Wrong object");
    if (unitsID2object.find(nUniqueID) == unitsID2object.end()) return nullptr;
    else return unitsID2object[nUniqueID];
  }

  // gives nSize of free links
  static void GetFreeLinks(std::list<int> *pLinks, int nSize);

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// returns 0 if an incorrect nUniqueID is passed
template<class T>
T *GetObjectByUniqueIdSafe(const int nUniqueID)
{
  CLinkObject *pLinkObject = CLinkObject::GetObjectByUniqueIdSafe(nUniqueID);
  return
      pLinkObject ? checked_cast<T *>(pLinkObject) : nullptr;
}

#endif // __LINK_OBJECT_H__