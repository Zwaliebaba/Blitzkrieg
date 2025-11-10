// SpriteFrm.h : interface of the CSpriteFrame class
//
// //

#ifndef __SPRITEFRM_H__
#define __SPRITEFRM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <SECWB.H>
#include "..\GFX\GFX.h"

#include "ParentFrame.h"
#include "SpriteTreeItem.h"
#include "SpriteView.h"
#include "ThumbList.h"


class CSpriteFrame : public CParentFrame
{
	DECLARE_DYNCREATE(CSpriteFrame)
public:
	CSpriteFrame();
	virtual ~CSpriteFrame();

// Attributes
public:

// Operations
public:
	virtual void GFXDraw();
	virtual void ShowFrameWindows( int nCommand );
	
	BOOL Run();										// Called from EditorApp OnIdle()
	bool IsRunning() { return bRunning; }
	void ClearComposedFlag() { bComposed = false; }

	void ViewSizeChanged();
	void UpdateThumbWindows() { m_wndAllDirThumbItems.Invalidate(); m_wndSelectedThumbItems.Invalidate(); }

	void ActiveDirNameChanged();
	void SelectItemInSelectedThumbList( DWORD dwData );
	void DeleteFrameInSelectedList( DWORD dwData );
	
// Overrides
	// ClassWizard generated virtual function overrides
	// {{AFX_VIRTUAL(CSpriteFrame)
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	// }}AFX_VIRTUAL

// Implementation
private:
	// view for the client area of ​​the frame.
	CSpritesItem *m_pActiveSpritesItem;
	CThumbList m_wndAllDirThumbItems;
	CThumbList m_wndSelectedThumbItems;

	bool bRunning;								// there are two states, editing and preview
	bool bComposed;

protected:
	void ComposeAnimations();
	void ClickOnThumbList( int nID );
	void DoubleClickOnThumbList( int nID );
	void DeleteFrameInTree( int nID );
	void SetActiveSpritesItem( CSpritesItem *pSpritesItem );

	virtual BOOL SpecificTranslateMessage( MSG *pMsg );
	virtual void SpecificInit();													// to initialize internal data after loading a project or creating a new one
	virtual void SpecificClearBeforeBatchMode();
	
	// exports one project, if everything is OK, returns 0, otherwise error code
	virtual bool ExportFrameData( IDataTree *pDT, const char *pszProjectName, const char *pszResultFileName, CTreeItem *pRootItem );
// virtual FILETIME FindMaximalSourceTime( const char *pszProjectName, CTreeItem *pRootItem );
	virtual FILETIME FindMinimalExportFileTime( const char *pszResultFileName, CTreeItem *pRootItem );
	
// Generated message map functions
protected:
	// {{AFX_MSG(CSpriteFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRunButton();
	afx_msg void OnStopButton();
	afx_msg void OnUpdateStopButton(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRunButton(CCmdUI* pCmdUI);
	// }}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// //

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif		// __SPRITEFRM_H__
