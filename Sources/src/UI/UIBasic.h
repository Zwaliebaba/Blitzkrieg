#ifndef __UIBASIC_H__
#define __UIBASIC_H__

#include "UIInternal.h"
#include "../lualib/script.h"

class CSimpleWindow;

using CWindowList = std::list<CObj<IUIElement>>;

class CSimpleWindow
{
  DECLARE_SERIALIZE;

  //
  CTRect<float> wndRect;// window coordinates relative to the screen
  int nPositionFlag;// sets the anchor point
  CVec2 vPos;// coordinates of the upper left point of the window relative to the selected anchor point
  CVec2 vSize;// window dimensions
  using CStateVector = std::vector<CWindowState>;

  int nID;// unique window identifier
  CPtr<IUIContainer> pParent;// parent
  bool bWindowActive;// is the window active //??
  int nCmdShow;// window display status

  CStateVector states;// all possible states of this button, for example CheckButton has two states
  int nCurrentState;// current state
  int nCurrentSubState;// current window substate: NORMAL, HIGHLIGHTED, PUSHED
  bool bShowBackground;// display the texture or not (makes sense for windows with text only)

  CObj<IManipulator> pManipulator;
  std::string szHighSound;// sound played when the mouse is hovered over the control, perhaps they should be different for different states, I don’t know

  // for text
  int nTextAlign;
  DWORD dwTextColor;
  CVec2 vShiftText;
  int nFontSize;
  CVec2 vTextPos;
  bool bRedLine;
  bool bSingleLine;

  // for shadow
  DWORD dwShadowColor;
  CVec2 vShadowShift;
  std::string szToolKey;

  // bound rect
  CTRect<float> rcBound;
  bool bBounded;

  // for blinking a button when clicked with the mouse (the button blinks when nBlink == 1 and it has one state)
  // if it has nBlink == 2 then the button blinks regardless of the amount of state
  // if 0, then the button does not blink when pressed
  int nBlink;
  DWORD dwLastBlinkTime;
  DWORD dwCurrentBlinkColor;
  bool bBlinking;// if true then the button is now blinking
  DWORD dwBlinkTime;
  int nBlinkColorIndex;// color number (for blinking)

  void InitDependentInfo();

protected:
  // calculates new values ​​for local coordinates using global coordinates and pParent
  void UpdateLocalCoordinates();
  void SetShowBackgroundFlag(bool bFlag) { bShowBackground = bFlag; }

  void DrawBackground(IGFX *pGFX);
  void DrawText(IGFX *pGFX);
  void VisitBackground(ISceneVisitor *pVisitor);
  void VisitText(ISceneVisitor *pVisitor);
  void BlinkMe(int nBlinkTime = 0, int nBlinkColorIndex = 0);

  friend class CUIWindowManipulator;
  friend class CUIEditBox;
  friend class CUIScrollTextBox;
  friend class CUIStatic;
  friend class CUIMiniMap;
  friend class CUINumberIndicator;
  friend class CMultipleWindow;
  friend class CUIMedals;
  friend class CUIShortcutBar;
  friend class CUIComboBox;
  friend class CUIStatusBar;
  friend class CUICreditsScroller;
  friend class CUIConsole;

  void GetWindowRect(CTRect<float> *pWndRect) const { *pWndRect = wndRect; }
  int GetCmdShow() const { return nCmdShow; }

public:
  CSimpleWindow() : nPositionFlag(0x0011), vPos(0, 0), vSize(0, 0), nID(-1), bWindowActive(true), nCmdShow(UI_SW_SHOW), states(1), nCurrentState(0),
                    nCurrentSubState(0), bShowBackground(true), nTextAlign(0x0011), dwTextColor(0xff9aceb7), vShiftText(0, 0), nFontSize(1), vTextPos(0, 0), bRedLine(false), bSingleLine(false),
                    dwShadowColor(0xff000000), vShadowShift(0, 0), bBounded(false), nBlink(0), dwLastBlinkTime(0), dwCurrentBlinkColor(0xff000000), bBlinking(false), nBlinkColorIndex(0) {}

  virtual ~CSimpleWindow() {}

  // serializing...
  virtual int STDCALL operator&(IDataTree &ss);

  // update
  virtual bool STDCALL Update(const NTimer::STime &currTime);
  virtual void STDCALL Reposition(const CTRect<float> &rcParent);

