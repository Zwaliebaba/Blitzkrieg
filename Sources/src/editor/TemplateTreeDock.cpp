#include "stdafx.h"
#include "TemplateTreeDock.h"
#include "resource.h"


CTemplateTreeDockBar::CTemplateTreeDockBar()
{
	pTemplateTree = new CTemplateTreeCtrl;
}

CTemplateTreeDockBar::~CTemplateTreeDockBar()
{
	delete pTemplateTree;
}


BEGIN_MESSAGE_MAP(CTemplateTreeDockBar, SECControlBar)
	// {{AFX_MSG_MAP(CTemplateTreeDockBar)
	ON_WM_SIZE()
	ON_WM_CREATE()
	// }}AFX_MSG_MAP
END_MESSAGE_MAP()


// //
// CTemplateTreeDockBar message handlers

int CTemplateTreeDockBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (SECControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

  pTemplateTree->Create( 0, "Template Tree", WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, 100 );
	return 0;
}

void CTemplateTreeDockBar::OnSize(UINT nType, int cx, int cy) 
{
	SECControlBar::OnSize(nType, cx, cy);

  CRect rectInside;
	GetInsideRect(rectInside);
	pTemplateTree->SetWindowPos( NULL, rectInside.left, rectInside.top,
		rectInside.Width(), rectInside.Height(), SWP_NOZORDER|SWP_NOACTIVATE );
}

BOOL CTemplateTreeDockBar::PreTranslateMessage(MSG* pMsg) 
{
/* CTreeItem *pItem = 0;
	 */
	return SECControlBar::PreTranslateMessage( pMsg );
}

void CTemplateTreeDockBar::SaveTemplateTree( IDataTree *pDT )
{
	pTemplateTree->SaveTemplateTree( pDT );
}

void CTemplateTreeDockBar::LoadTemplateTree( IDataTree *pDT )
{
	pTemplateTree->LoadTemplateTree( pDT );
}
