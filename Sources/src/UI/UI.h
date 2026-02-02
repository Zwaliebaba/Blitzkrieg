#pragma once

#include "TextSystem.h"

#undef GetMessage
#undef GetWindowText
#undef SetWindowText
#undef MessageBox

interface IGFXTexture;

enum
{
  UI_BASE_VALUE = 0x10000000,
  UI_UI = UI_BASE_VALUE + 0x00001100 + 1,
  UI_SCREEN = UI_BASE_VALUE + 0x00001100 + 2,
  UI_BUTTON = UI_BASE_VALUE + 0x00001100 + 3,
  UI_CONSOLE = UI_BASE_VALUE + 0x00001100 + 4,
  UI_STATIC = UI_BASE_VALUE + 0x00001100 + 5,
  UI_STATUS_BAR = UI_BASE_VALUE + 0x00001100 + 6,
  UI_DIALOG = UI_BASE_VALUE + 0x00001100 + 7,
  UI_SLIDER = UI_BASE_VALUE + 0x00001100 + 8,
  UI_SCROLLBAR = UI_BASE_VALUE + 0x00001100 + 9,
  UI_LIST = UI_BASE_VALUE + 0x00001100 + 10,
  UI_EDIT_BOX = UI_BASE_VALUE + 0x00001100 + 11,
  UI_MESSAGE_BOX = UI_BASE_VALUE + 0x00001100 + 12,
  UI_SCROLL_TEXT = UI_BASE_VALUE + 0x00001100 + 13,
  UI_MINIMAP = UI_BASE_VALUE + 0x00001100 + 14,
  UI_OBJECTIVE_SCREEN = UI_BASE_VALUE + 0x00001100 + 15,
  UI_OBJECTIVE = UI_BASE_VALUE + 0x00001100 + 16,
  UI_NUMBER_INDICATOR = UI_BASE_VALUE + 0x00001100 + 17,
  UI_TIME_COUNTER = UI_BASE_VALUE + 0x00001100 + 18,
  UI_OBJ_MAP = UI_BASE_VALUE + 0x00001100 + 19,
  UI_MEDALS = UI_BASE_VALUE + 0x00001100 + 20,
  UI_TREE = UI_BASE_VALUE + 0x00001100 + 21,
  UI_SHORTCUT_BAR = UI_BASE_VALUE + 0x00001100 + 22,
  UI_COMBOBOX = UI_BASE_VALUE + 0x00001100 + 23,
  UI_VIDEO_BUTTON = UI_BASE_VALUE + 0x00001100 + 24,
  UI_CREDITS_SCROLL = UI_BASE_VALUE + 0x00001100 + 25,
  UI_COLOR_TEXT_SCROLL = UI_BASE_VALUE + 0x00001100 + 26,
  UI_COLOR_TEXT_ENTRY = UI_BASE_VALUE + 0x00001100 + 27,// CANNOT BE IN xml
  UI_COMPLEX_TEXT_SCROLL = UI_BASE_VALUE + 0x00001100 + 28,

  UI_FORCE_DWORD = 0x7fffffff
};

enum EMouseState
{
  E_MOUSE_FREE = 0,
  E_LBUTTONDOWN = 1,
  E_RBUTTONDOWN = 2,
  E_CBUTTONDOWN = 4
};

enum EKeyboardState
{
  E_KEYBOARD_FREE = 0,
  E_SHIFT_KEY_DOWN = 1,
  E_ALT_KEY_DOWN = 2,
  E_CTRL_KEY_DOWN = 4
};

// Window can be attached to following points of parent window
enum
{
  UIPLACE_LEFT = 0x0001,
  UIPLACE_HMID = 0x0002,
  UIPLACE_RIGHT = 0x0003,
  UIPLACE_TOP = 0x0010,
  UIPLACE_VMID = 0x0020,
  UIPLACE_BOTTOM = 0x0030
};

enum
{
  UI_SW_HIDE = 0,
  UI_SW_SHOW = 1,// show window and move it to the end of drawing order (the window will be topmost)
  UI_SW_LAST = 2,// show window and move it to the begin of drawing order (the other windows will be up to it)
  UI_SW_MINIMIZE = 0x10,// minimize window
  UI_SW_MAXIMIZE = 0x11,// maximize window
  UI_SW_SHOW_DONT_MOVE_UP = 0x100,
  UI_SW_SHOW_MODAL = 0x200,
  UI_SW_HIDE_MODAL = 0x300,

};

