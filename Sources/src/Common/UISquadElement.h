#ifndef __UISQUADELEMENT_H__
#define __UISQUADELEMENT_H__

#pragma once

#include "../UI/UI.h"
#include "MapObject.h"

class CUISquadElement;

// ************************************************************************************************************************ //
// **
// ** icon update subelement
// **
// **
// **
// ************************************************************************************************************************ //

struct SIconDesc
{
  int nType;// icon type
  CPtr<ISceneIcon> pIcon;// icon itself
  //
  SIconDesc() {}

  SIconDesc(const int _nType, ISceneIcon *_pIcon)
    : nType(_nType), pIcon(_pIcon) {}

  //
  int operator&(IStructureSaver &ss)
  {
    CSaverAccessor saver = &ss;
    saver.Add(1, &nType);
    saver.Add(2, &pIcon);
    return 0;
  }
};

class CUIUnitObserver : public CTRefCount<IUnitStateObserver>
{
  OBJECT_SERVICE_METHODS(CUIUnitObserver);
  DECLARE_SERIALIZE;
  //
  float fHP;// HP of this passanger
  std::list<SIconDesc> icons;// icons of this passanger
  CPtr<CUISquadElement> pSquad;// squad to report about unit changes
  CPtr<IMOUnit> pMOUnit;// corresponding map object
public:
  CUIUnitObserver() : fHP(0) {}
  CUIUnitObserver(IMOUnit *_pMOUnit) : fHP(0) { pMOUnit = _pMOUnit; }
  //
  void AddIcon(int nType, interface ISceneIcon *pIcon) override;
  void RemoveIcon(int nType) override;
  void UpdateHP(float fValue) override;
  void RemoveUnit() override;
  IMOUnit * GetMOUnit() override { return pMOUnit; }
  //
  void SetSquad(CUISquadElement *_pSquad) { pSquad = _pSquad; }
  //
  const std::list<SIconDesc> &GetIcons() const { return icons; }
  const float GetHP() const { return fHP; }
};

// ************************************************************************************************************************ //
// **
// ** squad UI element (who-in-container interface)
// **
// **
// **
// ************************************************************************************************************************ //

class CUISquadElement : public CTRefCount<IUIElement>
{
  OBJECT_SERVICE_METHODS(CUISquadElement);
  DECLARE_SERIALIZE;
  //
  CTRect<float> rcWindow;// window coordinates relative to the screen
  int nPositionFlag;// sets the anchor point
  CVec2 vPos;// coordinates of the upper left point of the window relative to the selected anchor point
  CVec2 vSize;// window dimensions

  int nID;// unique window identifier
  CPtr<IUIContainer> pParent;// parent
  bool bWindowActive;// is the window active //??
  int nCmdShow;// window display status
  bool bSelected;// is this squad selected inside the container?
  //
  using CPassangersList = std::list<CObj<CUIUnitObserver>>;
  CPassangersList passangers;// container passengers
  CPtr<IGFXTexture> pSquadIcon;// general squad icon
public:
  CUISquadElement();
  // serializing...
  int operator&(IDataTree &ss) override;

  // update
  bool Update(const NTimer::STime &currTime) override;
  void Reposition(const CTRect<float> &rcParent) override;

  // drawing
  void Draw(interface IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;

  // CRAP{
  void SetWindowTexture(IGFXTexture *pTexture) override;
  IGFXTexture * GetWindowTexture() override { return nullptr; }
  void SetWindowMap(const CTRect<float> &maps) override;
  void SetWindowPlacement(const CVec2 *vPos, const CVec2 *vSize) override;
  void SetWindowID(int nID) override;
  void SetBoundRect(const CTRect<float> &rc) override;
  // CRAP}

  // cursor and actions
  bool OnLButtonDblClk(const CVec2 &vPos) override;
  bool OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override;
  bool IsInside(const CVec2 &vPos) override;
  bool OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  void SetParent(interface IUIContainer *pParent) override;
  IUIContainer * GetParent() override;
  bool IsModal() const override { return false; }


  // text functions
  void SetWindowText(int nState, const wchar_t *pszText) override;
  const wchar_t *GetWindowText(int nState) override;
  void SetTextColor(DWORD dwColor) override;

  // tool tip functions
  interface IText * GetHelpContext(const CVec2 &vPos, CTRect<float> *pRect) override;
  void SetHelpContext(int nState, const wchar_t *pszToolTipText) override;

  // state
  void SetFocus(bool bFocus) override;
  void EnableWindow(bool bEnable) override;
  bool IsWindowEnabled() override;
  void SetState(int nState, bool bNotify) override;
  int GetState() override;
  bool IsVisible() override;
  int GetVisibleState() override;
  void ShowWindow(int nCmdShow) override;
  int GetWindowID() override;

  // return position flags
  int GetWindowPlacement(CVec2 *pPos, CVec2 *pSize, CTRect<float> *pScreenRect) override;
  int GetPositionFlag() override;

  // msg processing
  bool ProcessMessage(const SUIMessage &msg) override;
  // pick the element over screen coordinates, nRecursion is a number of recursion function calls
  IUIElement * PickElement(const CVec2 &vPos, int nRecursion) override;
  // get manipulator for editor functionality
  IManipulator * GetManipulator() override;
  void GetTextSize(const int nState, int *pSizeX, int *pSizeY) const override {}

  //
  //
  //
  virtual void AddPassanger(IUnitStateObserver *pObserver);
  virtual void RemovePassanger(IUnitStateObserver *pObserver);
  virtual int GetPassangerCount();
  //
  virtual void SetSquadIcon(IGFXTexture *pTexSquadIcon) { pSquadIcon = pTexSquadIcon; }
};

// ************************************************************************************************************************ //
// **
// ** icons visitor
// **
// **
// **
// ************************************************************************************************************************ //

class CIconsVisitor : public ISceneVisitor
{
  // refcounting
  void AddRef(int nRef = 1, int nMask = 0x7fffffff) override {}
  void Release(int nRef = 1, int nMask = 0x7fffffff) override {}
  bool IsValid() const override { return true; }

public:
  const SSpriteInfo *pInfo;
  //
  CIconsVisitor() : pInfo(nullptr) {}
  // billboard sprite object
  void VisitSprite(const SBasicSpriteInfo *pObj, int nType, int nPriority) override { pInfo = static_cast<const SSpriteInfo *>(pObj); }
  // mesh object
  void VisitMeshObject(interface IMeshVisObj *pObj, int nType, int nPriority) override {}
  // particles
  void VisitParticles(interface IParticleSource *pObj) override {}
  // unknown scene object
  void VisitSceneObject(interface ISceneObject *pObj) override {}
  // text object
  void VisitText(const CVec3 &vPos, const char *pszText, interface IGFXFont *pFont, DWORD color) override {}
  // bold line object
  void VisitBoldLine(CVec3 *corners, float fWidth, DWORD color) override {}
  //
  void Clear() { pInfo = nullptr; }
  // empty
  void VisitMechTrace(const SMechTrace &trace) override {};
  void VisitGunTrace(const SGunTrace &trace) override {};
  // UI elements visiting
  void VisitUIRects(interface IGFXTexture *pTexture, const int nShadingEffect, struct SGFXRect2 *rects, const int nNumRects) override {}
  void VisitUIText(interface IGFXText *pText, const CTRect<float> &rcRect, const int nY, const DWORD dwColor, const DWORD dwFlags) override {}
  void VisitUICustom(interface IUIElement *pElement) override {}
};

#endif // __UISQUADELEMENT_H__