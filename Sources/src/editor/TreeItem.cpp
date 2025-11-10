#include "StdAfx.h"

#include "TreeItem.h"

CTreeItem::CTreeItem() : pTreeCtrl( 0 ), hItem( 0 ), pItemParent ( this )
{
	nItemType = E_UNKNOWN_ITEM;
	bComplexItem = false;
	bStaticElements = false;
	nNeedExpand = false;
	bSerializeChilds = true;
	nImageIndex = 0;
	InitDefaultValues();
	szDisplayName = "";
}

CTreeItem::~CTreeItem()
{
	treeItemList.clear();
}

void CTreeItem::InitDefaultValues()
{
	defaultValues.clear();
	values = defaultValues;
	defaultChilds.clear();
}

bool CTreeItem::IsCompatibleWith( CTreeItem *pCompare )
{
	if ( nItemType == pCompare->GetItemType() )
		return true;

	// —false items are not copied¤
	if ( bComplexItem || pCompare->bComplexItem )
		return false;
	
	if ( values.size() != pCompare->values.size() )
		return false;
	CPropVector::iterator it2 = pCompare->values.begin();
	CPropVector::iterator it  = values.begin();
	for ( ; it!=values.end(); ++it )
	{
		if ( it->nDomenType != it2->nDomenType )
			return false;
		++it2;
	}
	
	return true;
}

bool CTreeItem::CopyItemTo( CTreeItem *pTo )
{
	if ( !IsCompatibleWith(pTo) )
		return false;

	for ( CPropVector::iterator it=values.begin(); it!=values.end(); ++it )
	{
		pTo->UpdateItemValue( it->nId, it->value );
	}
	return true;
}

void CTreeItem::UpdateItemValue( int nItemId, const CVariant &newValue )
{
	CPropVector::iterator it = values.begin();
	for ( ; it!=values.end(); ++it )
	{
		if ( nItemId == it->nId )
		{
			it->value = newValue;
			break;
		}
	}
	NI_ASSERT( it != values.end() );
	NI_ASSERT( pTreeCtrl != 0 );
// pTreeCtrl->SendMessage( WM_USERCHANGEPARAM, nItemId );
// pTreeCtrl->PostMessage( WM_USERCHANGEPARAM, nItemId );
}

void CTreeItem::SelectMeInTheTree()
{
	NI_ASSERT( pTreeCtrl != 0 );
	pTreeCtrl->Select( hItem, TVGN_CARET );
}

CTreeItem *CTreeItem::GetParentTreeItem()
{
	/* NI_ASSERT( pTreeCtrl != 0 );
	 */
	NI_ASSERT( pItemParent != 0 );
	return pItemParent;
}

CTreeItem *CTreeItem::GetSiblingItem( int nType, int nIndex )
{
	HTREEITEM handle = 0, temp = hItem;

	while (	temp = pTreeCtrl->GetNextItem( temp, TVGN_PREVIOUS ) )
		handle = temp;
	if ( handle == 0 )
		return 0;

	int i = 0;
	do
	{
		CTreeItem *pTreeItem = (CTreeItem *) pTreeCtrl->GetItemData( handle );
		if ( pTreeItem->GetItemType() == nType )
			if ( i == nIndex )
				return pTreeItem;
			else
				i++;
	} while ( handle = pTreeCtrl->GetNextItem( handle, TVGN_NEXT ) );
	return 0;
}

void CTreeItem::DeleteMeInParentTreeItem()
{
	CTreeItem *pPapa = GetParentTreeItem();
	pPapa->RemoveChild( this );
}

void CTreeItem::ChangeItemImage( int nNewImage )
{
	NI_ASSERT( pTreeCtrl != 0 );

	pTreeCtrl->SetItemImage( hItem, nNewImage, nNewImage );
}

void CTreeItem::ChangeItemName( const char *pszName )
{
	NI_ASSERT( pTreeCtrl != 0 );
	NI_ASSERT( pszName != 0 );
	
	pTreeCtrl->SetItemText( hItem, pszName );
	szDisplayName = pszName;
}

void CTreeItem::AddChild( CTreeItem *pItem )
{
	NI_ASSERT( pItem != 0 );
// NI_ASSERT( pTreeCtrl != 0 );		
// NI_ASSERT( hItem != 0 );

	treeItemList.push_back( pItem );

	InsertNewTreeItem( pItem );
	pItem->CreateDefaultChilds();
	pItem->InsertChildItems();
}

