#ifndef __UI_OBJECTIVE_MAP_H__
#define __UI_OBJECTIVE_MAP_H__

#include "UIBasic.h"

class CUIObjMap : public CMultipleWindow
{
  CPtr<IGFXTexture> pMapTexture;
  std::vector<SGFXLVertex> vertices;
  std::vector<WORD> indices;

public:
  CUIObjMap() {}
  ~CUIObjMap() override {}

  virtual void STDCALL Init();
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
  virtual void STDCALL SetMapTexture(IGFXTexture *p) { pMapTexture = p; }
};

class CUIObjMapBridge : public IUIObjMap, public CUIObjMap
{
  OBJECT_NORMAL_METHODS(CUIObjMapBridge);

public:
  DECLARE_SUPER(CUIObjMap);
  DEFINE_UICONTAINER_BRIDGE;
  void STDCALL Init() override { CSuper::Init(); }
  void STDCALL SetMapTexture(IGFXTexture *p) override { CSuper::SetMapTexture(p); }
};


#endif		// __UI_OBJECTIVE_MAP_H__