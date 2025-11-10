#ifndef __PARENT_FRAME_H__
#define __PARENT_FRAME_H__

#include <SECWB.H>
#include "..\GFX\GFX.h"

using std::string;
using std::vector;
using std::list;

interface IGFX;
class CTreeDockWnd;
class CPropView;
class CTreeItem;
class CETreeCtrl;
class SECCustomToolBar;

struct SObjectInfo
{
	std::string szPath;
	std::string szType;
	std::string szName;
	std::string szVisType;
	int operator&( IDataTree &ss )
	{
		CTreeAccessor saver = &ss;
		saver.Add( "path", &szPath );
		saver.Add( "game_type", &szType );
		saver.Add( "name", &szName );
		saver.Add( "type", &szVisType );
		return 0;
	}
};

class CParentFrame : public SECWorksheet
{
	DECLARE_DYNCREATE(CParentFrame)
public:
	CParentFrame();
	virtual ~CParentFrame();
	
	virtual void Init( IGFX *_pGFX ) { pGFX = _pGFX; }			// initialization
	virtual void ShowFrameWindows( int nCommand );					// called when focus changes, parameter SW_SHOW or SW_HIDE
	virtual void GFXDraw() {}																// rendering

	void SetChangedFlag( bool bFlag );											// sets the modification flag, displays * in the project name
	int DisplayInsertMenu();																// displays a menu with an insert option
	int DisplayDeleteMenu();
	
	// functions for saving and loading information in the registry
	void SaveRegisterData();
	void LoadRegisterData();

	int GetFrameType() { return nFrameType; }
	float GetBrightness() { return m_fBrightness; }
	float GetContrast() { return m_fContrast; }
	float GetGamma() { return m_fGamma; }
	void SetBrightness( float fVal ) { m_fBrightness = fVal; }
	void SetContrast( float fVal ) { m_fContrast = fVal; }
	void SetGamma( float fVal ) { m_fGamma = fVal; }
	int GetCompressedFormat() { NI_ASSERT( m_nCompressedFormat != -1 ); return m_nCompressedFormat; }
	int GetLowFormat() { NI_ASSERT( m_nLowFormat != -1 ); return m_nLowFormat; }
	int GetHighFormat() { NI_ASSERT( m_nHighFormat != -1 ); return m_nHighFormat; }
	int GetCompressedShadowFormat() { NI_ASSERT( m_nCompressedShadowFormat != -1 ); return m_nCompressedShadowFormat; }
	int GetLowShadowFormat() { NI_ASSERT( m_nLowShadowFormat != -1 ); return m_nLowShadowFormat; }
	int GetHighShadowFormat() { NI_ASSERT( m_nHighShadowFormat != -1 ); return m_nHighShadowFormat; }
	
	// OI and tree initialization
	void SetTreeDockBar( CTreeDockWnd *pWnd ) { pTreeDockBar = pWnd; }
	void SetOIDockBar( CPropView *pWnd );
	void SetToolBar( SECCustomToolBar *pWnd ) { pToolBar = pWnd; }
	void ClearPropView();
	BOOL SaveFrame( bool bUnlock = false );					// to save frame data after program completion
	virtual void UpdatePropView( CTreeItem *pTreeItem );								// to display new information in OI
	string GetProjectFileName() { return szProjectFileName; }		// to calculate relative directory when tree paths change
	const char *GetModuleExtension() { return szExtension.c_str(); }
	void RunBatchExporter( const char *pszSourceDir, const char *pszDestDir, const char *pszMask, bool bForceFlag, bool bOpenSave );
	
protected:
	CWnd *pWndView;											// view window for displaying
	CPtr<IGFX> pGFX;										// to speed up access to GFX, old balance
	CTreeDockWnd *pTreeDockBar;					// tree for selecting the edited item
	CPropView *pOIDockBar;							// object inspector, editing window
	SECCustomToolBar *pToolBar;					// toolbar for this frame

	string szProjectFileName;						// open project name
	string szPrevExportFileName;				// previously exported name for the project, relative to szSourceDir
	bool bChanged;											// has the project changed since loading/creation
	bool bNewProjectJustCreated;				// if true, then a new project has just been created, you need to load data from RPG stats

	int m_nCompressedFormat;
	int m_nLowFormat;
	int m_nHighFormat;

	int m_nCompressedShadowFormat;
	int m_nLowShadowFormat;
	int m_nHighShadowFormat;

	// for transactions
	struct STransaction
	{
		string szSourceName;
		string szSourceDate;
		string szSourceTime;
		string szSourceOwner;
		string szAction;
		string szDestName;
		string szDestDate;
		string szDestTime;
		string szDestOwner;

