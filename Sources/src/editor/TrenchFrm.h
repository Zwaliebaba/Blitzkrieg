#ifndef __TRENCHFRM_H__
#define __TRENCHFRM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\Main\rpgstats.h"

#include "ParentFrame.h"
#include "TreeDockWnd.h"

interface IObjVisObj;
struct SProp;

class CTrenchFrame : public CParentFrame
{
	DECLARE_DYNCREATE(CTrenchFrame)
public:
	CTrenchFrame();
	virtual ~CTrenchFrame();

// Operations
public:
	virtual void GFXDraw();
	virtual void ShowFrameWindows( int nCommand );

	void RemoveTrenchIndex( int nIndex );
	int GetFreeTrenchIndex();
	
// Overrides
	// ClassWizard generated virtual function overrides
	// {{AFX_VIRTUAL(CTrenchFrame)
protected:
	// }}AFX_VIRTUAL

// Implementation
private:
	std::list<int> freeIndexes;			// for storing unfilled indexes

protected:
	virtual void SpecificInit();														// to initialize internal data after loading a project or creating a new one
	virtual void SpecificClearBeforeBatchMode();
	void SaveRPGStats( IDataTree *pDT, CTreeItem *pRootItem, const char *pszProjectName );
	void LoadRPGStats( IDataTree *pDT, CTreeItem *pRootItem );
	
	// exports one project, if all ќ, returns 0, otherwise error code
	virtual bool ExportFrameData( IDataTree *pDT, const char *pszProjectName, const char *pszResultFileName, CTreeItem *pRootItem );
	
	virtual FILETIME FindMaximalSourceTime( const char *pszProjectName, CTreeItem *pRootItem );
	virtual FILETIME FindMinimalExportFileTime( const char *pszResultFileName, CTreeItem *pRootItem );

// Generated message map functions
protected:
	// {{AFX_MSG(CTrenchFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// }}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// //

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif		// __TRENCHFRM_H__