struct SUIMessage
{
  int nMessageCode;// message code, see file UIMessages.h
  int nFirst;// first parameter, depends upon message code
  int nSecond;// second parameter, depends upon message code
};

constexpr int NOTIFY_PARENT_FLAG = 0x10000000;
inline bool IsNotifyParentMessage(int nMessageCode) { return nMessageCode & NOTIFY_PARENT_FLAG; }
inline bool IsNotifyParentMessage(const SUIMessage &msg) { return msg.nMessageCode & NOTIFY_PARENT_FLAG; }

constexpr int NOTIFY_FLAG = 0x20000000;
inline bool IsNotifyMessage(int nMessageCode) { return nMessageCode & NOTIFY_FLAG; }
inline bool IsNotifyMessage(const SUIMessage &msg) { return msg.nMessageCode & NOTIFY_FLAG; }

constexpr int PROCESSED_FLAG = 0x40000000;
inline bool IsProcessedMessage(int nMessageCode) { return nMessageCode & PROCESSED_FLAG; }
inline bool IsProcessedMessage(const SUIMessage &msg) { return msg.nMessageCode & PROCESSED_FLAG; }

interface IUIElement : IRefCount
{
  // serializing...
  virtual int operator&(IDataTree &ss) = 0;

  // update
  virtual bool Update(const NTimer::STime &currTime) = 0;
  virtual void Reposition(const CTRect<float> &rcParent) = 0;

  // drawing
  virtual void Draw(interface IGFX *pGFX) = 0;
  virtual void Visit(interface ISceneVisitor *pVisitor) = 0;

  virtual void SetWindowTexture(IGFXTexture *pTexture) = 0;
  virtual IGFXTexture * GetWindowTexture() = 0;
  virtual void SetWindowMap(const CTRect<float> &maps) = 0;
  virtual void SetWindowPlacement(const CVec2 *vPos, const CVec2 *vSize) = 0;
  virtual void SetWindowID(int nID) = 0;
  virtual void SetBoundRect(const CTRect<float> &rc) = 0;

  // cursor and actions
  virtual bool OnLButtonDblClk(const CVec2 &vPos) = 0;
  virtual bool OnMouseMove(const CVec2 &vPos, EMouseState mouseState) = 0;
  virtual bool OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) = 0;
  virtual bool OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) = 0;
  virtual bool OnRButtonDown(const CVec2 &vPos, EMouseState mouseState) = 0;
  virtual bool OnRButtonUp(const CVec2 &vPos, EMouseState mouseState) = 0;
  virtual bool OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) = 0;
  virtual bool IsInside(const CVec2 &vPos) = 0;
  virtual bool OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) = 0;
  virtual void SetParent(interface IUIContainer *pParent) = 0;
  virtual IUIContainer * GetParent() = 0;

  // text functions
  virtual void SetWindowText(int nState, const wchar_t *pszText) = 0;
  virtual const wchar_t * GetWindowText(int nState) = 0;
  virtual void SetTextColor(DWORD dwColor) = 0;

  // tool tip functions
  virtual IText * GetHelpContext(const CVec2 &vPos, CTRect<float> *pRect) = 0;
  virtual void SetHelpContext(int nState, const wchar_t *pszToolTipText) = 0;

  // state
  virtual void SetFocus(bool bFocus) = 0;
  virtual void EnableWindow(bool bEnable) = 0;
  virtual bool IsWindowEnabled() = 0;
  virtual bool IsModal() const = 0;
  virtual void SetState(int nState, bool bNotify = true) = 0;
  virtual int GetState() = 0;
  virtual bool IsVisible() = 0;
  virtual int GetVisibleState() = 0;// see enum UI_SW_SHOW .. UI_SW_MAXIMIZE
  virtual void ShowWindow(int nCmdShow) = 0;
  virtual int GetWindowID() = 0;

  // return position flags
  virtual int GetWindowPlacement(CVec2 *pPos, CVec2 *pSize, CTRect<float> *pScreenRect) = 0;
  virtual int GetPositionFlag() = 0;

  // msg processing
  virtual bool ProcessMessage(const SUIMessage &msg) = 0;
  // pick the element over screen coordinates, nRecursion is a number of recursion function calls
  virtual IUIElement * PickElement(const CVec2 &vPos, int nRecursion) = 0;
  // get manipulator for editor functionality
  virtual IManipulator * GetManipulator() = 0;

  virtual void GetTextSize(int nState, int *pSizeX, int *pSizeY) const = 0;

  // creation/copy
  virtual IUIElement * Duplicate()
  {
    NI_ASSERT_T(false, "wrong call");
    return nullptr;
  }
};

