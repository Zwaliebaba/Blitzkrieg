#ifndef __ROTATING_FIREPLACES_OBJECT_H__
#define __ROTATING_FIREPLACES_OBJECT_H__

#pragma once
// //////////////////////////////////////////////////////////// 
class CRotatingFireplacesObject
{
  DECLARE_SERIALIZE;

  struct SUnitInfo
  {
    DECLARE_SERIALIZE;

  public:
    class CSoldier *pSoldier;
    int nLastFireplace;
    NTimer::STime lastFireplaceChange;

    SUnitInfo() : pSoldier(nullptr), nLastFireplace(0), lastFireplaceChange(0) {}
  };

  std::list<SUnitInfo> units;

  //
  bool IsBetterToGoToFireplace(class CSoldier *pSoldier, int nFireplace) const;

public:
  CRotatingFireplacesObject() {}

  // called after the unit has been completely added to the object
  // nFireplace - fireplace number if a soldier is added to the fireplace
  void AddUnit(class CSoldier *pSoldier, int nFireplace);
  void DeleteUnit(class CSoldier *pSoldier);

  virtual void Segment();

  // Is it possible to change the slot for this slot?
  virtual bool CanRotateSoldier(class CSoldier *pSoldier) const = 0;
  // put the soldier in place instead of the one sitting there
  virtual void ExchangeUnitToFireplace(class CSoldier *pSoldier, int nFirePlace) = 0;
  // number of fireplaces
  virtual const int GetNFirePlaces() const = 0;
  // a soldier sitting in a fireplace, if the fireplace is empty then returns 0
  virtual class CSoldier *GetSoldierInFireplace(int nFireplace) const = 0;
};

// //////////////////////////////////////////////////////////// 
#endif // __ROTATING_FIREPLACES_OBJECT_H__