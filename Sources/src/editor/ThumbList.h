#if !defined(AFX_THUMBLIST_H__5F1FB4A5_8A47_46CC_891C_DCA8A658B640__INCLUDED_)
#define AFX_THUMBLIST_H__5F1FB4A5_8A47_46CC_891C_DCA8A658B640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThumbList.h : header file
//

#define IDC_LIST_THUMB_CONTROL 1015
const UINT WM_THUMB_LIST_SELECT	= WM_USER + 25;		// the selected item in the Thumb List control has changed, sent to the parent
const UINT WM_THUMB_LIST_DBLCLK = WM_USER + 26;		// double click on the Thumb List control, sent to parent
const UINT WM_THUMB_LIST_DELETE = WM_USER + 27;		// the user pressed delete in the Thumb List control

const int THUMBNAIL_WIDTH  = 100;
const int THUMBNAIL_HEIGHT = 100;
const int THUMBNAIL_SPACE_X  = 10;
const int THUMBNAIL_SPACE_Y  = 20;


using namespace std;
struct SThumbData
{
	int nImageId;							// item index in image list
	DWORD dwData;							// user data will be stored here, lParam
	string szThumbName;				// item name

	SThumbData() : nImageId(-1), dwData(0), szThumbName("") {}
};
typedef list<SThumbData> CListOfThumbData;

struct SThumbItems
{
	CListOfThumbData thumbDataList;			// vector holding the image data
	int nSelectedItem;

	SThumbItems();
};

// //
// CThumbList window

class CThumbList : public CWnd
{
// Construction
public:
	CThumbList( bool bHorz = false );
	virtual ~CThumbList();
	
// Attributes
private:
	CListCtrl	m_ListThumbnail;
	SThumbItems *m_pActiveThumbItems;
	bool bHorizontal;
	bool bValidIML;

// Operations
public:
	// Sets the current ThumbItems and recreates the ThumbList items
// void ChangeActiveImageList( CImageList *pIML );
	void SetActiveThumbItems( SThumbItems *pNewActiveThumbs, CImageList *pIML );
	// Loads all .tga images from the given directory into pThumbItems
	void LoadAllImagesFromDir( SThumbItems *pThumbItems, CImageList *pIML, const char *szDir, bool bShowAlpha = false );
	// Loads one file into the image list and returns the index of the created item
	int LoadImageToImageList( CImageList *pIML, char *szFileName, const char *szDir, bool bShowAlpha = false );

/* //Loads images with names from the list of names inside pThumbItems from the given directory into pThumbItems
	 */

	// I go through the current list and change the image index for all elements in this list, using the correspondence of the names in the current and in pAllItems
	void LoadImageIndexFromThumbs( SThumbItems *pAllItems, CImageList *pIML );


	// Inserts a new item at the end of the ThumbList
	int InsertItemToEnd( const char *szItemName, int nImageIndex );

	// Inserts a new item after the currently selected ThumbList item
// int InsertItemAfterSelection( char *szFileName, const char *szDir );

	bool HasValidImageList() { return bValidIML; }
	// Returns the current ImageList
	CImageList *GetCurrentImageList() { return m_ListThumbnail.GetImageList(LVSIL_NORMAL); }
	// Finds an item with the given user data among ThumbItems, returns index if found, otherwise -1
	int GetItemIndexWithUserData( DWORD dwData );
	// Returns the index of the selected item
	int GetSelectedItemIndex();
	// Returns the name of the item
	string GetItemName( int nIndex );
	// Returns the image index of the given item
	int GetItemImageIndex( int nIndex );
	// Selects item with nIndex
	void SelectItem( int nIndex );
	// Removes item with given index
	void DeleteItem( int nIndex );
	
	void  SetUserDataForItem( int nItemIndex, DWORD dwData );
	DWORD GetUserDataForItem( int nItemIndex );
	
	int GetThumbsCount() { return m_ListThumbnail.GetItemCount(); }

	void TestInsertSomeItems();
	// Outputs list control
// CListCtrl *GetListCtrl() { return &m_ListThumbnail; 

protected:
	void CreateListElements();

// Overrides
	// ClassWizard generated virtual function overrides
	// {{AFX_VIRTUAL(CThumbList)
	// }}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	// {{AFX_MSG(CThumbList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickListThumb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClickListThumb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListThumb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemStateChanged(NMHDR* pNMHDR, LRESULT* pResult);
	// }}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// //

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THUMBLIST_H__5F1FB4A5_8A47_46CC_891C_DCA8A658B640__INCLUDED_)