		int operator&( IDataTree &ss );
	};
	string m_szOldProjectName;					// name of the previous project file
	vector<STransaction> transactions;

	// these parameters must be initialized in the constructor for each composer
	string szComposerName;							// composer name, displayed in title
	string szExtension;									// extension for composer projects
	string szExportExtension;						// extension for exporting project files
	string szComposerSaveName;					// used when saving data in XML
	string szAddDir;										// additional directory, added to szSourceDir
	int nTreeRootItemID;								// ID root item for this composer
	bool bDefaultExportName;						// if the flag is set, then when exporting the file name 1.xml will be suggested by default
	int nFrameType;

	// settings saved in the registry
/* string szLocalSourceDir;						 */
	DWORD m_backgroundColor;						// background color for GFXDraw()
	float m_fBrightness;
	float m_fContrast;
	float m_fGamma;
	bool bTreeExpand;

protected:
	void ComputeCaption();							// updates the title of the window
	void GenerateProjectName();					// generates a name for a new project, used when creating a new project

	// reads information from the configuration file (looks for the nearest gamma.cfg file recursively going up)
	// the first parameter is the name of the directory, relative to which the linker will look for the configuration file when climbing up
	// the second parameter is a flag indicating whether we are in batch mode, so as not to swear
	bool ReadConfigFile( const char *pszDirectory, bool bBatchMode );
	bool WriteConfigFile( bool bAsk, bool bCurrentProjectOnly );	// writes information to the config file

	void LoadTransactions( IDataTree *pDT );		// reads information about previous changes in the project
	void SaveTransactions( FILETIME *pFT, IDataTree *pDT, const char *pszDest, int nAction );	// records information about changes that have occurred
	bool ConvertAndSaveImage( const char *pszSrc, const char *pszDest );								// applies transformations and saves the image
	CETreeCtrl *CreateTrees();
	virtual string GetExportFileName();

	// module-specific message processing
	virtual BOOL SpecificTranslateMessage( MSG *pMsg ) { return FALSE; }
	virtual void SpecificInit() {}													// to initialize internal data after loading a project or creating a new one
	virtual void SpecificSave( IDataTree *pDT ) {}					// called when writing a project, only needed in GUI composer
	virtual void SpecificClearBeforeBatchMode() {};					// to clean before starting batch mode
	
	virtual void SaveFrameOwnData( IDataTree *pDT );				// to save your own project data
	virtual void LoadFrameOwnData( IDataTree *pDT );				// to download

	// saves the composer's RPG stats in pDT, accepts the pRootItem parameter so that the function can be called from batch mode
	virtual void SaveRPGStats( IDataTree *pDT, CTreeItem *pRootItem, const char *pszProjectName ) {}
	// loads RPG stats
	virtual void LoadRPGStats( IDataTree *pDT, CTreeItem *pRootItem ) {}

/* //fills rpgStats from the tree before actually writing to the file (or when creating a new project)
	 */

	// BATCH MODE
	// exports one project, if everything is OK, returns 0, otherwise error code
	int ExportSingleFile( const char *pszFileName, const char *pszDestDir, bool bForceFlag, bool bOpenSave );
	// loads data before exporting the project, used in object, building, mesh frames
	virtual bool LoadFramePreExportData( const char *pszProjectFile, CTreeItem *pRootItem ) { return true; }
	// calculates the maximum change time for project sources
	virtual FILETIME FindMaximalSourceTime( const char *pszProjectName, CTreeItem *pRootItem ) { FILETIME time; time.dwHighDateTime = 0xffffffff; time.dwLowDateTime = 0xffffffff; return time; }
	// calculates the minimum time for export results
	virtual FILETIME FindMinimalExportFileTime( const char *pszResultFileName, CTreeItem *pRootItem ) { FILETIME time; time.dwHighDateTime = 0; time.dwLowDateTime = 0; return time; }
	// launches the exporter
	// this function exports all project data, called both from batch mode and with OnFileSave()
	// parameters: project source name, result file name, RootItem, additional flag
	virtual bool ExportFrameData( IDataTree *pDT, const char *pszProjectName, const char *pszResultFileName, CTreeItem *pRootItem ) { return true; }
	
	// to lock the editor
	bool LockFile();				// returns false if it cannot lock the file (already locked)
	bool UnLockFile();			// returns false if it cannot unlock (file deletion error or some other problem)
	void SwitchDockerVisible( SECControlBar *pBar );
	void UpdateShowMenu( CCmdUI* pCmdUI, SECControlBar *pBar );

