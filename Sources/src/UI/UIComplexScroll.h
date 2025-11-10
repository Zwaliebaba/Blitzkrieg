#ifndef __UICOMPLEXSCROLL_H__
#define __UICOMPLEXSCROLL_H__

#pragma once

#include "UIBasic.h"

class CUIComplexScroll : public CMultipleWindow
{
  DECLARE_SERIALIZE;

  class CUIScrollBar *pScrollBar;// initialized at boot time and used to speed up access to components
  struct IUIDialog *pItemContainer;

  int nCurrentPosToAdd;// position to add new element to

  int nScrollBarWidth;
  int m_nY;// Y shift from the beginning of the text so that the text scrolls
  int nLeftSpace, nRightSpace;// text indentation to the left of the edge and to the right of the scrollbar, respectively
  int nTopSpace, nBottomSpace;// text indentation above and below, respectively
  bool bScrollBarAlwaysVisible;
  int nVSubSpace;// subspace between elements

  void UpdateScrollBar(int nMaxValue, int nCurValue);
  void GetBorderRect(CTRect<float> *pBorderRect) const;
  void RepositionScrollbar();
  void UpdatePosition();

public:
  CUIComplexScroll();
  ~CUIComplexScroll() override {}

  int STDCALL operator&(IDataTree &ss) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

  // mouse wheel
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  // adding item window
  virtual void STDCALL AddItem(IUIElement *pElement, bool bResizeToFitText);
  virtual void STDCALL Clear();
};

class CUIComplexScrollBridge : public IUIComplexScroll, public CUIComplexScroll
{
  OBJECT_NORMAL_METHODS(CUIComplexScrollBridge);
  DECLARE_SUPER(CUIComplexScroll);
  DEFINE_UICONTAINER_BRIDGE;

  void STDCALL AddItem(IUIElement *pElement, const bool bResizeToFitText) override { CSuper::AddItem(pElement, bResizeToFitText); }
  void STDCALL Clear() override { CSuper::Clear(); }
};

#endif // __UICOMPLEXSCROLL_H__