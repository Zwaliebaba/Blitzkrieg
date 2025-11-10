#ifndef __FENCE_TREE_ITEM_H__
#define __FENCE_TREE_ITEM_H__

#include "..\GFX\GFX.h"
#include "..\Main\rpgstats.h"
#include "GridFrm.h"
#include "TreeItem.h"
#include "ThumbList.h"

class CFenceTreeRootItem : public CTreeItem
{
	OBJECT_NORMAL_METHODS( CFenceTreeRootItem );
public:
	CFenceTreeRootItem() { bStaticElements = true; nItemType = E_FENCE_ROOT_ITEM; InitDefaultValues(); }
	~CFenceTreeRootItem() {}
	
	virtual void InitDefaultValues();
	void ComposeFences( const char *pszProjectFileName, const char *pszResultingDir, SFenceRPGStats &rpgStats );
	
private:
	bool SaveShadowFile( const string &szFenceFileName, const string &szShadowFileName, const string &szTempShadow );
};

class CFenceCommonPropsItem : public CTreeItem
{
private:
	SThumbItems m_thumbItems;			// these items are displayed in AllDirThumbList
	CImageList imageList;
	
	OBJECT_NORMAL_METHODS( CFenceCommonPropsItem );
public:
	CFenceCommonPropsItem() { nItemType = E_FENCE_COMMON_PROPS_ITEM; InitDefaultValues(); nImageIndex = 0; imageList.Create(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, ILC_COLOR24, 0, 1); }
	~CFenceCommonPropsItem() {};
	
	SThumbItems* GetThumbItems() { return &m_thumbItems; }
	CImageList*  GetImageList() { return &imageList; }
	
	// Getting internal parameters
	const char *GetFenceName() { return values[0].value; }
	const char *GetDirName() { return values[1].value; }
	int GetFenceHealth() { return values[2].value; }
	int GetFenceAbsorbtion() { return values[3].value; }
	int GetPassForInfantry() { return values[4].value; }
	int GetPassForWheels() { return values[5].value; }
	int GetPassForHalfTracks() { return values[6].value; }
	int GetPassForTracks() { return values[7].value; }
	
	virtual void InitDefaultValues();
	virtual void UpdateItemValue( int nItemId, const CVariant &value );
};

class CFenceDirectionItem : public CTreeItem
{
	OBJECT_NORMAL_METHODS( CFenceDirectionItem );
public:
	CFenceDirectionItem() { bStaticElements = true; nItemType = E_FENCE_DIRECTION_ITEM; InitDefaultValues(); nImageIndex = 2; }
	~CFenceDirectionItem() {};
	
	virtual void InitDefaultValues();
};

class CFenceInsertItem : public CTreeItem
{
	SThumbItems m_thumbItems;			// these items are displayed in SelectedThumbList
	bool bLoaded;									// this flag is for paging items only when the user selects the folder c roads
	OBJECT_NORMAL_METHODS( CFenceInsertItem );
public:
	CFenceInsertItem() { nItemType = E_FENCE_INSERT_ITEM; bLoaded = false; InitDefaultValues(); nImageIndex = 7; }
	~CFenceInsertItem() {};
	
	void SetLoadedFlag( bool bState ) { bLoaded = bState; }
	bool GetLoadedFlag() { return bLoaded; }
	SThumbItems* GetThumbItems() { return &m_thumbItems; }
	
	virtual void InsertChildItems();					// Called after all components have been created to add them to the tree
	virtual void InitDefaultValues();
	virtual void MyLButtonClick();
};

class CFencePropsItem : public CTreeItem
{
	OBJECT_NORMAL_METHODS( CFencePropsItem );
	
public:
	CVec3 vSpritePos;		// I don’t want to carry a bunch of functions, TT
	CListOfTiles lockedTiles;
	CListOfTiles transeparences;
	bool bLoaded;
	int nSegmentIndex;

	CFencePropsItem() {
		nItemType = E_FENCE_PROPS_ITEM;
		vSpritePos = CVec3( 16*fWorldCellSize, 16*fWorldCellSize, 0 );
		InitDefaultValues();
		nImageIndex = 3;
		bLoaded = false;
		nSegmentIndex = -1;
	}
	~CFencePropsItem() {};

// void FillRPGStruct( SFenceRPGStats::SSegmentRPGStats &rpgStats );
	
	virtual void InitDefaultValues();
	virtual int operator&( IDataTree &ss );
	virtual void MyKeyDown( int nChar );
	virtual void MyLButtonClick();
};

#endif		// __FENCE_TREE_ITEM_H__
