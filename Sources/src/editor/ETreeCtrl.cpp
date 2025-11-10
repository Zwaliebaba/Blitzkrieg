// ETreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "TreeItem.h"
#include "ETreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 
// CETreeCtrl

CETreeCtrl::CETreeCtrl()
{
	m_pDragImageList = 0;
	m_bDragging = false;
}

CETreeCtrl::~CETreeCtrl()
{
// delete pRootItem;
	pRootItem = 0;					// deleted as CPtr
}

void CETreeCtrl::OnDestroy() 
{
/* HTREEITEM handle = m_treeCtrl.GetRootItem();
	 */		// Everything is deleted as part of pRootItem
	CWnd::OnDestroy();
}

void CETreeCtrl::DestroySiblingItems(HTREEITEM _handle)
{
	NI_ASSERT ( _handle != 0 );

	HTREEITEM handle = _handle;
	while ( handle )
	{
		DWORD data = m_treeCtrl.GetItemData( handle );
		CTreeItem *pItem = (CTreeItem *) data;
		if ( pItem )
		{
			delete pItem;
		}
/* HTREEITEM child = m_treeCtrl.GetNextItem( handle, TVGN_CHILD );
		 */
		handle = m_treeCtrl.GetNextItem( handle, TVGN_NEXT );
	}
}

BOOL CETreeCtrl::PreTranslateMessage( MSG* pMsg )
{
	return CWnd::PreTranslateMessage( pMsg );
}

CTreeItem *CETreeCtrl::CreateRootItem( int nRootItemId )
{
	IObjectFactory *pFactory = GetCommonFactory();
	IRefCount *pObj = pFactory->CreateObject( nRootItemId );
	NI_ASSERT ( pObj != 0 );

	pRootItem = (CTreeItem *) pObj;
	NI_ASSERT( pRootItem != 0 );
	if ( pRootItem )
	{
		pRootItem->SetTreeCtrl( GetTreeCtrl() );
		pRootItem->SetParent( pRootItem );
		pRootItem->CreateDefaultChilds();
		pRootItem->InsertChildItems();
		GetTreeCtrl()->SetItemData( 0, (DWORD) pRootItem.GetPtr() );
	}
	return pRootItem;
}

CTreeItem* CETreeCtrl::GetTreeItem( HTREEITEM hti )
{
	TV_ITEM tvi;
	tvi.mask = TVIF_PARAM;
	tvi.hItem = hti;
	m_treeCtrl.GetItem(&tvi);
	
	NI_ASSERT( tvi.lParam != 0 );
	return (CTreeItem *) tvi.lParam;
}

BEGIN_MESSAGE_MAP(CETreeCtrl, CWnd)
	// {{AFX_MSG_MAP(CETreeCtrl)
	ON_WM_CREATE()
  ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_CONTROL, OnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_CONTROL, OnItemExpanding)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONTROL, OnSelect)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_CONTROL, OnRButtonClick)
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_CONTROL, OnKeyDown)
	// }}AFX_MSG_MAP
END_MESSAGE_MAP()

// 
// CETreeCtrl message handlers

int CETreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	// create a tree control
	DWORD dwStyle = TVS_SHOWSELALWAYS |
		TVS_HASBUTTONS |
		TVS_LINESATROOT |
		TVS_HASLINES |
// TVS_EDITLABELS |
		TVS_SHOWSELALWAYS |
// TVS_DISABLEDRAGDROP |
		WS_CHILD | WS_VISIBLE;
	
	DWORD dwStyleEx = /* TVXS_MULTISEL | */ TVXS_FLYBYTOOLTIPS | LVXS_HILIGHTSUBITEMS;
	
	bool bCreated = m_treeCtrl.Create( dwStyle, dwStyleEx, CRect(0, 0, 0, 0), this, IDC_TREE_CONTROL);
	NI_ASSERT(bCreated);
/* AddSomeItems();
	 */
	m_treeCtrl.ShowWindow( SW_SHOW );  
  m_treeCtrl.SetNotifyWnd( this );
  m_treeCtrl.UpdateWindow();
	
	return 0;
}