CTreeItem *CTreeItem::GetChildItem( int nType, int nIndex )
{
	int i = 0;
	for ( CTreeItemList::iterator it=treeItemList.begin(); it!=treeItemList.end(); ++it )
	{
		if ( (*it)->GetItemType() == nType )
			if ( i == nIndex )
				return (*it);
			else
				i++;
	}
	return 0;
}

void CTreeItem::InsertNewTreeItem( CTreeItem *pItem )
{
	pItem->SetParent( this );
	if ( !pTreeCtrl )
	{
		pItem->SetTreeCtrl( 0 );
		pItem->SetHTREEITEM( 0 );
		return;
	}

	// Add a new item to tree ctrl
	TV_INSERTSTRUCT tvis;
	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvis.item.lParam = (LONG) pItem;
	tvis.item.pszText = (char *) pItem->GetItemName();
	tvis.item.iImage = pItem->GetImageIndex();
	tvis.item.iSelectedImage = 8;
	HTREEITEM newHandle = pTreeCtrl->InsertItem( &tvis );
	
	pItem->SetTreeCtrl( pTreeCtrl );
	pItem->SetHTREEITEM( newHandle );
	pTreeCtrl->ReMeasureAllItems();
	pTreeCtrl->Invalidate();
}

void CTreeItem::InsertChildItems()
{
	for ( CTreeItemList::iterator it=treeItemList.begin(); it!=treeItemList.end(); ++it )
	{
		InsertNewTreeItem( (*it) );
		(*it)->InsertChildItems();
	}
	if ( nNeedExpand )
		pTreeCtrl->Expand( hItem, TVE_EXPAND );
}

typedef list<string> CListOfStrings;

class CTreeItemSortFunc
{
	CListOfStrings nameList;
public:
	CTreeItemSortFunc() {}
	void Init( CTreeItem::CChildItemsList &childList )
	{
		CTreeItem::CChildItemsList::iterator it = childList.begin();
		for ( ; it!=childList.end(); ++it )
		{
			nameList.push_back( it->szDisplayName );
		}
	}

	bool operator() ( CTreeItem *item1, CTreeItem *item2 ) const
	{
		CListOfStrings::const_iterator it1 = find( nameList.begin(), nameList.end(), item1->GetItemName() );
		CListOfStrings::const_iterator it2 = find( nameList.begin(), nameList.end(), item2->GetItemName() );
		
		for ( CListOfStrings::const_iterator it=it1; it!=nameList.end(); ++it )
		{
			if ( it == it2 )
				return true;
		}
		return false;
	}
};

class CValuesSortFunc
{
private:
	CListOfStrings nameList;
public:
	CValuesSortFunc() {}
	void Init( CPropVector &valuesVector )
	{
		CPropVector::iterator it = valuesVector.begin();
		for ( ; it!=valuesVector.end(); ++it )
		{
			nameList.push_back( it->szDefaultName );
		}
	}

	bool operator() ( SProp prop1, SProp prop2 ) const
	{
		list<string>::const_iterator it1 = find( nameList.begin(), nameList.end(), prop1.szDefaultName );
		list<string>::const_iterator it2 = find( nameList.begin(), nameList.end(), prop2.szDefaultName );

		for ( list<string>::const_iterator it=it1; it!=nameList.end(); ++it )
		{
			if ( it == it2 )
				return true;
		}
		return false;
	}
};

void CTreeItem::DeleteNullChilds()
{
	for ( CTreeItemList::iterator it=treeItemList.begin(); it!=treeItemList.end(); )
	{
		if ( *it == 0 )
			it = treeItemList.erase( it );
		else
		{
			// call recursively for all children
			(*it)->DeleteNullChilds();
			++it;
		}
	}
}

