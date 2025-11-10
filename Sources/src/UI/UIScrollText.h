#ifndef __UI_SCROLL_TEXT_H__
#define __UI_SCROLL_TEXT_H__

#include "UIBasic.h"
#include "UISlider.h"

class CUIScrollTextBox : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  CUIScrollBar *pScrollBar;// initialized at boot time and used to speed up access to components
  int nScrollBarWidth;
  int m_nY;// Y shift from the beginning of the text so that the text scrolls
  int nLeftSpace, nRightSpace;// text indentation to the left of the edge and to the right of the scrollbar, respectively
  int nTopSpace, nBottomSpace;// text indentation above and below, respectively
  bool bScrollBarAlwaysVisible;

protected:
  int GetY() const { return m_nY; }
  void GetBorderRect(CTRect<float> *pBorderRect) const;
  void RepositionScrollbar();
  virtual void RepositionText();
  void UpdateScrollBar(int nMaxValue, int nCurValue);

public:
  CUIScrollTextBox() : nScrollBarWidth(30), m_nY(0), nLeftSpace(4), nRightSpace(4), nTopSpace(0), nBottomSpace(0), bScrollBarAlwaysVisible(true) {}
  ~CUIScrollTextBox() override {}

  // mouse wheel
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  // text
  void STDCALL SetWindowText(int nState, const WORD *pszText) override;
  virtual void STDCALL AppendText(const WORD *pszText);

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
};

class CUIScrollTextBoxBridge : public IUIScrollTextBox, public CUIScrollTextBox
{
  OBJECT_NORMAL_METHODS(CUIScrollTextBoxBridge);
  DECLARE_SUPER(CUIScrollTextBox);
  DEFINE_UICONTAINER_BRIDGE;

  void STDCALL AppendText(const WORD *pszText) override { CSuper::AppendText(pszText); }
};

#endif // __UI_SCROLL_TEXT_H__