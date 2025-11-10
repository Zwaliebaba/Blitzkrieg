#ifndef __UI_LIST_H__
#define __UI_LIST_H__

#include "UIBasic.h"
#include "UISlider.h"

struct SColumnProperties
{
  DECLARE_SERIALIZE;

public:
  int nWidth;// column width
  std::string szFileName;// XML file from which internal elements are created
  int nSorterType;
  SColumnProperties() : nWidth(0), nSorterType(0) {}

  // serializing...
  virtual int STDCALL operator&(IDataTree &ss);
};

using CVectorOfColumnProperties = std::vector<SColumnProperties>;

struct SUIListRow : IUIListRow
{
  OBJECT_NORMAL_METHODS(SUIListRow);
  DECLARE_SERIALIZE;

public:
  using CUIListSubItems = std::vector<CPtr<IUIElement>>;
  CUIListSubItems subItems;
  int nUserData;

  SUIListRow() : nUserData(0) {}
  int STDCALL GetNumberOfElements() const override { return subItems.size(); }
  IUIElement * STDCALL GetElement(int nIndex) const override;
  void STDCALL SetUserData(int nData) override { nUserData = nData; }
  int STDCALL GetUserData() const override { return nUserData; }

  // serializing...
  // virtual int STDCALL operator&( IDataTree &ss );
};

using CUIListItems = std::vector<CPtr<SUIListRow>>;

struct SUIListHeader : IUIListRow
{
  OBJECT_NORMAL_METHODS(SUIListHeader);
  DECLARE_SERIALIZE;

public:
  struct SColumn
  {
    CPtr<IUIElement> pElement;
    CPtr<IUIListSorter> pSorter;

    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &pElement);
      saver.Add(2, &pSorter);
      return 0;
    }
  };

  using CUIListHeaderItems = std::vector<SColumn>;
  CUIListHeaderItems subItems;
  int nUserData;

  SUIListHeader() : nUserData(0) {}
  int STDCALL GetNumberOfElements() const override { return subItems.size(); }
  IUIElement * STDCALL GetElement(int nIndex) const override;
  void STDCALL SetUserData(int nData) override { nUserData = nData; }
  int STDCALL GetUserData() const override { return nUserData; }
};

// The window behaves like MultipleWindow in terms of processing messages (it simply passes them to childs)
// But Serialize is different, it does not save the childs list, the left, right buttons and elevator are stored separately
class CUIList : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  //
  CObj<IUIScrollBar> pScrollBar;// initialized at boot time and used to speed up access to components

  int nLeftSpace;// item indentation to the left and right of the control edge
  int nTopSpace;// item indent from the bottom of the header at the top and from the bottom of the control at the bottom
  int nHeaderTopSpace;// header indent from the top of the control
  int nItemHeight;// height of one item
  int nHSubSpace;// horizontal distance between two subitems
  int nVSubSpace;// vertical distance between two items
  bool bLeftScrollBar;
  bool bScrollBarAlwaysVisible;
  int nHeaderSize;// vertical size of header, if > 0 that is the title
  int nScrollBarWidth;
  int nSelection;
  int nSortedHeaderIndex;
  bool bSortAscending;

  SUIListHeader headers;
  CUIListItems listItems;
  CVectorOfColumnProperties columnProperties;

  // To draw Selection
  std::vector<SWindowSubRect> selSubRects;
  CPtr<IGFXTexture> pSelectionTexture;// appearance - texture

  void UpdateItemsCoordinates();// Updates the coordinates of all internal items
  void UpdateScrollBarStatus();// Called to check whether the ScrollBar should be displayed and its state updates
  void EnsureSelectionVisible();// To make the selection fully visible, moves the position of the scrollbar.

  IUIElement *CreateComponent(const char *pszFileName);
  CVec2 GetComponentSize(const char *pszFileName);// returns the element's size
  void InitItemHeight();// Called from serialization to calculate line height

  // sending a message to the top about a change in the current position
  void NotifySelectionChanged();
  void NotifyDoubleClick(int nItem);
  void RemoveFocusFromItem(int nIndex);
  void MoveSelectionItemUp();

  // initializing sorting functors
  void InitSortFunctors();

public:
  CUIList();
  ~CUIList() override;

  // mouse wheel
  bool STDCALL OnMouseWheel(const CVec2 &vPos, EMouseState mouseState, float fDelta) override = 0;

  void STDCALL Reposition(const CTRect<float> &rcParent) override;

  bool STDCALL OnChar(int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState) override;
  bool STDCALL ProcessMessage(const SUIMessage &msg) override;

  // serializing...
  int STDCALL operator&(IDataTree &ss) override;

  // drawing
  void STDCALL Draw(IGFX *pGFX) override;
  void STDCALL Visit(interface ISceneVisitor *pVisitor) override;

  bool STDCALL OnLButtonDblClk(const CVec2 &vPos) override;
  bool STDCALL OnLButtonDown(const CVec2 &vPos, EMouseState mouseState) override;

  // Public interface
  // Get number of items
  virtual int STDCALL GetNumberOfItems() { return listItems.size(); }
  // Add new line of items
  virtual void STDCALL AddItem(int nData = 0);// adds a new line VectorElements to the end of the list
  // Remove last line of items
  virtual void STDCALL RemoveItem(int nIndex);// removes a line from the end of the list
  // Get line
  virtual IUIListRow * STDCALL GetItem(int nIndex);
  // Get index of item by user data, if no such nID then returns -1
  virtual int STDCALL GetItemByID(int nID);
  // selection operations
  virtual void STDCALL SetSelectionItem(int nSel);
  virtual int STDCALL GetSelectionItem() { return nSelection; }
  virtual void STDCALL InitialUpdate();
  virtual void STDCALL SetSortFunctor(int nColumn, IUIListSorter *pSorter);
  virtual bool STDCALL Sort(int nColumn, int nSortType = 0);
  virtual bool STDCALL ReSort();

  /* int GetNumberOfItems() { return listItems.size();  */
};

class CUIListBridge : public IUIListControl, public CUIList
{
  OBJECT_NORMAL_METHODS(CUIListBridge);
  DECLARE_SUPER(CUIList);
  DEFINE_UICONTAINER_BRIDGE;
  // Get number of items
  int STDCALL GetNumberOfItems() override { return CSuper::GetNumberOfItems(); }
  // Add new line of items
  void STDCALL AddItem(int nData = 0) override { CSuper::AddItem(nData); }
  // Remove last line of items
  void STDCALL RemoveItem(int nIndex) override { CSuper::RemoveItem(nIndex); }
  // Get line
  IUIListRow * STDCALL GetItem(int nIndex) override { return CSuper::GetItem(nIndex); }
  // Get index of item by user data, if no such nID then returns -1
  int STDCALL GetItemByID(int nID) override { return CSuper::GetItemByID(nID); }
  // selection operations
  void STDCALL SetSelectionItem(int nSel) override { CSuper::SetSelectionItem(nSel); }
  int STDCALL GetSelectionItem() override { return CSuper::GetSelectionItem(); }
  void STDCALL InitialUpdate() override { CSuper::InitialUpdate(); }
  void STDCALL SetSortFunctor(int nColumn, IUIListSorter *pSorter) override { CSuper::SetSortFunctor(nColumn, pSorter); }
  bool STDCALL Sort(int nColumn, const int nSortType) override { return CSuper::Sort(nColumn, nSortType); }
  bool STDCALL ReSort() override { return CSuper::ReSort(); }
};

#endif // __UI_LIST_H__