interface IUIContainer : IUIElement
{
  // Add and remove child window
  virtual void AddChild(IUIElement *pWnd) = 0;
  virtual void RemoveChild(IUIElement *pWnd) = 0;
  virtual void RemoveAllChildren() = 0;
  // return the window by id, if it is inside children, else return 0
  virtual IUIElement * GetChildByID(int nChildID) = 0;
  // this window will be pushed on top of window messaging service
  virtual void MoveWindowUp(IUIElement *pWnd) = 0;
  // this window will be pushed on the bottom of window messaging service
  virtual void MoveWindowDown(IUIElement *pWnd) = 0;
  // if modal flag is set then messages will be transmeted only to first child window
  virtual void SetModalFlag(bool bFlag) = 0;
  // recursive function to change focused window
  virtual void SetFocusedWindow(IUIElement *pNewFocusWindow) = 0;
};

enum EMessageBoxType
{
  E_MESSAGE_BOX_TYPE_OK,
  E_MESSAGE_BOX_TYPE_OKCANCEL,
  E_MESSAGE_BOX_TYPE_YESNO,
};

enum EMessageBoxReturnValues
{
  E_MESSAGE_BOX_RETURN_OK,
  E_MESSAGE_BOX_RETURN_CANCEL,
  E_MESSAGE_BOX_RETURN_YES,
  E_MESSAGE_BOX_RETURN_NO,
};

interface IUIScreen : IUIContainer
{
  virtual bool IsEmpty() = 0;
  // Load screen from resource, if bRelative is false, then ResourceName is full file name. 
  virtual int Load(const char *pszResourceName, bool bRelative = true) = 0;
  virtual void ProcessGameMessage(const SGameMessage &msg) = 0;
  virtual bool GetMessage(SGameMessage *pMsg) = 0;// return false when no messages available
  // clear all chat strings
  virtual void ClearStrings() = 0;
  // show message box
  virtual int MessageBox(const wchar_t *pszText, int nType) = 0;
};

// this is an element of the 'chain-of-responsibility' pattern for the commands processing inside the console
interface IConsoleCommandHandler : IRefCount
{
  virtual bool HandleCommand(const char *pszCommandString) = 0;
};

interface IUIConsole : IUIElement
{
  virtual bool IsAnimationStage() = 0;

  // register command pHandler with name pszName and parameter nCommandId
  virtual void RegisterCommand(IConsoleCommandHandler *pHandler) = 0;
};

// dialog is a control that contains children. 
interface IUIDialog : IUIContainer {};

// static is a simple control with text and icon
interface IUIStatic : IUIElement {};

// button is a control that supports process messages mechanism
interface IUIButton : IUIElement {};

// scrolling text for credits
interface IUICreditsScroller : IUIElement {};

// EditBox allow user to enter text
interface IUIEditBox : IUIElement
{
  virtual void SetCursor(int nPos) = 0;
  virtual int GetCursor() = 0;
  virtual void SetSel(int nBegin, int nEnd) = 0;
  virtual void GetSel(int *nBegin, int *nEnd) = 0;// -1 if no selection
  virtual void SetMaxLength(int nLength) = 0;
};

interface IUINumberIndicator : IUIElement
{
  virtual void SetValue(float fVal) = 0;
  virtual void ClearColors() = 0;
  virtual void SetColor(float fVal, DWORD dwColor) = 0;
};

