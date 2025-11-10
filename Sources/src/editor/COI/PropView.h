#ifndef __PROPVIEW_H__
#define __PROPVIEW_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CtrlObjectInspector.h"
class CTreeItem;

// These are messages from the Object Inspector going through the tree to frame
const UINT WM_USERCHANGEPARAM = WM_USER + 10;		// the user changed the value in OI

// 
// CPropView window

class CPropView : public SECControlBar
{
public:
  
  // Operations
public:
	void SetItemProperty( const char *szItemName, CTreeItem *pProp );
	void ClearControl() { pActiveTreeItem = 0; m_wndOI.ClearAll(); }

/* void SetPropMap( const CPropMap *_pPropMap );
   */
  // Implementation
  BOOL PreTranslateMessage( MSG* pMsg );
  // BOOL OnInitDialog();
  
public:
  CPropView();  
  virtual ~CPropView();
	void UpdateValue( PropID nID );
  
  // Generated message map functions
protected:
	CCtrlObjectInspector m_wndOI;
	CTreeItem *pActiveTreeItem;

  
  // {{AFX_MSG(CPropView)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnChangeProp( WPARAM wParam, LPARAM lParam );
	// }}AFX_MSG
  DECLARE_MESSAGE_MAP()
};

#endif // __PROPVIEW_H__