void CETreeCtrl::LoadImageList( UINT nID )
{
	CBitmap bmp;
	// normal tree images
	m_imlNormal.Create(16,
		16,
		TRUE,
		8,	// number of initial images
		8);
	
	NI_ASSERT( m_imlNormal.m_hImageList != 0 );
	
	bmp.LoadBitmap(nID);
	m_imlNormal.Add( &bmp, RGB(255,255,255));
	bmp.DeleteObject();
	m_treeCtrl.SetImageList(&m_imlNormal, TVSIL_NORMAL);
}

void CETreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
// TRACE(_T("Begin Drag\n"));
	NI_ASSERT(m_bDragging == FALSE);
	NI_ASSERT(m_pDragImageList == NULL);
	
	CPoint ptAction;
	UINT nFlags;
	
	GetCursorPos(&ptAction);
  ScreenToClient( &ptAction );
	
	HTREEITEM hTreeItem = m_treeCtrl.HitTest(ptAction, &nFlags);
  if ( !hTreeItem )
    return;
	
	m_bDragging = TRUE;
	m_hitemDrag = hTreeItem;
	m_pDragImageList = m_treeCtrl.CreateDragImage(hTreeItem);

	NI_ASSERT( m_pDragImageList != 0 );
	m_pDragImageList->DragShowNolock(TRUE);

	CRect rcItem;
	m_treeCtrl.GetItemRect(hTreeItem, &rcItem, TRUE);
	m_pDragImageList->BeginDrag(0, CPoint( ptAction.x - rcItem.left, ptAction.y - rcItem.top ) );

	CRect r;
  theApp.GetMainWnd()->GetWindowRect( &r );
	GetCursorPos(&ptAction);
  ptAction -= r.TopLeft();
  rcItem   -= r.TopLeft();

	m_pDragImageList->DragMove(ptAction);
	m_pDragImageList->DragEnter(theApp.GetMainWnd(), ptAction);
// m_pDragImageList->DragEnter(this, ptAction);
	SetCapture();
	
  *pResult = 0;
}

void CETreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging)
	{
		HTREEITEM		hitem;
		UINT				flags;
    CRect       r;
    CPoint      pt, ptTree;
		
		NI_ASSERT(m_pDragImageList != NULL);
		
    theApp.GetMainWnd()->GetWindowRect( &r );
		GetCursorPos( &pt );
    pt -= r.TopLeft();
		m_pDragImageList->DragMove( pt );
		
    GetCursorPos(&ptTree);
    ScreenToClient( &ptTree );

		hitem = m_treeCtrl.HitTest(ptTree, &flags);
		if ( hitem && (TVHT_ONITEM & flags) )
		{
			// Checking item drag & item drop types
			CTreeItem *pDrag = GetTreeItem( m_hitemDrag );
			CTreeItem *pDrop = GetTreeItem( hitem );

// if ( pDrag->GetItemType() == pDrop->GetItemType() )
			if ( pDrag->IsCompatibleWith(pDrop) )
			{
				m_pDragImageList->DragLeave( theApp.GetMainWnd() );
				m_treeCtrl.SelectDropTarget( hitem );
				m_hitemDrop = hitem;
				m_pDragImageList->DragEnter( theApp.GetMainWnd(), pt );
			}
			else
			{
				m_pDragImageList->DragLeave( theApp.GetMainWnd() );
				m_treeCtrl.SelectDropTarget( 0 );
				m_hitemDrop = 0;
				m_pDragImageList->DragEnter( theApp.GetMainWnd(), pt );
			}
		}
		else if ( !hitem )
		{
			m_pDragImageList->DragLeave(theApp.GetMainWnd());
			m_treeCtrl.SelectDropTarget( 0 );
			m_pDragImageList->DragEnter(theApp.GetMainWnd(), pt);
		}
/* else if ( m_hitemDrop && !(TVHT_ONITEM & flags) )
		 */
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CETreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
  if(m_bDragging == TRUE)
  {
// TRACE(_T("End Drag\n"));
		ReleaseCapture();
		NI_ASSERT(m_pDragImageList != NULL);
		m_pDragImageList->DragLeave(this);
		m_pDragImageList->EndDrag();
		delete m_pDragImageList;
		m_pDragImageList = NULL;
		m_bDragging = FALSE;
/* TV_ITEM tvidrag;
		 */

// m_treeCtrl.SelectDropTarget( 0 );
/* CRect rect;
		 */
		HTREEITEM hItemDrop = m_treeCtrl.GetDropHilightItem();
		m_treeCtrl.SelectDropTarget( 0 );
		if ( hItemDrop == NULL || hItemDrop == m_hitemDrag )
			return;

		// Checking item drag & item drop types
		CTreeItem *pDrag = GetTreeItem( m_hitemDrag );
		CTreeItem *pDrop = GetTreeItem( hItemDrop );
		pDrag->CopyItemTo( pDrop );

// CopyItemValues( pDrag, pDrop );
/* if ( pDrag->GetItemType() == pDrop->GetItemType() )
		 */

/* if ( IsFolder( tvi.lParam ) ) // can only be thrown into a folder
			 */
  }
  
  CWnd::OnLButtonUp(nFlags, point);
}

void CETreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
/* NM_TREEVIEW* pInfo = (NM_TREEVIEW*)pNMHDR;
	 */
	*pResult = 0;
}

void CETreeCtrl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// for proper operation of the QuickView window,
	// which shows the currently selected object
	if ( !bShow )
		m_treeCtrl.DeselectAllItems();
}

// ///////
// Without this tricontrol function we will not see
void CETreeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if ( ::IsWindow(m_treeCtrl) )
		m_treeCtrl.SetWindowPos( NULL, 0, 0, cx, cy,
		SWP_NOMOVE |
		SWP_NOACTIVATE |
		SWP_NOZORDER );
}

void CETreeCtrl::OnSelect(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hti = m_treeCtrl.GetFirstSelectedItem();

	if ( hti )
	{
		LPARAM lParam = m_treeCtrl.GetItemData( hti );
		GetParent()->PostMessage( WM_USERTREESEL, 0, lParam );
	}
	*pResult = 0;
}

/* void CETreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
 */

void CETreeCtrl::OnRButtonClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	HTREEITEM hti = m_treeCtrl.GetFirstSelectedItem();
	if ( hti )
	{
		LPARAM lParam = m_treeCtrl.GetItemData( hti );
		GetParent()->PostMessage( WM_USERRBUTTONCLICK, 0, lParam );
	}
}

void CETreeCtrl::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN)pNMHDR;
	HTREEITEM hti = m_treeCtrl.GetFirstSelectedItem();

	if ( hti )
	{
		LPARAM lParam = m_treeCtrl.GetItemData( hti );
		GetParent()->PostMessage( WM_USERKEYDOWN, ptvkd->wVKey, lParam );
	}
	*pResult = 0;
}

void CETreeCtrl::SaveTree( IStructureSaver *pSS )
{	
	pRootItem->operator &( *pSS );
}

void CETreeCtrl::LoadTree( IStructureSaver *pSS )
{
	// First we kill all the childrens in the tree
	pRootItem->RemoveAllChilds();

	pRootItem->operator &( *pSS );
	pRootItem->CreateDefaultChilds();
	pRootItem->InsertChildItems();
}

void CETreeCtrl::SaveTree( IDataTree *pDT )
{	
	pRootItem->operator&( *pDT );
}

void CETreeCtrl::LoadTree( IDataTree *pDT )
{
	// First we kill all the childrens in the tree
	pRootItem->RemoveAllChilds();
	
	pRootItem->operator&( *pDT );
	// here you need to go through all childs and remove zero elements
	pRootItem->DeleteNullChilds();
	pRootItem->CreateDefaultChilds();
	pRootItem->InsertChildItems();
	pRootItem->CallMeAfterSerialize();
}
