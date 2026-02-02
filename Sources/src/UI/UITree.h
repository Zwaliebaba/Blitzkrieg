#ifndef __UI_TREE_H__
#define __UI_TREE_H__

#include "UIBasic.h"
#include "UISlider.h"

#ifdef OLD

struct SUITreeItem : public IUITreeItem
{
  OBJECT_NORMAL_METHODS(SUITreeItem);
  DECLARE_SERIALIZE;

  CPtr<IUIContainer> pTreeCtrl;
  CPtr<IUIElement> pIcon;
  CPtr<IUIElement> pInfo;
  CPtr<SUITreeItem> pParent;
  int nData;

public:
  typedef std::list<CObj<SUITreeItem>> CItemList;

private:
  CItemList treeItems;

public:
  SUITreeItem() : nData(0) {}

  virtual IUITreeItem * AddTreeItem(IUIElement *_pIcon, IUIElement *_pInfo, int nUserData);
  virtual IUITreeItem * GetParentTreeItem() { return pParent; }

  virtual IUITreeIterator GetBegin() { return treeItems.begin(); }
  virtual IUITreeIterator GetEnd() { return treeItems.end(); }
  virtual int GetUserData() { return nData; }

  // serializing...
  virtual int operator&(IDataTree &ss);
};

class CUITree : public CMultipleWindow
{
  DECLARE_SERIALIZE;
  //
  CUIScrollBar *pScrollBar;// initialized at boot time and used to speed up access to components

  int nLeftSpace;
  int nTopSpace;
  int nItemHeight;
  int nHSubSpace;
  int nVSubSpace;
  bool bScrollBarAlwaysVisible;
  int nScrollBarWidth;

  CPtr<SUITreeItem> pRoot;
  CPtr<SUITreeItem> pSelection;

  // To draw Selection
  std::vector<SWindowSubRect> selSubRects;
  CPtr<IGFXTexture> pSelectionTexture;// appearance - texture

  void UpdateAll();
  void UpdateItemsCoordinates();// Updates the coordinates of all internal items
  void UpdateScrollBarStatus();// Called to check whether the ScrollBar should be displayed and its state updates
  void OnUserChangeScrollBarPosition();// Called to recalculate in response to changes in the ScrollBar's status

public:
  CUITree() : pScrollBar(0), nLeftSpace(10), nTopSpace(5), nItemHeight(30),
              nHSubSpace(2), nVSubSpace(2), nScrollBarWidth(30), bScrollBarAlwaysVisible(true) {}

  virtual ~CUITree();

  virtual void Reposition(const CTRect<float> &rcParent);

  virtual bool ProcessMessage(const SUIMessage &msg);

  // serializing...
  virtual int operator&(IDataTree &ss);

  // drawing
  virtual void Draw(IGFX *pGFX);
  virtual void Visit(interface ISceneVisitor *pVisitor);

  virtual bool OnLButtonDown(const CVec2 &vPos, EMouseState mouseState);

  // Public interface
  virtual SUITreeItem * GetRootTreeItem() { return pRoot; }
  // virtual SUITreeItem* Get
  // selection operations
  virtual void SetSelectionItem(SUITreeItem *pTreeItem);
  virtual SUITreeItem * GetSelectionItem() { return pSelection; }
};

class CUITreeBridge : public IUITreeControl, public CUITree
{
  OBJECT_NORMAL_METHODS(CUITreeBridge);
  DECLARE_SUPER(CUITree);
  DEFINE_UICONTAINER_BRIDGE;
};
#endif	// OLD

#endif // __UI_TREE_H__