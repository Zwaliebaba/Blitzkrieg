#if !defined(__FMT__AI_GENERAL__H__)
#define __FMT__AI_GENERAL__H__


#pragma once

// #include "..\Common\Actions.h"


struct SAIGeneralParcelInfo
{
  enum EPatchType
  {
    EPATCH_UNKNOWN = 0,// center of defense
    EPATCH_DEFENCE = 1,// center of defense
    EPATCH_REINFORCE = 2,// reserves are accumulating here
  };

  // where to put reinforcement in this area
  struct SReinforcePointInfo
  {
    CVec2 vCenter;
    WORD wDir;

    // constructors
    SReinforcePointInfo() : vCenter(VNULL2), wDir(0) {}
    SReinforcePointInfo(const CVec2 &rvCenter, const WORD _wDir) : vCenter(rvCenter), wDir(_wDir) {}
    SReinforcePointInfo(const SReinforcePointInfo &rReinforcePointInfo) : vCenter(rReinforcePointInfo.vCenter), wDir(rReinforcePointInfo.wDir) {}

    SReinforcePointInfo &operator=(const SReinforcePointInfo &rReinforcePointInfo)
    {
      if (&rReinforcePointInfo != this)
      {
        vCenter = rReinforcePointInfo.vCenter;
        wDir = rReinforcePointInfo.wDir;
      }
      return *this;
    }

    // serializing...
    virtual int operator&(IDataTree &ss);
    virtual int operator&(IStructureSaver &ss);
  };

  std::vector<SReinforcePointInfo> reinforcePoints;// points where reinforcements need to be placed
  int eType;// type
  CVec2 vCenter;// AI points center
  float fRadius;// AI points size
  WORD wDefenceDirection;// direction of protection 0 - 65535

  // constructors
  SAIGeneralParcelInfo() : eType(EPATCH_UNKNOWN), vCenter(VNULL2), fRadius(0), wDefenceDirection(0) {}
  SAIGeneralParcelInfo(const SAIGeneralParcelInfo &rAIGeneralParcelInfo) : reinforcePoints(rAIGeneralParcelInfo.reinforcePoints), eType(rAIGeneralParcelInfo.eType), vCenter(rAIGeneralParcelInfo.vCenter), fRadius(rAIGeneralParcelInfo.fRadius), wDefenceDirection(rAIGeneralParcelInfo.wDefenceDirection) {}

  SAIGeneralParcelInfo &operator=(const SAIGeneralParcelInfo &rAIGeneralParcelInfo)
  {
    if (&rAIGeneralParcelInfo != this)
    {
      reinforcePoints = rAIGeneralParcelInfo.reinforcePoints;
      eType = rAIGeneralParcelInfo.eType;
      vCenter = rAIGeneralParcelInfo.vCenter;
      fRadius = rAIGeneralParcelInfo.fRadius;
      wDefenceDirection = rAIGeneralParcelInfo.wDefenceDirection;
    }
    return *this;
  }

  // serializing...
  virtual int operator&(IStructureSaver &ss);
  virtual int operator&(IDataTree &ss);
};


struct SAIGeneralSideInfo
{
  std::vector<int> mobileScriptIDs;// script IDs of units that belong to mobile reinforcements
  std::vector<SAIGeneralParcelInfo> parcels;

  // serializing...
  virtual int operator&(IDataTree &ss);
  virtual int operator&(IStructureSaver &ss);
};


struct SAIGeneralMapInfo
{
  std::vector<SAIGeneralSideInfo> sidesInfo;

  // serializing...
  virtual int operator&(IDataTree &ss);
  virtual int operator&(IStructureSaver &ss);
};

#endif // #if !defined(__FMT__AI_GENERAL__H__)