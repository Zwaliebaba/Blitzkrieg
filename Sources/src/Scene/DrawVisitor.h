#ifndef __DRAWVISITOR_H__
#define __DRAWVISITOR_H__

#pragma once

#include "SceneInternal.h"
#include "DepthOptimizer.h"

// ************************************************************************************************************************ //
// **
// ** helper and container structures
// **
// **
// **
// ************************************************************************************************************************ //

struct STextObject
{
  CPtr<IGFXFont> pFont;
  const char *pszText;
  CVec3 vPos;
  CVec3 vScreenPos;
  DWORD color;
};

struct SBoldLineObject
{
  CVec3 *corners;
  DWORD color;
  //
  SBoldLineObject(CVec3 *_corners, DWORD _color) : corners(_corners), color(_color) {}
};

using CTextVisList = std::list<STextObject>;
using CUnknownVisList = std::list<ISceneObject *>;
using CBoldLineVisList = std::list<SBoldLineObject>;

struct SSortByFont
{
  bool operator()(const STextObject &s1, const STextObject &s2) const { return s1.pFont.GetPtr() < s2.pFont.GetPtr(); }
};

struct SUIObject
{
  enum EType
  {
    TYPE_UNKNOWN = 0,
    TYPE_RECTS = 1,
    TYPE_TEXT = 2,
    TYPE_CUSTOM = 3
  };

  //
  const EType eType;

  union
  {
    // rects specific
    struct
    {
      IGFXTexture *pTexture;
      int nShadingEffect;
    };

    // text specific
    struct
    {
      IGFXText *pText;
      RECT rcRect;
      int nY;
      DWORD dwColor;
      DWORD dwFlags;
    };

    // custom
    IUIElement *pElement;
  };

  // non-simple types
  // rects specific
  std::vector<SGFXRect2> rects;
  //
  SUIObject(const EType _eType) : eType(_eType) {}
  //
  bool IsRects() const { return eType == TYPE_RECTS; }
  bool IsText() const { return eType == TYPE_TEXT; }
  bool IsCustom() const { return eType == TYPE_CUSTOM; }
  EType GetType() const { return eType; }
};

// ************************************************************************************************************************ //
// **
// ** draw visitor itself
// **
// **
// **
// ************************************************************************************************************************ //

class CDrawVisitor : public ISceneVisitor
{
  OBJECT_MINIMAL_METHODS(CDrawVisitor);
  //
  ICamera *pCamera;
  SHMatrix matrix;
  CTRect<float> rcScreen;
  SPlane vViewVolumePlanes[6];
  CDepthOptimizer depthoptimizer;
  //
  void AddSingleSprite(const SBasicSpriteInfo *pObj, CSpriteVisList *pSprites, WORD wPriority);
  void AddSingleParticleEffect(IParticleSource *pPS, CParticlesVisMap *pParticles);
  void AddSingleMesh(IMeshVisObj *pMesh, CMeshVisList *pMeshes);
  void AddSingleMeshUnchecked(IMeshVisObj *pMesh, CMeshVisList *pMeshes);

public:
  CSpriteVisList sprites;
  CSpriteVisList spriteBuildings;
  CSpriteVisList spriteUnits;
  CSpriteVisList spriteEffects;
  CSpriteVisList spriteFlashes;
  CMeshVisList meshes;
  CMeshVisList aviation;
  // CMeshVisList meshTerraObjects;
  CSpriteVisList terraObjects;
  CSpriteVisList shadowObjects;
  CParticlesVisMap particles;
  CUnknownVisList unknowns;
  CBoldLineVisList boldLines;
  std::list<SMechTrace> traces;
  std::list<SGunTrace> gunTraces;
  // icons
  CSpriteVisList icons;// bar and picture icons
  CTextVisList textes;// text icons
  //
  using CUIObjectsList = std::deque<SUIObject>;
  CUIObjectsList uiObjects;// ui objects, divided by levels
  CDrawVisitor(const float fAllowedDepth)
    : pCamera(nullptr), depthoptimizer(100, 75, fAllowedDepth) {}

  //
  void Init(ICamera *_pCamera, const SHMatrix &_matrix, const CTRect<short> &_rcScreen, const SPlane *pViewVolumePlanes);
  void Clear();
  void Sort();
  // billboard sprite object
  void STDCALL VisitSprite(const SBasicSpriteInfo *pObj, int nType, int nPriority) override;
  // mesh object
  void STDCALL VisitMeshObject(IMeshVisObj *pObj, int nType, int nPriority) override;
  // particles
  void STDCALL VisitParticles(IParticleSource *pObj) override;
  // unknown scene object
  void STDCALL VisitSceneObject(ISceneObject *pObj) override;
  // text object
  void STDCALL VisitText(const CVec3 &vPos, const char *pszText, IGFXFont *pFont, DWORD color) override;
  // bold line object
  void STDCALL VisitBoldLine(CVec3 *corners, float fWidth, DWORD color) override;
  // mech trace object
  void STDCALL VisitMechTrace(const SMechTrace &trace) override;
  // gun trace object
  void STDCALL VisitGunTrace(const SGunTrace &trace) override;
  // UI elements visiting
  void STDCALL VisitUIRects(IGFXTexture *pTexture, int nShadingEffect, SGFXRect2 *rects, int nNumRects) override;
  void STDCALL VisitUIText(IGFXText *pText, const CTRect<float> &rcRect, int nY, DWORD dwColor, DWORD dwFlags) override;
  void STDCALL VisitUICustom(interface IUIElement *pElement) override;
};

#endif // __DRAWVISITOR_H__