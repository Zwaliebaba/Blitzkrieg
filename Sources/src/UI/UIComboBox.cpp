#include "StdAfx.h"
#include "UIComboBox.h"
#include "UIMessages.h"


int CUIComboBox::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.AddTypedSuper(1, static_cast<CMultipleWindow *>(this));

  saver.Add(2, &nVSubSpace);
  saver.Add(3, &nItemLeftSpace);
  saver.Add(4, &items);
  saver.Add(5, &nSelItem);

  return 0;
}

int CUIComboBox::operator&(IDataTree &ss)
{
  CTreeAccessor saver = &ss;
  saver.AddTypedSuper(static_cast<CMultipleWindow *>(this));

  saver.Add("VSubSpace", &nVSubSpace);
  saver.Add("ItemLeftSpace", &nItemLeftSpace);
  saver.Add("Items", &items);

  return 0;
}

void CUIComboBox::AddItem(IUIElement *pElement)
{
  if (items.size() == 0)
  {
    nSelItem = 0;
    SetWindowText(0, pElement->GetWindowText(0));
  }

  items.push_back(pElement);
  AddChild(pElement);
}

void CUIComboBox::Reposition(const CTRect<float> &rcParent)
{
  NI_ASSERT_T(bComboShown == 0, "Error in CUIComboBox");
  int nV = wndRect.Height();

  // move items
  CVec2 vPos, vSize;
  vPos.x = nItemLeftSpace;
  for (CWindowList::const_iterator it = items.begin(); it != items.end(); ++it)
  {
    IUIElement *pElement = *it;
    vPos.y = nV + nVSubSpace;
    pElement->GetWindowPlacement(nullptr, &vSize, nullptr);
    pElement->SetWindowPlacement(&vPos, nullptr);
    pElement->ShowWindow(UI_SW_HIDE);
    nV += vSize.y + nVSubSpace * 2;
  }

  CMultipleWindow::Reposition(rcParent);
}

/* void CUIComboBox::SetFocus( bool bFocus )
 */

void CUIComboBox::ShowCombo(bool bShow)
{
  NI_ASSERT_T(bShow != bComboShown, "Error in CUIComboBox()");
  bComboShown = bShow;
  float fHeight = 0;
  for (CWindowList::const_iterator it = items.begin(); it != items.end(); ++it)
  {
    IUIElement *pElement = *it;
    CVec2 vSize;
    pElement->GetWindowPlacement(nullptr, &vSize, nullptr);
    pElement->ShowWindow(bShow);
    fHeight += vSize.y;
  }

  // resize the window
  if (bShow)
  {
    saveRect = wndRect;
    wndRect.y2 = wndRect.y1 + fHeight;
  }
  else { wndRect = saveRect; }
}

void CUIComboBox::Visit(interface ISceneVisitor *pVisitor) { CMultipleWindow::Visit(pVisitor); }

void CUIComboBox::Draw(IGFX *pGFX)
{
  NI_ASSERT_SLOW_T(false, "Can't user Draw() directly - use visitor pattern");
  return;

  CMultipleWindow::Draw(pGFX);
}

bool CUIComboBox::OnLButtonDown(const CVec2 &vPos, EMouseState mouseState)
{
  bool bRet = CMultipleWindow::OnLButtonDown(vPos, mouseState);
  if (!bComboShown)
  {
    if (bRet)
    {
      // Clicked on the window area, you need to open it
      ShowCombo(true);
    }
    return bRet;
  }

  if (!bRet)
  {
    ShowCombo(false);
    return bRet;
  }

  // find the area where the click was made
  if (saveRect.IsInside(vPos))
  {
    // let's turn it off
    ShowCombo(false);
    return bRet;
  }
  // let's look among the internal items
  int nV = saveRect.Height();
  float fY = vPos.y - wndRect.y1;
  if (fY < nV) return bRet;

  int nTempItem = 0;
  auto it = items.begin();
  for (; it != items.end(); ++it)
  {
    CVec2 vSize;
    (*it)->GetWindowPlacement(nullptr, &vSize, nullptr);
    if (nV <= fY && nV + vSize.y + nVSubSpace * 2 > fY) break;
    nV += vSize.y + nVSubSpace * 2;
    nTempItem++;
  }

  if (it != items.end())
  {
    // found a new allocation
    IUIElement *pElement = *it;
    SetWindowText(0, pElement->GetWindowText(0));
    nSelItem = nTempItem;

    // send a message to the top about the change in selection state
    SUIMessage msg;
    msg.nMessageCode = UI_NOTIFY_SELECTION_CHANGED;
    msg.nFirst = GetWindowID();
    msg.nSecond = nSelItem;
    GetParent()->ProcessMessage(msg);
    return bRet;
  }

  return bRet;
}

IUIElement *CUIComboBox::GetItem(int nItem)
{
  int i = 0;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    if (i == nItem) return *it;
    i++;
  }

  NI_ASSERT(0);
  return nullptr;// WTF
}

void CUIComboBox::Clear()
{
  RemoveAllChildren();
  nSelItem = -1;
  SetWindowText(0, L"");
}

void CUIComboBox::SetSelectionItem(int nItem)
{
  NI_ASSERT_T(nItem < items.size(), "Invalid selection item index");
  nSelItem = nItem;

  int i = 0;
  for (auto it = items.begin(); it != items.end(); ++it)
  {
    if (i == nItem)
      SetWindowText(0, (*it)->GetWindowText(0));
    i++;
  }
}