  // text
  virtual void STDCALL SetWindowText(int nState, const WORD *pszText);
  virtual const WORD * STDCALL GetWindowText(int nState);
  virtual void STDCALL SetTextColor(DWORD dwColor);

  // tool tip functions
  virtual IText * STDCALL GetHelpContext(const CVec2 &vPos, CTRect<float> *pRect);
  virtual void STDCALL SetHelpContext(int nState, const WORD *pszToolTipText);

  // CRAP set texture
  virtual void STDCALL SetWindowTexture(IGFXTexture *pTexture);
  virtual IGFXTexture * STDCALL GetWindowTexture();
  virtual void STDCALL SetWindowMap(const CTRect<float> &maps);
  virtual void STDCALL SetWindowPlacement(const CVec2 *_vPos, const CVec2 *_vSize);// require to call Reposition() next
  virtual void STDCALL SetWindowID(int _nID);

  virtual void STDCALL SetBoundRect(const CTRect<float> &rc)
  {
    bBounded = true;
    rcBound = rc;
  }

  // drawing
  virtual void STDCALL Draw(IGFX *pGFX);
  virtual void STDCALL Visit(interface ISceneVisitor *pVisitor);

  // cursor and actions
  virtual bool STDCALL OnLButtonDblClk(const CVec2 &vPos);
  virtual bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState);
  virtual bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState);
  virtual bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState);
  virtual bool STDCALL OnRButtonDown(const CVec2 &vPos, EMouseState mouseState);
  virtual bool STDCALL OnRButtonUp(const CVec2 &vPos, EMouseState mouseState);
  virtual bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta);
  virtual bool STDCALL IsInside(const CVec2 &vPos);
  virtual bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) { return false; }
  virtual bool STDCALL IsModal() const { return false; }

  virtual int STDCALL GetWindowID() { return nID; }
  virtual int STDCALL GetWindowPlacement(CVec2 *pPos, CVec2 *pSize, CTRect<float> *pScreenRect);
  virtual int STDCALL GetPositionFlag() { return nPositionFlag; }
  virtual void STDCALL SetParent(interface IUIContainer *pPapa) { pParent = pPapa; }
  virtual IUIContainer * STDCALL GetParent() { return pParent; }
  // state
  virtual void STDCALL SetFocus(bool bFocus);
  virtual void STDCALL EnableWindow(bool bEnable);
  virtual bool STDCALL IsWindowEnabled() { return bWindowActive; }
  virtual void STDCALL SetState(int nState, bool bNotify);// if bNotify flag is false, then button will not send notification message
  virtual int STDCALL GetState() { return nCurrentState; }
  virtual bool STDCALL IsVisible() { return nCmdShow != UI_SW_HIDE && nCmdShow != UI_SW_MINIMIZE; }
  virtual int STDCALL GetVisibleState() { return nCmdShow; }
  virtual void STDCALL ShowWindow(int _nCmdShow);

  virtual bool STDCALL ProcessMessage(const SUIMessage &msg)
  {
    if (IsProcessedMessage(msg)) return pParent->ProcessMessage(msg);
    return false;
  }

  virtual IUIElement * STDCALL PickElement(const CVec2 &vPos, int nRecursion);
  virtual IManipulator * STDCALL GetManipulator();
  virtual void STDCALL GetTextSize(int nState, int *pSizeX, int *pSizeY) const;


  // for internal use only
  const CTRect<float> &GetScreenRect() { return wndRect; }
  void SetScreenRect(const CTRect<float> &rc) { wndRect = rc; }
  void UpdateSubRects();
  CVec2 GetSize() { return vSize; }
  void SetPos(const CVec2 &pos) { vPos = pos; }
  void SetSize(const CVec2 &size) { vSize = size; }
  void SetPositionFlag(int nFlag) { nPositionFlag = nFlag; }
  void DestroyWindow();
  int GetCurrentSubState() { return nCurrentSubState; }

  void InitText();

  float GetWidth() const { return wndRect.Width(); }
  // duplicate
  void CopyInternals(CSimpleWindow *pWnd);
};

class CMultipleWindow : public CSimpleWindow
{
  DECLARE_SERIALIZE;
  //
  //
  CWindowList childList;// child windows
  // The very first window in this list has focus and receives messages from the keyboard.
  // Windows are drawn from the end of the list to the beginning