interface IUIStatusBar : IUIContainer
{
  virtual void OutputString(int nControl, const wchar_t *pszText) = 0;
  virtual void OutputValue(int nControl, float fVal) = 0;
  virtual void SetUnitProperty(int nPropType, int nPropValue, const wchar_t *pszToolText) = 0;
  virtual void SetUnitIcons(DWORD dwIcons) = 0;
};

interface IUISlider : IUIElement
{
  virtual void SetMinValue(int nVal) = 0;
  virtual void SetMaxValue(int nVal) = 0;
  virtual void SetStep(int nVal) = 0;
  virtual void SetPosition(int nPos) = 0;
  virtual int GetPosition() = 0;
};

interface IUIScrollBar : IUIContainer
{
  virtual void SetMinValue(int nVal) = 0;
  virtual void SetMaxValue(int nVal) = 0;
  virtual void SetStep(int nVal) = 0;
  virtual void SetButtonStep(int nVal) = 0;// how the position is changed if we press min or max button
  virtual void SetPosition(int nPos) = 0;
  virtual int GetPosition() = 0;
};

interface IUIListRow : IRefCount
{
  virtual int GetNumberOfElements() const = 0;
  virtual IUIElement * GetElement(int nIndex) const = 0;
  virtual void SetUserData(int nData) = 0;
  virtual int GetUserData() const = 0;
};

interface IUIListSorter : IRefCount
{
  virtual bool operator()(int nSortColumn, const IUIListRow *pRow1, const IUIListRow *pRow2, bool bForward) const = 0;
};

interface IUIListControl : IUIContainer
{
  // Get number of items
  virtual int GetNumberOfItems() = 0;
  // Add new line of items
  virtual void AddItem(int nData = 0) = 0;
  // Remove line of items
  virtual void RemoveItem(int nIndex) = 0;
  // Get line, if nItem == -1 then return headers line
  virtual IUIListRow * GetItem(int nIndex) = 0;
  // Get index of item by user data, if no such nID then returns -1
  virtual int GetItemByID(int nID) = 0;
  // selection operations
  virtual void SetSelectionItem(int nSel) = 0;
  virtual int GetSelectionItem() = 0;
  // initial update, call this function after all items are added
  virtual void InitialUpdate() = 0;
  // set sort functor for column
  virtual void SetSortFunctor(int nColumn, IUIListSorter *pSorter) = 0;
  // sort list by column
  virtual bool Sort(int nColumn, int nSortType = 0) = 0;
  // ReSort list
  virtual bool ReSort() = 0;
};

interface IUIComplexScroll : IUIContainer
{
  virtual void AddItem(IUIElement *pElement, bool bResizeToFitText) = 0;
  virtual void Clear() = 0;
};

interface IUIComboBox : IUIContainer
{
  // Add item
  virtual void AddItem(IUIElement *pElement) = 0;
  // Get selected item
  virtual int GetSelectionItem() = 0;
  // Set selected item
  virtual void SetSelectionItem(int nItem) = 0;
  // Get number of added items
  virtual int GetNumberOfItems() = 0;
  // Get item by index
  virtual IUIElement * GetItem(int nItem) = 0;
  // Clear control
  virtual void Clear() = 0;
};

interface IUIShortcutBar : IUIContainer
{
  // add bar and return pointer to IUIElement interface to change bar visualization params
  virtual IUIElement * AddBar() = 0;
  // add item to the current bar (last added bar). 
  // return pointer to IUIElement to change item params
  virtual IUIElement * AddItem() = 0;
  // Add nNum lines of new items ( for faster performance in total encyclopedia )
  virtual void AddMultyItems(int nNum) = 0;
  // add text item, the height of text will be computed automatically
  virtual IUIElement * AddTextItem(const wchar_t *pszText) = 0;
  // initial update, call this function after all bars and items are added
  virtual void InitialUpdate() = 0;
  // get selection index of bar and selection index of item
  virtual void GetSelectionItem(int *pBar, int *pItem) = 0;
  // set selection index of bar and selection index of item
  virtual void SetSelectionItem(int nBar, int nItem) = 0;
  // get bar expand state
  virtual bool GetBarExpandState(int nBar) = 0;
  // set bar expand state
  virtual void SetBarExpandState(int nBar, bool bExpand, bool bNotify = false) = 0;
  // get bar
  virtual IUIElement * GetBar(int nBar) = 0;
  // get number of bars
  virtual int GetNumberOfBars() = 0;
  // get number of items in the bar
  virtual int GetNumberOfItems(int nBar) = 0;
  // get item
  virtual IUIElement * GetItem(int nBar, int nItem) = 0;
  // clear control ( do not use RemoveAllChildren() because it will destroy scroll bar )
  virtual void Clear() = 0;
};


