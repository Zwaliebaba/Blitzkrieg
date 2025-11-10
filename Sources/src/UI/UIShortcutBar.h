#ifndef __UI_SHORTCUT_BAR_H__
#define __UI_SHORTCUT_BAR_H__

#include "UIBasic.h"
#include "UISlider.h"


class CUIShortcutBar : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  //
  CObj<IUIScrollBar> pScrollBar;// initialized at boot time and used to speed up access to components

  int nLeftSpace;
  int nRightSpace;
  int nTopSpace;
  int nBottomSpace;
  int nBarHeight;
  int nVSubSpace;
  int nItemLeftSpace;
  bool bScrollBarAlwaysVisible;
  int nScrollBarWidth;

  struct SBar
  {
    DECLARE_SERIALIZE;

  public:
    CPtr<IUIElement> pElement;
    CWindowList items;
    bool bExpandState;

    SBar() : bExpandState(false) {}
  };

  using CBarsVector = std::vector<SBar>;
  CBarsVector bars;

  // ƒl¤ rendering Selection
  int nSelBar;// dedicated bar
  int nSelItem;// selected item
  std::vector<SWindowSubRect> selSubRects;
  CPtr<IGFXTexture> pSelectionTexture;// appearance - texture

  // ƒl¤creation¤ Bar, Item, Text
  std::string szBarFileName, szItemFileName, szTextFileName;

  void InitSBWidth();
  void UpdateItemsCoordinates();// Updates the coordinates of all internal items
  void UpdateScrollBarStatus();// Updates min max and SB position

public:
  CUIShortcutBar();

  // mouse wheel
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  bool STDCALL ProcessMessage(const SUIMessage &msg) override;
  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool STDCALL OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;

  // Public interface
  // add bar
  virtual IUIElement * STDCALL AddBar();
  // add item to the current bar (last added bar). 
  virtual IUIElement * STDCALL AddItem();
  virtual void STDCALL AddMultyItems(int nNum);
  virtual IUIElement * STDCALL AddTextItem(const WORD *pszText);
  // initial update, call this function after all bars and items are added
  virtual void STDCALL InitialUpdate();
  virtual void STDCALL GetSelectionItem(int *pBar, int *pItem);
  virtual void STDCALL SetSelectionItem(int nBar, int nItem);
  virtual bool STDCALL GetBarExpandState(int nBar) { return bars.size() > nBar ? bars[nBar].bExpandState : false; }
  virtual void STDCALL SetBarExpandState(int nBar, bool bExpand, bool bNotify = false);
  virtual IUIElement * STDCALL GetBar(int nBar);
  virtual int STDCALL GetNumberOfBars() { return bars.size(); }
  virtual int STDCALL GetNumberOfItems(int nBar);
  virtual IUIElement * STDCALL GetItem(int nBar, int nItem);
  virtual void STDCALL Clear();
};

class CUIShortcutBarBridge : public IUIShortcutBar, public CUIShortcutBar
{
  OBJECT_NORMAL_METHODS(CUIShortcutBarBridge);
  DECLARE_SUPER(CUIShortcutBar);
  DEFINE_UICONTAINER_BRIDGE;

  IUIElement * STDCALL AddBar() override { return CSuper::AddBar(); }
  IUIElement * STDCALL AddItem() override { return CSuper::AddItem(); }
  void STDCALL AddMultyItems(int nNum) override { CSuper::AddMultyItems(nNum); }
  IUIElement * STDCALL AddTextItem(const WORD *pszText) override { return CSuper::AddTextItem(pszText); }
  void STDCALL InitialUpdate() override { CSuper::InitialUpdate(); }
  void STDCALL GetSelectionItem(int *pBar, int *pItem) override { CSuper::GetSelectionItem(pBar, pItem); }
  void STDCALL SetSelectionItem(int nBar, int nItem) override { CSuper::SetSelectionItem(nBar, nItem); }
  bool STDCALL GetBarExpandState(int nBar) override { return CSuper::GetBarExpandState(nBar); }
  void STDCALL SetBarExpandState(int nBar, bool bExpand, const bool bNotify = false) override { CSuper::SetBarExpandState(nBar, bExpand, bNotify); }
  IUIElement * STDCALL GetBar(int nBar) override { return CSuper::GetBar(nBar); }
  int STDCALL GetNumberOfBars() override { return CSuper::GetNumberOfBars(); }
  int STDCALL GetNumberOfItems(int nBar) override { return CSuper::GetNumberOfItems(nBar); }
  IUIElement * STDCALL GetItem(int nBar, int nItem) override { return CSuper::GetItem(nBar, nItem); }
  void STDCALL Clear() override { CSuper::Clear(); }
};

#endif // __UI_SHORTCUT_BAR_H__