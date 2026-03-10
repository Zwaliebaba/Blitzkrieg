#include "pch.h"

#include "UIVideoButton.h"


int CUIVideoButton::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;
  saver.AddTypedSuper(1, static_cast<CSimpleWindow *>(this));
  saver.Add(2, &szBinkFile);
  return 0;
}

int CUIVideoButton::operator&(IDataTree &ss)
{
  CTreeAccessor saver = &ss;
  saver.AddTypedSuper(static_cast<CSimpleWindow *>(this));
  saver.Add("BinkFile", &szBinkFile);
  return 0;
}

void CUIVideoButton::Reposition(const CTRect<float> &rcParent)
{
  CSimpleWindow::Reposition(rcParent);
}

void CUIVideoButton::Visit(interface ISceneVisitor *pVisitor) { pVisitor->VisitUICustom(dynamic_cast<IUIElement *>(this)); }

void CUIVideoButton::Draw(IGFX *pGFX) {}

bool CUIVideoButton::Update(const NTimer::STime &currTime) { return false; }

int CUIVideoButton::GetCurrentFrame() { return -1; }

bool CUIVideoButton::SetCurrentFrame(int nFrame) { return false; }