interface IUIMessageBox : IUIContainer
{
  virtual void SetMessageBoxType(int nType) = 0;
  virtual void SetMessageBoxText(const char *pszText) = 0;
  virtual int GetResult() = 0;
};

interface IUIScrollTextBox : IUIContainer
{
  virtual void AppendText(const wchar_t *pszText) = 0;
};

interface IUIColorTextScroll : IUIContainer
{
  enum EColorEntrys
  {
    E_COLOR_DEFAULT = 0,
    E_COLOR_IMPORTANT = 1,
  };

  virtual void AppendMessage(const wchar_t *pszCaption, const wchar_t *pszMessage,
                                     enum EColorEntrys color = E_COLOR_DEFAULT) = 0;

};

interface IUITimeCounter : IUIElement {};

interface IUIObjMap : IUIContainer
{
  virtual void Init() = 0;
  virtual void SetMapTexture(IGFXTexture *p) = 0;
};


enum MINIMAP_CIRCLE_STYLE
{
  MMC_STYLE_DIVERGENT = 0,// circle
  MMC_STYLE_CONVERGENT = 1,//
  MMC_STYLE_MIXED = 2,//
  MMC_STYLE_LPOLYGON_DIVERGENT = 3,// left rotating polygon ( counterclockwise )
  MMC_STYLE_LPOLYGON_CONVERGENT = 4,
  MMC_STYLE_LPOLYGON_MIXED = 5,
  MMC_STYLE_RPOLYGON_DIVERGENT = 6,// right rotating polygon ( clockwise )
  MMC_STYLE_RPOLYGON_CONVERGENT = 7,
  MMC_STYLE_RPOLYGON_MIXED = 8,
};


interface IUIMiniMap : IUIElement
{
  // set the size of the field to obtain information on fog of war, fog textures will be created based on these sizes
  // this method must be called before calling the Update and Draw methods, otherwise the minimap will not be drawn
  virtual void SetTerrainSize(int nXTerrainSize, int nYTerrainSize, int _nPlayersCount) = 0;
  // set map texture
  virtual void SetBackgroundTexture(IGFXTexture *_pBackgroundTexture) = 0;
  // add information about fog of war, taken from AILogic by calling the method:
  // if the last piece of information is received - returns true
  // otherwise it returns false
  virtual bool AddWarFogData(const BYTE *pVizBuffer, int nLength) = 0;
  // add information about units taken from AILogic by calling the method:
  virtual void AddUnitsData(const struct SMiniMapUnitInfo *pUnitsBuffer, int nUnitsCount) = 0;
  // add information about displayed radii and firing angles
  virtual void AddFireRangeAreas(const struct SShootAreas *pShootAreasBuffer, int nShootAreasCount) = 0;
  // add information about artillery
  // lParam - loWord - number of sides of the polygon (must be greater than 1), hiWord - number of full revolutions during the display period (more than or equal to zero, if 0 then do not rotate)
  virtual void AddCircle(const CVec2 &vCenter, float fRadius, int nStyle, WORD wColor, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative, LPARAM lParam) = 0;
  // manipulations with the position of Markers
  virtual int AddMarker(const std::string &rszName, const CVec2 &vPos, bool _bActive, int _nID, const NTimer::STime &rStart, const NTimer::STime &rDuration, bool bRelative) = 0;
  virtual void ActivateMarker(int _nID, bool _bActive) = 0;
  virtual void ActivateMarker(const std::string &rszName, bool _bActive) = 0;
  virtual void RemoveMarker(int _nID) = 0;
  virtual void RemoveMarker(const std::string &rszName) = 0;
};

// video button is bink player interface control
interface IUIVideoButton : IUIElement
{
  virtual int GetCurrentFrame() = 0;
  virtual bool SetCurrentFrame(int nFrame) = 0;
};
