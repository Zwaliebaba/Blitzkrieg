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
  void STDCALL AddIcon(int nType, interface ISceneIcon *pIcon) override;
  void STDCALL RemoveIcon(int nType) override;
  void STDCALL UpdateHP(float fValue) override;
  void STDCALL RemoveUnit() override;
  IMOUnit * STDCALL GetMOUnit() override { return pMOUnit; }
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
  int STDCALL operator&(IDataTree &ss) override;

  // update
  bool STDCALL Update(const NTimer::STime &currTime) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  // drawing
  void STDCALL Draw(interface IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  // CRAP{
  void STDCALL SetWindowTexture(IGFXTexture *pTexture) override;
  IGFXTexture * STDCALL GetWindowTexture() override { return nullptr; }
  void STDCALL SetWindowMap(const CTRect<float> &maps) override;
  void STDCALL SetWindowPlacement(const CVec2 *vPos, const CVec2 *vSize) override;
  void STDCALL SetWindowID(int nID) override;
  void STDCALL SetBoundRect(const CTRect<float> &rc) override;
  // CRAP}

  // cursor and actions
  bool STDCALL OnLButtonDblClk(const CVec2 &vPos) override;
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override;
  bool STDCALL IsInside(const CVec2 &vPos) override;
  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  void STDCALL SetParent(interface IUIContainer *pParent) override;
  IUIContainer * STDCALL GetParent() override;
  bool STDCALL IsModal() const override { return false; }


  // text functions
  void STDCALL SetWindowText(int nState, const WORD *pszText) override;
  const WORD * STDCALL GetWindowText(int nState) override;
  void STDCALL SetTextColor(DWORD dwColor) override;

  // tool tip functions
  interface IText * STDCALL GetHelpContext(const CVec2 &vPos, CTRect<float> *pRect) override;
  void STDCALL SetHelpContext(int nState, const WORD *pszToolTipText) override;

  // state
  void STDCALL SetFocus(bool bFocus) override;
  void STDCALL EnableWindow(bool bEnable) override;
  bool STDCALL IsWindowEnabled() override;
  void STDCALL SetState(int nState, bool bNotify) override;
  int STDCALL GetState() override;
  bool STDCALL IsVisible() override;
  int STDCALL GetVisibleState() override;
  void STDCALL ShowWindow(int nCmdShow) override;
  int STDCALL GetWindowID() override;

  // return position flags
  int STDCALL GetWindowPlacement(CVec2 *pPos, CVec2 *pSize, CTRect<float> *pScreenRect) override;
  int STDCALL GetPositionFlag() override;

  // msg processing
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
  // pick the element over screen coordinates, nRecursion is a number of recursion function calls
  IUIElement * STDCALL PickElement(const CVec2 &vPos, int nRecursion) override;
  // get manipulator for editor functionality
  IManipulator * STDCALL GetManipulator() override;
  void STDCALL GetTextSize(const int nState, int *pSizeX, int *pSizeY) const override {}

  //
  //
  //
  virtual void STDCALL AddPassanger(IUnitStateObserver *pObserver);
  virtual void STDCALL RemovePassanger(IUnitStateObserver *pObserver);
  virtual int STDCALL GetPassangerCount();
  //
  virtual void STDCALL SetSquadIcon(IGFXTexture *pTexSquadIcon) { pSquadIcon = pTexSquadIcon; }
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
  void STDCALL AddRef(int nRef = 1, int nMask = 0x7fffffff) override {}
  void STDCALL Release(int nRef = 1, int nMask = 0x7fffffff) override {}
  bool STDCALL IsValid() const override { return true; }

public:
  const SSpriteInfo *pInfo;
  //
  CIconsVisitor() : pInfo(nullptr) {}
  // billboard sprite object
  void STDCALL VisitSprite(const SBasicSpriteInfo *pObj, int nType, int nPriority) override { pInfo = static_cast<const SSpriteInfo *>(pObj); }
  // mesh object
  void STDCALL VisitMeshObject(interface IMeshVisObj *pObj, int nType, int nPriority) override {}
  // particles
  void STDCALL VisitParticles(interface IParticleSource *pObj) override {}
  // unknown scene object
  void STDCALL VisitSceneObject(interface ISceneObject *pObj) override {}
  // text object
  void STDCALL VisitText(const CVec3 &vPos, const char *pszText, interface IGFXFont *pFont, DWORD color) override {}
  // bold line object
  void STDCALL VisitBoldLine(CVec3 *corners, float fWidth, DWORD color) override {}
  //
  void Clear() { pInfo = nullptr; }
  // empty
  void STDCALL VisitMechTrace(const SMechTrace &trace) override {};
  void STDCALL VisitGunTrace(const SGunTrace &trace) override {};
  // UI elements visiting
  void STDCALL VisitUIRects(interface IGFXTexture *pTexture, const int nShadingEffect, struct SGFXRect2 *rects, const int nNumRects) override {}
  void STDCALL VisitUIText(interface IGFXText *pText, const CTRect<float> &rcRect, const int nY, const DWORD dwColor, const DWORD dwFlags) override {}
  void STDCALL VisitUICustom(interface IUIElement *pElement) override {}
};

#endif // __UISQUADELEMENT_H__