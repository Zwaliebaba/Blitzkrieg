#ifndef __EFFECT_TREE_ITEM_H__
#define __EFFECT_TREE_ITEM_H__

#include "TreeItem.h"

class CEffectTreeRootItem : public CTreeItem
{
	OBJECT_NORMAL_METHODS( CEffectTreeRootItem );
public:
	CEffectTreeRootItem() { bStaticElements = true; nItemType = E_EFFECT_ROOT_ITEM; InitDefaultValues(); }
	~CEffectTreeRootItem() {}
	
	virtual void InitDefaultValues();
};

class CEffectCommonPropsItem : public CTreeItem
{
	OBJECT_NORMAL_METHODS( CEffectCommonPropsItem );
public:
	CEffectCommonPropsItem() { nItemType = E_EFFECT_COMMON_PROPS_ITEM; nImageIndex = 0; InitDefaultValues(); }
	~CEffectCommonPropsItem() {};
	
	// Getting internal parameters
	const char *GetEffectName() { return values[0].value; }
	const char *GetSoundName() { return values[1].value; }

	virtual void InitDefaultValues();
};

class CEffectAnimationsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectAnimationsItem );

public:
	CEffectAnimationsItem() { nItemType = E_EFFECT_ANIMATIONS_ITEM; nImageIndex = 7; InitDefaultValues(); bComplexItem = true; }
	~CEffectAnimationsItem() {};

	// Getting internal parameters
	int GetBeginTime();

	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();

private:
	// Displaying the animations input dialog
	void DisplayInsertChildDialog();
};

class CEffectMeshesItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectMeshesItem );
	
public:
	CEffectMeshesItem() { nItemType = E_EFFECT_MESHES_ITEM; nImageIndex = 2; InitDefaultValues(); bComplexItem = true; }
	~CEffectMeshesItem() {};
	
	// Getting internal parameters
	int GetBeginTime();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();

private:
	// Displaying a dialog for entering meshes
	void DisplayInsertChildDialog();
};

class CEffectFuncParticlesItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectFuncParticlesItem );
	
public:
	CEffectFuncParticlesItem() { nItemType = E_EFFECT_FUNC_PARTICLES_ITEM; nImageIndex = 7; InitDefaultValues(); bComplexItem = true; }
	~CEffectFuncParticlesItem() {};
	
	// Getting internal parameters
	int GetBeginTime();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();
	
private:
	// Displaying a dialog for entering function particles
	void DisplayInsertChildDialog();
};

class CEffectMayaParticlesItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectMayaParticlesItem );
	
public:
	CEffectMayaParticlesItem() { nItemType = E_EFFECT_MAYA_PARTICLES_ITEM; nImageIndex = 4; InitDefaultValues(); bComplexItem = true; }
	~CEffectMayaParticlesItem() {};
	
	// Getting internal parameters
	int GetBeginTime();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();
	
private:
	// Displaying a dialog for entering maya particles
	void DisplayInsertChildDialog();
};

class CEffectLightsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectLightsItem );
	
public:
	CEffectLightsItem() { nItemType = E_EFFECT_LIGHTS_ITEM; nImageIndex = 5; InitDefaultValues(); bComplexItem = true; }
	~CEffectLightsItem() {};
	
	// Getting internal parameters
	int GetBeginTime();

	virtual void InitDefaultValues();
// virtual void MyLButtonClick();
};

class CEffectAnimationPropsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectAnimationPropsItem );
	
public:
	CEffectAnimationPropsItem() { nItemType = E_EFFECT_ANIMATION_PROPS_ITEM; nImageIndex = 0; InitDefaultValues(); }
	~CEffectAnimationPropsItem() {};

	// Getting internal parameters
	CVec3 GetPosition();
	int GetBeginTime();
	int GetRepeatCount();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();
};

class CEffectMeshPropsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectMeshPropsItem );
	
public:
	CEffectMeshPropsItem() { nItemType = E_EFFECT_MESH_PROPS_ITEM; nImageIndex = 2; InitDefaultValues(); }
	~CEffectMeshPropsItem() {};
	
	// Getting internal parameters
	CVec3 GetPosition();
	int GetBeginTime();
	int GetDuration();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
};

class CEffectMayaPropsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectMayaPropsItem );
	
public:
	CEffectMayaPropsItem() { nItemType = E_EFFECT_MAYA_PROPS_ITEM; nImageIndex = 4; InitDefaultValues(); }
	~CEffectMayaPropsItem() {};
	
	// Getting internal parameters
	CVec3 GetPosition();
	int GetBeginTime();
	int GetDuration();
	
	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
};

class CEffectFuncPropsItem : public CTreeItem
{
public:
	OBJECT_NORMAL_METHODS( CEffectFuncPropsItem );
	
public:
	CEffectFuncPropsItem() { nItemType = E_EFFECT_FUNC_PROPS_ITEM; nImageIndex = 0; InitDefaultValues(); }
	~CEffectFuncPropsItem() {};
	
	// Getting internal parameters
	CVec3 GetPosition();
	int GetBeginTime();
	int GetDuration();
	float GetScaleFactor() { return values[5].value; }

	void SetDuration( int nVal ) { values[1].value = nVal; }

	virtual void InitDefaultValues();
	virtual void MyKeyDown( int nChar );
	virtual void MyRButtonClick();
};

#endif		// __EFFECT_TREE_ITEM_H__
