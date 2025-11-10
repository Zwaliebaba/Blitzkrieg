#include "StdAfx.h"
#include "..\TreeItem.h"
#include "PropView.h"

#define ID_OI 515

// 
// CPropView

BEGIN_MESSAGE_MAP(CPropView, SECControlBar)
// {{AFX_MSG_MAP(CPropView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	// }}AFX_MSG_MAP
END_MESSAGE_MAP()

// 
// CPropView construction/destruction

CPropView::CPropView()
{
	pActiveTreeItem = 0;
}

CPropView::~CPropView()
{
}
// ///////
int CPropView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (SECControlBar::OnCreate(lpCreateStruct) == -1)
    return -1;
  
	// Init object inspector
  RECT rect;
	
  GetClientRect( &rect );
	// GetWindowRect( &rect );
	// m_wndOI.CreateEx( WS_EX_STATICEDGE, 0, 0, WS_CHILD | WS_VISIBLE, rect, this, ID_OI );
  m_wndOI.Create( 0, "ObjectInspector", WS_CHILD | WS_VISIBLE, rect, this, ID_OI );
  m_wndOI.SetWindowPos( 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE );

	return 0;
}

// ///////
BOOL CPropView::PreTranslateMessage( MSG* pMsg )
{
  if ( pMsg->message == WM_USER + 1 )
	{
		GetParent()->PostMessage( WM_USERCHANGEPARAM );
		UpdateValue( pMsg->wParam );
		return true;
	}

  return SECControlBar::PreTranslateMessage( pMsg );
}

// ///////
void CPropView::OnSize(UINT nType, int cx, int cy) 
{
  SECControlBar::OnSize(nType, cx, cy);
  
  CRect rectInside;
  GetInsideRect(rectInside);
  ::SetWindowPos(m_wndOI, NULL, rectInside.left, rectInside.top,
    rectInside.Width(), rectInside.Height(),
    SWP_NOZORDER|SWP_NOACTIVATE);
}

/* // ///////
 */

void CPropView::SetItemProperty( const char *szItemName, CTreeItem *pProp )
{
	PropID nActiveProp = m_wndOI.GetMyActiveProp();
	if ( nActiveProp != -1 )
	{
		UpdateValue( nActiveProp );
	}
	
	m_wndOI.ClearAll();
	ASSERT( pProp != 0 );
	pActiveTreeItem = pProp;
	
	m_wndOI.SetGroup( 1, szItemName );
	for ( CPropVector::iterator it=pProp->values.begin(); it!=pProp->values.end(); ++it )
	{
		m_wndOI.AddPropertiesValue( it->nId, it->nDomenType, it->szDisplayName, it->value, 1 );
		if ( it->nDomenType == DT_COMBO || it->nDomenType == DT_BROWSE || it->nDomenType == DT_BROWSEDIR )
		{
			for ( vector<string>::iterator st=it->szStrings.begin(); st!=it->szStrings.end(); ++st )
				m_wndOI.AddPropertyString( it->nId, *st);
		}
	}
}

void CPropView::UpdateValue( PropID nID )
{
	if ( pActiveTreeItem )
	{
		string szName = m_wndOI.GetPropertyName( nID );
		if ( szName.size() == 0 )
			return;			// ERROR

		CPropVector::iterator it = pActiveTreeItem->values.begin();
		for ( ; it!=pActiveTreeItem->values.end(); ++it )
		{
			if ( nID == it->nId )
			{
				CVariant newValue = m_wndOI.GetPropertyValue( nID );
				if ( it->value != newValue )
					pActiveTreeItem->UpdateItemValue( nID, newValue );
				return;
			}
		}
	}
}
