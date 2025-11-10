#ifndef __GEOMETRYMANAGER_H__
#define __GEOMETRYMANAGER_H__

#pragma once

#include "../Misc/BasicShare.h"
#include "GeometryMesh.h"

BASIC_SHARE_DECLARE(CMeshShare, std::string, CGeometryMesh, GFX_MESH, 105, "");

class CMeshManager : public IMeshManager
{
  OBJECT_COMPLETE_METHODS(CMeshManager);
  DECLARE_SERIALIZE;
  //
  CMeshShare share;

public:
  void STDCALL SetSerialMode(ESharedDataSerialMode eSerialMode) override { share.SetSerialMode(eSerialMode); }
  // setup sharing mode
  void STDCALL SetShareMode(ESharedDataSharingMode eShareMode) override { share.SetShareMode(eShareMode); }
  // remove all shared resource from this manager
  void STDCALL Clear(EClearMode eMode, int nUsage, int nAmount) override;
  //
  bool STDCALL Init() override { return share.Init(); }
  //
  IGFXMesh * STDCALL GetMesh(const char *pszName) override { return share.Get(pszName); }
  //
  void ClearContainers() { share.ClearContainers(); }
  void ReloadAllData() { share.ReloadAllData(); }
};

#endif // __GEOMETRYMANAGER_H__