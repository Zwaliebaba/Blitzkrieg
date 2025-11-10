#ifndef __UISCREEN_H__
#define __UISCREEN_H__

#include "../Input/Input.h"
#include "UIBasic.h"
#include "UIConsole.h"
#include "UIMessageBox.h"

class CUIScreen : public CMultipleWindow
{

public:
  // for chat/acks system
  struct SAcknowledgment
  {
    DECLARE_SERIALIZE;

  public:
    std::wstring szString;
    DWORD dwColor;
    NTimer::STime time;
  };

private:
  //
  std::string szResourceName;
  DWORD m_mouseState;
  DWORD m_keyboardState;
  CVec2 m_prevLButtonPos;// saved coordinate where the left mouse button was pressed
  CVec2 m_prevPrevLButtonPos;// previous saved left mouse click coordinate

  CMessageList uiMessageList;

  // for mouse wheel
  CPtr<IInputSlider> pMouseWheelSlider;

  using CListOfAcks = std::list<SAcknowledgment>;
  CListOfAcks listOfAcks;

  // chat message
  std::wstring szChatMessage;
  std::wstring szLastChatMessage;
  int nNumChatDublicates;
  bool bChatMode;
  bool bMessagesToEveryone;
  int nCursorPos;

  // Message Box
  CPtr<CUIMessageBoxBridge> pMessageBox;

public:
  CUIScreen();

  int STDCALL operator&(interface IStructureSaver &ss) override;
  int STDCALL operator&(IDataTree &ss) override;

  //
  virtual int STDCALL Load(const char *pszResourceName, bool bRelative = true);
  bool STDCALL Update(const NTimer::STime &currTime) override;
  void STDCALL Draw(interface IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  bool STDCALL IsInside(const CVec2 &_vPos) override { return IsInsideChild(_vPos); }
  virtual bool STDCALL IsEmpty() { return CMultipleWindow::IsEmpty(); }
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  // overloading event processing
  bool STDCALL OnLButtonDblClk(const CVec2 &vPos) override;
  bool STDCALL OnMouseMove(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
  virtual void STDCALL ProcessGameMessage(const SGameMessage &msg);
  virtual bool STDCALL GetMessage(SGameMessage *pMsg);

  virtual int STDCALL MessageBox(const WORD *pszText, int nType);

  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  virtual void STDCALL ClearStrings() { listOfAcks.clear(); }
  void UpdateChatString(int nAsciiCode, int nVirtualKey, bool bPressed);
};

class CUIScreenBridge : public IUIScreen, public CUIScreen
{
  OBJECT_NORMAL_METHODS(CUIScreenBridge);
  DECLARE_SUPER(CUIScreen);
  DEFINE_UISCREEN_BRIDGE;
  bool STDCALL IsEmpty() override { return CSuper::IsEmpty(); }
  int STDCALL MessageBox(const WORD *pszText, int nType) override { return CSuper::MessageBox(pszText, nType); }
};

#endif // __UISCREEN_H__