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

  virtual void Init();
  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;
  virtual void SetMapTexture(IGFXTexture *p) { pMapTexture = p; }
};

class CUIObjMapBridge : public IUIObjMap, public CUIObjMap
{
  OBJECT_NORMAL_METHODS(CUIObjMapBridge);

public:
  DECLARE_SUPER(CUIObjMap);
  DEFINE_UICONTAINER_BRIDGE;
  void Init() override { CSuper::Init(); }
  void SetMapTexture(IGFXTexture *p) override { CSuper::SetMapTexture(p); }
};


#endif		// __UI_OBJECTIVE_MAP_H__