void CTreeItem::CreateDefaultChilds()
{
	{
		// —create default values
		CPropVector::iterator itemIt;
		CPropVector::iterator defIt;

		if ( values.begin() == values.end() )
		{
			// “In a particular case, if there is no childs in the thrown ones, then I create defaultValues
			for ( defIt=defaultValues.begin(); defIt!=defaultValues.end(); ++defIt )
			{
				values.push_back( *defIt );
			}
		}
		else
		{
			// I go through the already created values ​​and, if one is not in the defaultValues ​​list, then I delete it
			for ( itemIt=values.begin(); itemIt!=values.end(); )
			{
				for ( defIt=defaultValues.begin(); defIt!=defaultValues.end(); ++defIt )
				{
					if ( itemIt->szDefaultName == defIt->szDefaultName )
						break;
				}
				
				if ( defIt == defaultValues.end() )
					itemIt = values.erase( itemIt );
				else
					++itemIt;
			}

			// — I sort the list of thrown values ​​in accordance with the order in defaultValues
			CValuesSortFunc sortFunctor;
			sortFunctor.Init( defaultValues );
			sort( values.begin(), values.end(), sortFunctor );
			
			// I go through the list of defaultValues ​​and, if something is not in the entered values, then I create it
			itemIt = values.begin();
			for ( defIt=defaultValues.begin(); defIt!=defaultValues.end(); ++defIt )
			{
				if ( itemIt == values.end() )
				{
					itemIt = values.insert( itemIt, *defIt );
				}
				else if ( defIt->szDefaultName != itemIt->szDefaultName )
				{
					itemIt = values.insert( itemIt, *defIt );
				}
				else
				{
					// Maybe the id or options in the combobox have changed, I’ll copy them from default values
					itemIt->nId = defIt->nId;
					itemIt->szDisplayName = defIt->szDisplayName;
					itemIt->value.SetType( defIt->value.GetType() );
					itemIt->nDomenType = defIt->nDomenType;
					itemIt->szStrings = defIt->szStrings;
				}

				++itemIt;
			}
		}
	}

	if ( bStaticElements )
	{
		// —create child items by default
		CTreeItemList::iterator itemIt;
		CChildItemsList::iterator defIt;
		IObjectFactory *pFactory = GetCommonFactory();

		if ( treeItemList.begin() == treeItemList.end() )
		{
			// “In a special case, if there are no childs in the thrown ones, then I create defaultChilds
			for ( defIt=defaultChilds.begin(); defIt!=defaultChilds.end(); ++defIt )
			{
				IRefCount *pNewObject = pFactory->CreateObject( defIt->nChildItemType );
				NI_ASSERT( pNewObject != 0 );

				CTreeItem *pTreeItem = (CTreeItem *) pNewObject;
				pTreeItem->szDefaultName = defIt->szDefaultName;
				pTreeItem->szDisplayName = defIt->szDisplayName;
				treeItemList.push_back( pTreeItem );
				pTreeItem->SetParent( this );
			}
		}
		else
		{
			// I go through the already added childs and, if one is not in the defaultChilds list, then I delete it
			for ( itemIt=treeItemList.begin(); itemIt!=treeItemList.end(); )
			{
				for ( defIt=defaultChilds.begin(); defIt!=defaultChilds.end(); ++defIt )
				{
					if ( (*itemIt)->szDefaultName == defIt->szDefaultName && (*itemIt)->nItemType == defIt->nChildItemType )
						break;
				}
				
				if ( defIt == defaultChilds.end() )
					itemIt = treeItemList.erase( itemIt );
				else
					++itemIt;
			}
			// —sort the list of childs in accordance with defaultChilds
			CTreeItemSortFunc sortFunctor;
			sortFunctor.Init( defaultChilds );
			treeItemList.sort( sortFunctor );

			// I go through the list of defaultChilds and, if one is not in the selected childs, then I create it
			itemIt = treeItemList.begin();
			for ( defIt=defaultChilds.begin(); defIt!=defaultChilds.end(); ++defIt )
			{
				if ( itemIt == treeItemList.end() || defIt->szDefaultName != (*itemIt)->szDefaultName || (*itemIt)->nItemType != defIt->nChildItemType )
				{
					IRefCount *pNewObject = pFactory->CreateObject( defIt->nChildItemType );
					NI_ASSERT( pNewObject != 0 );
					
					CTreeItem *pTreeItem = (CTreeItem *) pNewObject;
					pTreeItem->szDefaultName = defIt->szDefaultName;
					pTreeItem->szDisplayName = defIt->szDisplayName;
					itemIt = treeItemList.insert( itemIt, pTreeItem );
					pTreeItem->SetParent( this );
				}
				else
				{
					(*itemIt)->szDisplayName = defIt->szDisplayName;
				}

				++itemIt;
			}
		}
	}

	// recursive call
	for ( CTreeItemList::iterator it=treeItemList.begin(); it!=treeItemList.end(); ++it )
	{
		(*it)->CreateDefaultChilds();
	}
}

