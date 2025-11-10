#ifndef __UI_EDIT_BOX_H__
#define __UI_EDIT_BOX_H__

#include "UIBasic.h"

class CUIEditBox : public CSimpleWindow
{
  DECLARE_SERIALIZE;
  int nCursorPos;// cursor position in the currently edited line
  bool bShowCursor;// to blink the cursor
  bool bFocused;// to display the cursor
  DWORD dwLastCursorAnimatedTime;// for cursor animation
  int m_nBeginSel;// beginning of selection
  int m_nEndSel;// end of selection
  DWORD dwSelColor;// color for selected text
  int m_nBeginDragSel;// start of mouse selection
  bool bNumericMode;// only numbers are entered
  bool bGameSpySymbols;// character limit
  bool bLocalPlayerNameMode;// local player's name allowed symbols
  bool bFileNameSymbols;// characters available for filename
  int nMaxLength;// if this variable is set, then the limit on the number of characters in the text is enabled

  // to scroll text left and right
  // pGFXText will store only part of the displayed string, and this variable will store the entire text
  std::wstring wszFullText;
  int nBeginText;// from this position the text display szFullText begins
  bool bTextScroll;// if this variable is set, you can enter text wider than the edit box

  bool IsValidSymbol(int nAsciiCode);

public:
  CUIEditBox() : nCursorPos(0), bShowCursor(false), bFocused(false), dwLastCursorAnimatedTime(0), m_nBeginSel(-1), m_nEndSel(-1), dwSelColor(0xff2e401b), m_nBeginDragSel(-1),
                 bNumericMode(false), bGameSpySymbols(false), bLocalPlayerNameMode(false), bFileNameSymbols(false), nMaxLength(-1), nBeginText(0), bTextScroll(false) {}

  ~CUIEditBox() override {}

  // mouse actions
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  // virtual bool STDCALL OnLButtonUp( const CVec2 &vPos, EMouseState mouseState ) { return true; 

  //
  void STDCALL SetWindowText(int nState, const WORD *pszText) override;
  void STDCALL SetFocus(bool bFocus) override;
  virtual void STDCALL SetCursor(int nPos);
  virtual int STDCALL GetCursor() { return nCursorPos; }
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;
  bool STDCALL Update(const NTimer::STime &currTime) override;

  virtual void STDCALL SetSel(int nBegin, int nEnd)
  {
    m_nBeginSel = nBegin;
    m_nEndSel = nEnd;
  }

  virtual void STDCALL GetSel(int *nBegin, int *nEnd)
  {
    *nBegin = m_nBeginSel;
    *nEnd = m_nEndSel;
  }

  virtual void STDCALL SetMaxLength(const int nLength) { nMaxLength = nLength; }

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;
  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

private:
  bool DeleteSelection();
  int GetSelection(int nX);
  // sending a message to the top about a change in the current position
  void NotifyTextChanged();
  void EnsureCursorVisible();
  bool IsTextInsideEditBox();
};

class CUIEditBoxBridge : public IUIEditBox, public CUIEditBox
{
  OBJECT_NORMAL_METHODS(CUIEditBoxBridge);
  DECLARE_SUPER(CUIEditBox);
  DEFINE_UIELEMENT_BRIDGE;

  void STDCALL SetCursor(int nPos) override { CSuper::SetCursor(nPos); }
  int STDCALL GetCursor() override { return CSuper::GetCursor(); }
  void STDCALL SetSel(int nBegin, int nEnd) override { CSuper::SetSel(nBegin, nEnd); }
  void STDCALL GetSel(int *nBegin, int *nEnd) override { CSuper::GetSel(nBegin, nEnd); }
  void STDCALL SetMaxLength(const int nLength) override { CSuper::SetMaxLength(nLength); }
};

#endif // __UI_EDIT_BOX_H__