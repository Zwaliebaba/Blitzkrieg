#include "StdAfx.h"
#include "UIStatusBar.h"

static constexpr int ICON_WIDTH = 14;

CUIStatusBar::CUIStatusBar() : dwActiveIcons(0)
{
  vInitialIconPos = CVec2(13, 14);

  for (int i = 0; i < PROP_SIZE; i++) { nVisibleWindowPropIDs[i] = -1; }

  idVectors[0].push_back(30);// recruit unit
  idVectors[0].push_back(31);// normal unit
  idVectors[0].push_back(32);// veteran unit
  idVectors[0].push_back(33);// elite unit

  idVectors[1].push_back(40);// temp
  idVectors[1].push_back(41);// temp
  idVectors[1].push_back(42);// temp
}

void CUIStatusBar::OutputString(int nControl, const WORD *pszText)
{
  IUIElement *pElement = GetChildByID(nControl);
  NI_ASSERT_TF(pElement != 0, NStr::Format("Can't find child %d by ID", nControl), return);
  pElement->SetWindowText(pElement->GetState(), pszText);
}

void CUIStatusBar::OutputValue(int nControl, float fVal)
{
  auto pNumIndicator = checked_cast<IUINumberIndicator *>(GetChildByID(nControl));
  NI_ASSERT_TF(pNumIndicator != 0, NStr::Format("Can't find child %d by ID", nControl), return);
  pNumIndicator->SetValue(fVal);
}

void CUIStatusBar::SetUnitProperty(int nPropType, int nPropValue, const WORD *pszToolText)
{
  NI_ASSERT_T(nPropType < PROP_SIZE, "Invalid nPropType parameter in CUIStatusBar::SetUnitProperty()");
  /* if ( nPropType >= PROP_SIZE )
       */

  int nWindowID = -1;
  if (nPropValue != -1)
  {
    if (nPropValue >= idVectors[nPropType].size())
    {
      // print an error message to the console buffer
      GetSingleton<IConsoleBuffer>()->Write(CONSOLE_STREAM_CONSOLE, L"Error in CUIStatusBar::SetUnitProperty(): nPropValue < idVectors[nPropType].size()");
      return;
    }
    // NI_ASSERT_T( nPropValue < idVectors[nPropType].size(), "Invalid nPropValue parameter in CUIStatusBar::SetUnitProperty()" );
    nWindowID = idVectors[nPropType][nPropValue];
  }

  if (nWindowID != nVisibleWindowPropIDs[nPropType])
  {
    IUIElement *pVisible = nullptr;
    if (nVisibleWindowPropIDs[nPropType] != -1)
    {
      pVisible = GetChildByID(nVisibleWindowPropIDs[nPropType]);
      NI_ASSERT_T(pVisible != 0, NStr::Format( "There is no window with id %d", nVisibleWindowPropIDs[nPropType] ));
      if (pVisible) pVisible->ShowWindow(UI_SW_HIDE);
    }
    if (nWindowID != -1)
    {
      pVisible = GetChildByID(nWindowID);
      NI_ASSERT_T(pVisible != 0, NStr::Format( "There is no window with id %d", nWindowID ));
      if (pVisible) pVisible->ShowWindow(UI_SW_SHOW);
    }
    nVisibleWindowPropIDs[nPropType] = nWindowID;
  }

  // set the tooltip text
  if (pszToolText) { if (auto pIcon = dynamic_cast<CSimpleWindow *>(GetChildByID(nWindowID))) pIcon->SetHelpContext(0, pszToolText); }
}

void CUIStatusBar::SetUnitIcons(DWORD dwIcons)
{
  if (dwActiveIcons == dwIcons)
  {
    // nothing has changed
    return;
  }

  CVec2 pos(vInitialIconPos.x, vInitialIconPos.y);
  for (int i = 0; i < 32; ++i)
  {
    IUIElement *pElement = GetChildByID(i + 50);
    if (!pElement) continue;
    if (dwIcons & (1 << i))
    {
      pElement->ShowWindow(UI_SW_SHOW);
      pElement->SetWindowPlacement(&pos, nullptr);
      pos.x += ICON_WIDTH;
    }
    else { pElement->ShowWindow(UI_SW_HIDE); }
  }

  dwActiveIcons = dwIcons;
  CTRect<float> screenRc;
  GetParent()->GetWindowPlacement(nullptr, nullptr, &screenRc);
  Reposition(screenRc);
}

int CUIStatusBar::operator&(IDataTree &ss)
{
  CTreeAccessor saver = &ss;
  saver.AddTypedSuper(static_cast<CMultipleWindow *>(this));

  if (saver.IsReading())
  {
    // All internal icons have already been created, I want to initialize their position here at the expense of the very first icon
    if (IUIElement *pElement = GetChildByID(56)) pElement->GetWindowPlacement(&vInitialIconPos, nullptr, nullptr);
  }
  return 0;
}

int CUIStatusBar::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.AddTypedSuper(1, static_cast<CMultipleWindow *>(this));
  saver.Add(2, &dwActiveIcons);
  saver.Add(3, &vInitialIconPos);

  return 0;
}