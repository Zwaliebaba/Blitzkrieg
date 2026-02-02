#ifndef __UI_COMBOBOX_H__
#define __UI_COMBOBOX_H__

#include "UIBasic.h"
#include "UISlider.h"


class CUIComboBox : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  //
  int nVSubSpace;
  int nItemLeftSpace;
  CWindowList items;
  bool bComboShown;
  CTRect<float> saveRect;// saved window size, necessary to restore the state after the windows collapse

  // To draw Selection
  int nSelItem;// selected item
  /* std::vector<SWindowSubRect> selSubRects;
     */

  void UpdateItemsCoordinates();// Updates the coordinates of all internal items
  void ShowCombo(bool bShow);

public:
  CUIComboBox() : nVSubSpace(2), nItemLeftSpace(0), bComboShown(false), nSelItem(-1) {}

  void Reposition(const CTRect<float> &rcParent) override;
  // virtual void SetFocus( bool bFocus );

  // serializing...
  int operator&(IDataTree &ss) override;

  // drawing
  void Draw(IGFX *pGFX) override;
  void Visit(interface ISceneVisitor *pVisitor) override;

  bool OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;

  // Public interface
  virtual void AddItem(IUIElement *pElement);
  virtual int GetSelectionItem() { return nSelItem; }
  virtual void SetSelectionItem(int nItem);
  virtual int GetNumberOfItems() { return items.size(); }
  virtual IUIElement * GetItem(int nItem);
  virtual void Clear();
};

class CUIComboBoxBridge : public IUIComboBox, public CUIComboBox
{
  OBJECT_NORMAL_METHODS(CUIComboBoxBridge);
  DECLARE_SUPER(CUIComboBox);
  DEFINE_UICONTAINER_BRIDGE;

  void AddItem(IUIElement *pElement) override { CSuper::AddItem(pElement); }
  int GetSelectionItem() override { return CSuper::GetSelectionItem(); }
  void SetSelectionItem(int nItem) override { CSuper::SetSelectionItem(nItem); }
  int GetNumberOfItems() override { return CSuper::GetNumberOfItems(); }
  IUIElement * GetItem(int nItem) override { return CSuper::GetItem(nItem); }
  void Clear() override { CSuper::Clear(); }
};

#endif // __UI_COMBOBOX_H__