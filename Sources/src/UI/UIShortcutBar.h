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
  bool OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  bool ProcessMessage(const SUIMessage &msg) override;
  void Reposition(const CTRect<float> &rcParent) override;

  // serializing...
  int operator&(IDataTree &ss) override;

  // drawing
  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;

  bool OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;
  bool OnLButtonUp(const CVec2 &vPos, EMouseState mouseState) override;

  // Public interface
  // add bar
  virtual IUIElement * AddBar();
  // add item to the current bar (last added bar). 
  virtual IUIElement * AddItem();
  virtual void AddMultyItems(int nNum);
  virtual IUIElement * AddTextItem(const wchar_t *pszText);
  // initial update, call this function after all bars and items are added
  virtual void InitialUpdate();
  virtual void GetSelectionItem(int *pBar, int *pItem);
  virtual void SetSelectionItem(int nBar, int nItem);
  virtual bool GetBarExpandState(int nBar) { return bars.size() > nBar ? bars[nBar].bExpandState : false; }
  virtual void SetBarExpandState(int nBar, bool bExpand, bool bNotify = false);
  virtual IUIElement * GetBar(int nBar);
  virtual int GetNumberOfBars() { return bars.size(); }
  virtual int GetNumberOfItems(int nBar);
  virtual IUIElement * GetItem(int nBar, int nItem);
  virtual void Clear();
};

class CUIShortcutBarBridge : public IUIShortcutBar, public CUIShortcutBar
{
  OBJECT_NORMAL_METHODS(CUIShortcutBarBridge);
  DECLARE_SUPER(CUIShortcutBar);
  DEFINE_UICONTAINER_BRIDGE;

  IUIElement * AddBar() override { return CSuper::AddBar(); }
  IUIElement * AddItem() override { return CSuper::AddItem(); }
  void AddMultyItems(int nNum) override { CSuper::AddMultyItems(nNum); }
  IUIElement * AddTextItem(const wchar_t *pszText) override { return CSuper::AddTextItem(pszText); }
  void InitialUpdate() override { CSuper::InitialUpdate(); }
  void GetSelectionItem(int *pBar, int *pItem) override { CSuper::GetSelectionItem(pBar, pItem); }
  void SetSelectionItem(int nBar, int nItem) override { CSuper::SetSelectionItem(nBar, nItem); }
  bool GetBarExpandState(int nBar) override { return CSuper::GetBarExpandState(nBar); }
  void SetBarExpandState(int nBar, bool bExpand, const bool bNotify = false) override { CSuper::SetBarExpandState(nBar, bExpand, bNotify); }
  IUIElement * GetBar(int nBar) override { return CSuper::GetBar(nBar); }
  int GetNumberOfBars() override { return CSuper::GetNumberOfBars(); }
  int GetNumberOfItems(int nBar) override { return CSuper::GetNumberOfItems(nBar); }
  IUIElement * GetItem(int nBar, int nItem) override { return CSuper::GetItem(nBar, nItem); }
  void Clear() override { CSuper::Clear(); }
};

#endif // __UI_SHORTCUT_BAR_H__