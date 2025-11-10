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

  void STDCALL Reposition(const CTRect<float> &rcParent) override;
  // virtual void STDCALL SetFocus( bool bFocus );

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;

  // Public interface
  virtual void STDCALL AddItem(IUIElement *pElement);
  virtual int STDCALL GetSelectionItem() { return nSelItem; }
  virtual void STDCALL SetSelectionItem(int nItem);
  virtual int STDCALL GetNumberOfItems() { return items.size(); }
  virtual IUIElement * STDCALL GetItem(int nItem);
  virtual void STDCALL Clear();
};

class CUIComboBoxBridge : public IUIComboBox, public CUIComboBox
{
  OBJECT_NORMAL_METHODS(CUIComboBoxBridge);
  DECLARE_SUPER(CUIComboBox);
  DEFINE_UICONTAINER_BRIDGE;

  void STDCALL AddItem(IUIElement *pElement) override { CSuper::AddItem(pElement); }
  int STDCALL GetSelectionItem() override { return CSuper::GetSelectionItem(); }
  void STDCALL SetSelectionItem(int nItem) override { CSuper::SetSelectionItem(nItem); }
  int STDCALL GetNumberOfItems() override { return CSuper::GetNumberOfItems(); }
  IUIElement * STDCALL GetItem(int nItem) override { return CSuper::GetItem(nItem); }
  void STDCALL Clear() override { CSuper::Clear(); }
};

#endif // __UI_COMBOBOX_H__