void CTreeItem::RemoveChild( CTreeItem *pItem )
{
	NI_ASSERT( pItem != 0 );
	NI_ASSERT( pTreeCtrl != 0 );
	NI_ASSERT( pItem->GetHTREEITEM() != 0 );

	pTreeCtrl->DeleteItem( pItem->GetHTREEITEM() );
	treeItemList.remove( pItem );

	// find the next item that will be selected when the current one is deleted
	HTREEITEM hSel = pTreeCtrl->GetSelectedItem();
	if ( !hSel )
		return;
	CTreeItem *pSelItem = (CTreeItem *) ( pTreeCtrl->GetItemData( hSel ) );
	pSelItem->SelectMeInTheTree();
}

void CTreeItem::RemoveAllChilds()
{
	if ( pTreeCtrl != 0 )
	{
		for ( CTreeItemList::iterator it=treeItemList.begin(); it!=treeItemList.end(); ++it )
		{
			// “let’s make it from wood”
			pTreeCtrl->DeleteItem( (*it)->GetHTREEITEM() );
		}
	}

// NI_ASSERT( pTreeCtrl != 0 );
	treeItemList.clear();
}

int CTreeItem::operator&( interface IStructureSaver &ss )
{
	CSaverAccessor saver = &ss;

	// —we protect the type
	saver.Add( 1, &nItemType );
	// —we protect it by default
	saver.Add( 2, &szDefaultName );
	// —we protect what is displayed by it¤
	saver.Add( 3, &szDisplayName );

	// —we protect the values
	saver.Add( 4, &values );

	// —we protect the internal list - child items
	saver.Add( 5, &treeItemList );

	// —we protect the expand state
	if ( saver.IsReading() )
	{
		saver.Add( 6, &nNeedExpand );
		// expand it in the InsertChildItems function
	}
	else
	{
		int nState = pTreeCtrl->GetItemState( hItem, TVIS_EXPANDED );
		if ( nState == TVIS_EXPANDED )
			nNeedExpand = true;
		else
			nNeedExpand = false;
		saver.Add( 6, &nNeedExpand );
	}

	return 0;
}


int SProp::operator&( IDataTree &ss )
{
	CTreeAccessor saver = &ss;
	saver.Add( "default_name", &szDefaultName );
	saver.Add( "value", &value );
	return 0;
}

int CTreeItem::operator&( IDataTree &ss )
{
	CTreeAccessor saver = &ss;
	saver.Add( "default_name", &szDefaultName );
	saver.Add( "display_name", &szDisplayName );
	saver.Add( "values", &values );
	// —we protect the expand state
	if ( saver.IsReading() )
	{
		saver.Add( "expand", &nNeedExpand );
		// expand it in the InsertChildItems function
	}
	else
	{
		int nState = pTreeCtrl->GetItemState( hItem, TVIS_EXPANDED );
		if ( nState == TVIS_EXPANDED )
			nNeedExpand = true;
		else
			nNeedExpand = false;
		saver.Add( "expand", &nNeedExpand );
	}
	// save childs
	if ( bSerializeChilds )
		saver.Add( "childs", &treeItemList );
	if ( saver.IsReading() )
	{
		for ( CTreeItemList::iterator it = treeItemList.begin(); it != treeItemList.end(); ++it )
			(*it)->SetParent( this );
	}
	return 0;
}


void CTreeItem::ExpandTreeItem( bool bExpand )
{
	nNeedExpand = bExpand;
	if ( bExpand )
	{
		pTreeCtrl->Expand( hItem, TVE_EXPAND );
	}
	else
		pTreeCtrl->Expand( hItem, TVE_COLLAPSE );
}

/* void Serialize( CTreeAccessor *pFile, CTreeItem *pData )
 */

int CKeyFrameTreeItem::operator&( IDataTree &ss )
{
	CTreeAccessor saver = &ss;
	saver.AddTypedSuper( static_cast<CTreeItem*>(this) );
	saver.Add( "Key_frames", &framesList );
	return 0;
}
// ////////////////////////////////
