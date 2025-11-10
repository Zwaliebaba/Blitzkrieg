#ifndef __UI_CONSOLE_H__
#define __UI_CONSOLE_H__

#include "UIBasic.h"

class CUIConsole : public CSimpleWindow
{
  DECLARE_SERIALIZE;

public:
  struct SColorString
  {
    DECLARE_SERIALIZE;

  public:
    std::wstring szString;
    DWORD dwColor;

    SColorString() : dwColor(0xffffffff) {}
    SColorString(const wchar_t *pszStr, DWORD col) : szString(pszStr), dwColor(col) {}
    SColorString(const std::wstring &szStr, DWORD col) : szString(szStr), dwColor(col) {}
    int operator&(IDataTree &ss);
  };

private:
  using CVectorOfStrings = std::vector<std::wstring>;
  using CVectorOfColorStrings = std::vector<SColorString>;
  CVectorOfColorStrings vectorOfStrings;// all lines in the console
  CVectorOfStrings vectorOfCommands;// executed commands in the console, to select previous commands using the up/down arrows

  DWORD dwLastOpenTime;// time when the console opening animation started
  DWORD dwLastCloseTime;// time when the console closing animation started
  bool bAnimation;// flag that animation is happening, useful for speed
  int nCursorPos;// cursor position in the currently edited line
  int nBeginString;// initial display string from a list of strings
  // 0 is considered the most recent line
  int nBeginCommand;// current command from the command log
  DWORD m_dwColor;
  bool bShowCursor;// Is the cursor currently visible?
  DWORD dwLastCursorAnimatedTime;// for cursor animation

  std::wstring szEditString;// current edited line
  // to process commands
  using CCommandsList = std::list<CPtr<IConsoleCommandHandler>>;
  CCommandsList commandsChain;
  Script consoleScript;// console script

  // this case is called after reading a new command from the buffer
  void ParseCommand(const std::wstring &szCommand);
  void InitConsoleScript();
  bool RunScriptFile(const std::string &szScriptFileName);
  using CConsoleFunctions = std::hash_map<std::string, int>;
  CConsoleFunctions consoleFunctions;

public:
  CUIConsole();
  ~CUIConsole() override {}

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  bool STDCALL IsVisible() override;
  void STDCALL ShowWindow(int _nCmdShow) override;
  virtual bool STDCALL IsAnimationStage() { return bAnimation; }

  // the console always takes up the entire width of the screen
  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  // this is needed for console animation, collapsing and expanding
  bool STDCALL Update(const NTimer::STime &currTime) override;
  // in addition to drawing a window, lines should be displayed here
  void STDCALL Draw(interface IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;

  virtual void STDCALL RegisterCommand(IConsoleCommandHandler *pHandler);
};

class CUIConsoleBridge : public IUIConsole, public CUIConsole
{
  OBJECT_NORMAL_METHODS(CUIConsoleBridge);
  DECLARE_SUPER(CUIConsole);
  DEFINE_UIELEMENT_BRIDGE;

  void STDCALL RegisterCommand(IConsoleCommandHandler *pHandler) override { CSuper::RegisterCommand(pHandler); }
  bool STDCALL IsAnimationStage() override { return CSuper::IsAnimationStage(); }
};

#endif		// __UI_CONSOLE_H__