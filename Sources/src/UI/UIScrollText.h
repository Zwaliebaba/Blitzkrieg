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
  bool OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  // text
  void SetWindowText(int nState, const wchar_t *pszText) override;
  virtual void AppendText(const wchar_t *pszText);

  // serializing...
  int operator&(IDataTree &ss) override;
  void Reposition(const CTRect<float> &rcParent) override;

  bool ProcessMessage(const SUIMessage &msg) override;

  // drawing
  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;
};

class CUIScrollTextBoxBridge : public IUIScrollTextBox, public CUIScrollTextBox
{
  OBJECT_NORMAL_METHODS(CUIScrollTextBoxBridge);
  DECLARE_SUPER(CUIScrollTextBox);
  DEFINE_UICONTAINER_BRIDGE;

  void AppendText(const wchar_t *pszText) override { CSuper::AppendText(pszText); }
};

#endif // __UI_SCROLL_TEXT_H__