	void SwitchActiveFrame( int id );
	void UpdateFrameMenu( CCmdUI* pCmdUI, int id );

public:
	void LoadComposerFile( const char *pszFileName );

	// Overrides
	// ClassWizard generated virtual function overrides
	// {{AFX_VIRTUAL(CAnimationFrame)
protected:
// virtual BOOL PreTranslateMessage(MSG* pMsg);
// virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnUpdateFrameTitle( BOOL bAddToTitle ) { /* this shit should be empty */ }
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	// }}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	// {{AFX_MSG(CAnimationFrame)
// afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnFileCreateNewProject();
	afx_msg void OnFileOpen();
	afx_msg void OnFileClose();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveProjectAs();
	afx_msg void OnExportPak();
	afx_msg void OnRunGame();
	afx_msg void OnHelp();
	afx_msg void OnFileExportFiles();
	afx_msg void OnUpdateFileExportFiles(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveProjectAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCloseFile(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnFileSetdirectories();
	afx_msg void OnSetPictureOptions();
	afx_msg void OnFileBatchMode();
	afx_msg void OnEditSetbackgroundcolor();
	afx_msg void OnUpdateInsertTreeItem(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeleteTreeItem(CCmdUI* pCmdUI);
	afx_msg void OnViewAdvancedToolbar();
	afx_msg void OnUpdateViewAdvancedToolbar(CCmdUI* pCmdUI);
	afx_msg void OnShowTree();	
	afx_msg void OnUpdateShowTree(CCmdUI* pCmdUI);
	afx_msg void OnShowOI();	
	afx_msg void OnUpdateShowOI(CCmdUI* pCmdUI);
	afx_msg void OnMODSettings();	
	afx_msg void OnExpandTree();
	
	afx_msg void OnUnitEditor();	
	afx_msg void OnUpdateUnitEditor(CCmdUI* pCmdUI);
	afx_msg void OnInfantryEditor();	
	afx_msg void OnUpdateInfantryEditor(CCmdUI* pCmdUI);
	afx_msg void OnSquadEditor();	
	afx_msg void OnUpdateSquadEditor(CCmdUI* pCmdUI);
	afx_msg void OnWeaponEditor();	
	afx_msg void OnUpdateWeaponEditor(CCmdUI* pCmdUI);
	afx_msg void OnMineEditor();	
	afx_msg void OnUpdateMineEditor(CCmdUI* pCmdUI);

	afx_msg void OnParticleEditor();	
	afx_msg void OnUpdateParticleEditor(CCmdUI* pCmdUI);
	afx_msg void OnSpriteEditor();	
	afx_msg void OnUpdateSpriteEditor(CCmdUI* pCmdUI);
	afx_msg void OnEffectEditor();	
	afx_msg void OnUpdateEffectEditor(CCmdUI* pCmdUI);

	afx_msg void OnBuildingEditor();	
	afx_msg void OnUpdateBuildingEditor(CCmdUI* pCmdUI);
	afx_msg void OnObjectEditor();	
	afx_msg void OnUpdateObjectEditor(CCmdUI* pCmdUI);
	afx_msg void OnFenceEditor();	
	afx_msg void OnUpdateFenceEditor(CCmdUI* pCmdUI);
	afx_msg void OnBridgeEditor();	
	afx_msg void OnUpdateBridgeEditor(CCmdUI* pCmdUI);
	afx_msg void OnTrenchEditor();	
	afx_msg void OnUpdateTrenchEditor(CCmdUI* pCmdUI);

	afx_msg void OnMissionEditor();	
	afx_msg void OnUpdateMissionEditor(CCmdUI* pCmdUI);
	afx_msg void OnChapterEditor();	
	afx_msg void OnUpdateChapterEditor(CCmdUI* pCmdUI);
	afx_msg void OnCampaignEditor();	
	afx_msg void OnUpdateCampaignEditor(CCmdUI* pCmdUI);
	afx_msg void OnMedalEditor();	
	afx_msg void OnUpdateMedalEditor(CCmdUI* pCmdUI);

	afx_msg void OnTerrainEditor();	
	afx_msg void OnUpdateTerrainEditor(CCmdUI* pCmdUI);
	afx_msg void OnRoadEditor();	
	afx_msg void OnUpdateRoadEditor(CCmdUI* pCmdUI);
	afx_msg void OnRiverEditor();	
	afx_msg void OnUpdateRiverEditor(CCmdUI* pCmdUI);
	afx_msg void OnSaveObjects();
	// }}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif		// __PARENT_FRAME_H__