  CPtr<IUIElement> pHighlighted;// illuminated window
  CPtr<IUIElement> pPushed;// clicked window (left button)
  CPtr<IUIElement> pRPushed;// right-click window
  CPtr<IUIElement> pFocused;// window with focus, when removing focus for an edit box, for example, TEXT_MODE is removed

  using CMessageList = std::list<SUIMessage>;
  CMessageList messageList;

  // constant for mouse wheel support
  float fMouseWheelMultiplyer;

  // LUA support
  std::string szLuaFileName;
  bool bLua;// whether LUA has initialized
  Script luaScript;
  static CMessageList staticMessageList;// this is for adding messages to the queue while the LUA script is running

  struct SLuaValue
  {
    int nID;
    int nVal;
  };

  using CLuaValues = std::vector<SLuaValue>;
  static CLuaValues staticLuaValues;

  // For sliding windows
  bool bAnimation;// if the flag is set, then the window with animation
  bool bAnimationRunning;// flag that animation is happening, useful for speed
  DWORD dwLastOpenTime;// time when the opening animation started
  DWORD dwLastCloseTime;// time when the closing animation started
  DWORD dwAnimationTime;// opening or closing animation time
  CVec2 vMinPos;
  CVec2 vMaxPos;
  CVec2 vBeginPos;
  int nAnimationCmdShow;

  // CRAP
  bool bModal;// used to ensure that all messages are sent only to the first child
  friend class CUIScrollTextBox;
  friend class CUIObjective;
  friend class CUIList;
  friend class CUIMedals;
  friend class CUIShortcutBar;
  friend class CUIObjMap;
  friend class CUIConsole;

  void InitDependentInfoMW();
  IUIElement *GetFirstModal();

protected:
  bool IsInsideChild(const CVec2 &_vPos);
  bool IsEmpty() { return childList.empty(); }

  CWindowList &GetChildList() { return childList; }

public:
  CMultipleWindow() : fMouseWheelMultiplyer(4.375f), bLua(false), bAnimation(false), bAnimationRunning(false), dwLastOpenTime(0),
                      dwLastCloseTime(0), dwAnimationTime(200), nAnimationCmdShow(0), bModal(false) {}

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // update
  bool STDCALL Update(const NTimer::STime &currTime) override;
  void STDCALL SetFocus(bool bFocus) override;
  virtual void STDCALL SetFocusedWindow(IUIElement *pNewFocusWindow);
  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  void STDCALL EnableWindow(bool bEnable) override;

  // tool tip functions
  IText * STDCALL GetHelpContext(const CVec2 &vPos, CTRect<float> *pRect) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  // cursor and actions
  bool STDCALL OnLButtonDblClk(const CVec2 &vPos) override;
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override;
  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;

  virtual void STDCALL AddChild(IUIElement *pSimple)
  {
    childList.push_front(pSimple);
    auto pContainer = dynamic_cast<IUIContainer *>(this);
    pSimple->SetParent(pContainer);
  }

  virtual void STDCALL RemoveChild(IUIElement *pSimple) { childList.remove(pSimple); }
  virtual void STDCALL RemoveAllChildren() { childList.clear(); }
  virtual IUIElement * STDCALL GetChildByID(int nChildID);
  IUIElement *GetChildByIndex(int nIndex);
  bool STDCALL IsModal() const override { return bModal; }

  virtual void STDCALL MoveWindowUp(IUIElement *pWnd);
  virtual void STDCALL MoveWindowDown(IUIElement *pWnd);
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
  void STDCALL ShowWindow(int _nCmdShow) override;
  void STDCALL SetBoundRect(const CTRect<float> &rc) override;

  IUIElement * STDCALL PickElement(const CVec2 &vPos, int nRecursion) override;

  // For LUA work
  static int AddMessage(lua_State *pLuaState);// called from script
  static int SaveLuaValue(lua_State *pLuaState);// called from script
  static int IsGameButtonProcessing(lua_State *pLuaState);// called from script

  // for indoor use
  void SetModalFlag(bool bFlag) { bModal = bFlag; }
  bool GetModalFlag() { return bModal; }
  void SetMouseWheelMultiplyer(float fVal) { fMouseWheelMultiplyer = fVal; }
  float GetMouseWheelMultiplyer() { return fMouseWheelMultiplyer; }

  // duplication
  void CopyInternals(CMultipleWindow *pWnd);
};

#endif // __UIBASIC_H__