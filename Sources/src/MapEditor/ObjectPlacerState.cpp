// ObjectPlacerState.cpp: implementation of the CObjectPlacerState class.
//
// /////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editor.h"
#include "ObjectPlacerState.h"
#include "PropertieDialog.h"

#include "TemplateEditorFrame1.h"
#include "frames.h"
#include "GameWnd.h"
#include "MainFrm.h"
#include "..\GFX\GFX.h"
#include "..\Scene\Terrain.h"
#include "..\Image\Image.h"
#include "..\Scene\Scene.h"
#include "IUndoRedoCmd.h"
#include "PropertieDialog.h"
#include "resource.h"
#include "..\Anim\Animation.h"
#include "SetupFilterDialog.h"

#include "..\RandomMapGen\LA_Types.h"

#include "SetAnim.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// /extern int GetAnimationFrameIndex( IVisObj *pVisObj );
CObjectPlacerState::CObjectPlacerState()
{
}

inline int GetFrameIndexForObj( SMapObject *obj,  CTemplateEditorFrame *frame  )
{
		return frame->GetAnimationFrameIndex( obj->pVisObj );
}


void CObjectPlacerState::OnMouseMove(UINT nFlags, const CTPoint<int> &rMousePoint, CTemplateEditorFrame *frame )
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_MOUSEMOVE, rMousePoint, frame ) )
	{
		return;
	}

	if ( ( nFlags & MK_LBUTTON ) == 0 )
	{
		GetSingleton<IScene>()->GetFrameSelection()->End();
	}

	CPoint point( rMousePoint.x, rMousePoint.y );
	frame->m_cursorName = MAKEINTRESOURCE( IDC_ARROW );

	if ( frame->dlg && frame->isStartCommandPropertyActive )
	{
		// frame->DrawAIStartCommandRedLines();
		// frame->DrawUnitsSelection();
		frame->RedrawWindow();
		return;
	}
	
	/* *
	 */

	RECT r;
	g_frameManager.GetGameWnd()->GetClientRect( &r );
	point.x -=  r.left;
	point.y -=  r.top ;
	CVec3 v;

	IScene *pScene = GetSingleton<IScene>();
	if ( pScene == 0 ) return;
	ITerrain *pTerrain = pScene->GetTerrain();
	ITerrainEditor *pTerrainEditor = dynamic_cast<ITerrainEditor*>( pTerrain );
	STerrainInfo &rTerrainInfo = const_cast<STerrainInfo&>( pTerrainEditor->GetTerrainInfo() );

	if ( pTerrain == 0 ) return;
	CVec2 p ( point.x , point.y );
	pScene->GetPos3( &v, p );

	bool bFrameDrawing = ( frame->m_currentMovingObjectForPlacementPtr == 0 ) && ( nFlags & MK_LBUTTON ) && frame->m_ifCanMultiSelect;
	if ( !bFrameDrawing )
	{
		if ( !stateParameter.LastPointInTerrain() )
		{
			return;	
		}
	}
	
	// Carrying an object behind the mouse
	if ( frame->m_currentMovingObjectPtrAI && ( nFlags & MK_LBUTTON ) )
	{
		p += frame->m_currentObjectShift;
		GetSingleton<IScene>()->GetPos3( &v, p );
		CVec3 vAI;
		if ( frame->ifFitToAI && frame->m_currentMovingObjectPtrAI->pDesc->eGameType != SGVOGT_UNIT 
			  && frame->m_currentMovingObjectPtrAI->pDesc->eGameType != SGVOGT_ENTRENCHMENT 
			) 
		{
			CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
			const SObjectBaseRPGStats *pObjDesc = static_cast< const SObjectBaseRPGStats*>( pODB->GetRPGStats( frame->m_currentMovingObjectPtrAI->pDesc ) );
			//
			
			FitVisOrigin2AIGrid( &v, pObjDesc->GetOrigin( GetFrameIndexForObj( frame->m_currentMovingObjectPtrAI, frame  )) );
		}
 
		Vis2AI( &vAI, v );
		if( frame->m_currentMovingObjectPtrAI->pAIObj )
		{
			// GetSingleton<IAIEditor>()->MoveObject(frame-> m_currentMovingObjectPtrAI->pAIObj, int( vAI.x ) , int ( vAI.y ) );
				frame->MoveObject(frame->m_currentMovingObjectPtrAI->pAIObj, int( vAI.x ) , int ( vAI.y ) );
		}

		// when we carry a group of objects, we need to check who we are carrying over (i.e. whether we need to put a little man there)
		std::pair<IVisObj*, CVec2> *pObjects;
		int num;
		frame->Pick( p, &pObjects, &num, SGVOGT_UNKNOWN, true, true );
		
		// in the pick group you need to see if you clicked on an object that can plant this soldier
		for ( int i = 0; i != num; ++i )
		{
			if( pObjects[i].first != frame->m_currentMovingObjectPtrAI->pVisObj )
			{
				std::vector< SMapObject* > v;v.push_back( frame->m_currentMovingObjectPtrAI );
				if(	CheckForInserting( v, frame->FindByVis(pObjects[i].first ) ) )
				{
					frame->m_cursorName = MAKEINTRESOURCE( IDC_UPARROW );
					break;
				}
			}
		}

		IGameTimer *pTimer = GetSingleton<IGameTimer>();
		pTimer->Update( timeGetTime() );
		frame->Update( pTimer->GetGameTime() );
		frame->SetMapModified();
	} 

	// If we are transporting a group of objects
	if ( frame->m_ifCanMovingMultiGroup && ( nFlags & MK_LBUTTON ) )
	{
		GetSingleton<IScene>()->GetPos3( &v, p );
		std::set<IRefCount*> movedSquads;
		for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
		{
			CVec3 vAI = v;
			if ( (*it)->pAIObj )
			{
				IRefCount* obj = GetSingleton<IAIEditor>()->GetFormationOfUnit( (*it)->pAIObj ) ;
				if ( obj != 0 )
				{
					if ( movedSquads.find( obj ) == movedSquads.end() )
					{
						if ( frame->m_squadsShiftsForMovingObjectsAI.find( obj ) != frame->m_squadsShiftsForMovingObjectsAI.end() )
						{
							vAI.x += frame->m_squadsShiftsForMovingObjectsAI[obj].x;
							vAI.y += frame->m_squadsShiftsForMovingObjectsAI[obj].y;
						}
						Vis2AI( &vAI );
						frame->MoveObject( obj, vAI.x, vAI.y, true );	
						movedSquads.insert( obj );
					}
				}
				else
				{
					if ( frame->m_shiftsForMovingObjectsAI.find( *it ) != frame->m_shiftsForMovingObjectsAI .end() )
					{
						vAI.x += frame->m_shiftsForMovingObjectsAI[*it].x;
						vAI.y += frame->m_shiftsForMovingObjectsAI[*it].y;
					}
					if ( frame->ifFitToAI && ( *it )->pDesc->eGameType != SGVOGT_UNIT ) 
					{
						CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
						const SObjectBaseRPGStats *pObjDesc = static_cast< const SObjectBaseRPGStats*>( pODB->GetRPGStats( (*it)->pDesc ) );
						//
						FitVisOrigin2AIGrid( &vAI, pObjDesc->GetOrigin( GetFrameIndexForObj( (*it), frame ) )/* vOrigin */);
					}
					Vis2AI( &vAI );
					frame->MoveObject( (*it)->pAIObj, vAI.x, vAI.y );
				}
			}
			IGameTimer *pTimer = GetSingleton<IGameTimer>();
			pTimer->Update( timeGetTime() );
			frame->Update( pTimer->GetGameTime() );
			frame->SetMapModified();
		}
		movedSquads.clear();
		// when we carry a group of objects, we need to check who we are carrying over (i.e. whether we need to put a little man there)
		std::pair<IVisObj*, CVec2> *pObjects;
		int num;
		frame->Pick( p, &pObjects, &num, SGVOGT_UNKNOWN, true, true );
		
		if ( num )
		{
			// if there is a pick element in the selected group, then you need to enable the “drag group” mode
			for ( int i = 0; i != num; ++i )
			{
				bool ifInGroup = false;
				for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
				{
						if( (*it)->pVisObj == pObjects[i].first )
							ifInGroup = true;
				}
				if( !ifInGroup )
				{
					// pObjects[i].first - it does not belong to the selected group
					if(	CheckForInserting( frame->m_currentMovingObjectsAI, frame->FindByVis(pObjects[i].first ) ) )
					{
						frame->m_cursorName = MAKEINTRESOURCE( IDC_UPARROW );
						break;
					}
				}
			}
		}
	}	
	
	// We are not placing anything now ( == !m_currentMovingObjectForPlacementPtr )
	// and the mouse is pressed -> it means now we select it with a frame
	if ( ( frame->m_currentMovingObjectForPlacementPtr == 0 ) && ( nFlags & MK_LBUTTON ) && frame->m_ifCanMultiSelect )
	{
		frame->GetTileIndexBy2DPoint( point.x, point.y, frame->m_lastSelectPoint.x, frame->m_lastSelectPoint.y );
		ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
		const STerrainInfo	&info = dynamic_cast<ITerrainEditor *>(terra)->GetTerrainInfo();
		ValidatePoint( CTRect<int>( 0, 0, info.tiles.GetSizeX(), info.tiles.GetSizeY() ), &( frame->m_lastSelectPoint ) ); 		

		GRect r( 
						Min( frame->m_lastSelectPoint.x, frame->m_firstSelectPoint.x ),
						Min( frame->m_lastSelectPoint.y, frame->m_firstSelectPoint.y ),
						Max( frame->m_lastSelectPoint.x, frame->m_firstSelectPoint.x ) + 1,
						Max(frame-> m_lastSelectPoint.y, frame->m_firstSelectPoint.y ) + 1
						);
		std::vector< CTPoint<int> > pointsForUpdate;
		frame->FillGRect( r, pointsForUpdate );

		
		CVec2 selectedPoint( point.x , point.y );
		CVec3 selectedVec3;
 		GetSingleton<IScene>()->GetPos3( &selectedVec3, selectedPoint );
		
		// highlight with a square or diamond
		if(  ( GetAsyncKeyState( VK_CONTROL  ) & 32768 ) )
		{
			if ( pointsForUpdate.size() )
			{
				dynamic_cast<ITerrainEditor*>( GetSingleton<IScene>()->GetTerrain() )->SetMarker( &pointsForUpdate[0], pointsForUpdate.size() );
			}
			else
				dynamic_cast<ITerrainEditor*>( GetSingleton<IScene>()->GetTerrain() )->SetMarker( NULL, 0 );
		}
		else
		{
			GetSingleton<IScene>()->GetFrameSelection()->Update( selectedVec3 );
		}
	}
	ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
	GRect terrainRect( 0, 0 , 0, 0 );
	if ( terra )
	{
		const STerrainInfo	&info = dynamic_cast<ITerrainEditor *>(terra)->GetTerrainInfo();
		terrainRect = GRect( 0, 0 , info.tiles.GetSizeX() * fWorldCellSize - 1, info.tiles.GetSizeY() * fWorldCellSize - 1 );
	}		
	// draw a translucent picture behind the cursor
	if ( frame->m_currentMovingObjectForPlacementPtr == 0 )
	{
		const int picNum = frame->m_mapEditorBarPtr->GetObjectWnd()->GetObjectIndex();
		if ( picNum >= 0 )
		{	
			GetSingleton<IScene>()->GetPos3( &v, p );
			IVisObjBuilder *pVOB = GetSingleton<IVisObjBuilder>();
			CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
			int nImageCount = pODB->GetNumDescs();
			const SGDBObjectDesc *descPtr = pODB->GetAllDescs(); 				
			if ( terrainRect.contains( v.x, v.y ) )// you can't draw outside
			{
				// IVisObj *pVisObj = pVOB->BuildObject( descPtr[picNum].szPath.c_str(), descPtr[picNum].eVisType );
				IVisObj *pVisObj = frame->AddObject( descPtr[picNum], 0, true );
				if ( pVisObj )
				{
					if ( descPtr[picNum].eGameType == SGVOGT_UNIT)
					{
						SetAnim( pVisObj, &( descPtr[picNum] ), 0 );
						pVisObj->SetDirection( frame->m_mapEditorBarPtr->GetObjectWnd()->GetDefaultDirAngel() * 182.04f );
					}
					pVisObj->SetPlacement( v, frame->m_mapEditorBarPtr->GetObjectWnd()->GetDefaultDirAngel() * 182.04f );

					GetSingleton<IScene>()->AddObject( pVisObj, descPtr[picNum].eGameType, &descPtr[picNum] );
					frame->m_currentMovingObjectForPlacementPtr = pVisObj;
				}
			}
		}
		
	}
	else
	{
		POSITION pos = frame->m_mapEditorBarPtr->GetObjectWnd()->m_imageList.GetFirstSelectedItemPosition( );
		GetSingleton<IScene>()->GetPos3( &v, p );
		if ( pos != NULL ) 
		{				
			GetSingleton<IScene>()->MoveObject( frame->m_currentMovingObjectForPlacementPtr, v );
			frame->m_currentMovingObjectForPlacementPtr->SetOpacity( 128 );
			IGameTimer *pTimer = GetSingleton<IGameTimer>();
			pTimer->Update( timeGetTime() );
			frame->m_currentMovingObjectForPlacementPtr->Update( pTimer->GetGameTime() );
		}
		else
		{
			frame->RemoveObject( frame->m_currentMovingObjectForPlacementPtr );
			frame->m_currentMovingObjectForPlacementPtr = 0;
		}
	}
	// frame->DrawUnitsSelection();
	frame->RedrawWindow();
}


void CObjectPlacerState::OnLButtonDown( UINT nFlags, const CTPoint<int> &rMousePoint, CTemplateEditorFrame *frame )
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_LBUTTONDOWN, rMousePoint, frame ) )
	{
		return;
	}

	CPoint point( rMousePoint.x, rMousePoint.y );
	if ( frame->dlg && frame->isStartCommandPropertyActive )
	{
		frame->RedrawWindow();
		return;
	}

	CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
	IVisObjBuilder *pVOB = GetSingleton<IVisObjBuilder>();
	int nImageCount = pODB->GetNumDescs();
	const SGDBObjectDesc *descPtr = pODB->GetAllDescs(); 
	//
	RECT r;
	g_frameManager.GetGameWnd()->GetClientRect( &r );
	point.x -= r.left;
	point.y -= r.top;
	CVec3 v;
	
	IScene *pScene = GetSingleton<IScene>();
	ITerrain *pTerrain = pScene->GetTerrain();
	ITerrainEditor *pTerrainEditor = dynamic_cast<ITerrainEditor*>( pTerrain );
	STerrainInfo &rTerrainInfo = const_cast<STerrainInfo&>( pTerrainEditor->GetTerrainInfo() );

	CVec2 p ( point.x , point.y );
	pScene->GetPos3( &v, p );

	const int picNum = frame->m_mapEditorBarPtr->GetObjectWnd()->GetObjectIndex();
	if ( picNum >= 0 )
	{	
		if ( ( v.x <= fWorldCellSize ) || 
				 ( v.y <= fWorldCellSize / 2.0f ) || 
				 ( v.x >= ( ( rTerrainInfo.tiles.GetSizeX() * fWorldCellSize ) - fWorldCellSize / 2.0f ) ) || 
				 ( v.y >= ( ( rTerrainInfo.tiles.GetSizeY() * fWorldCellSize ) - fWorldCellSize ) ) )
		{
			frame->RedrawWindow();
			return;	
		}

		// put the object
		if ( frame->m_currentMovingObjectPtrAI ) 
			frame->m_currentMovingObjectPtrAI->pVisObj->Select( SGVOSS_UNSELECTED );

		int picNum = frame->m_mapEditorBarPtr->GetObjectWnd()->GetObjectIndex();
		
		SMapObjectInfo info;
		info.szName = descPtr[picNum].szKey;
		info.nDir = frame->m_mapEditorBarPtr->GetObjectWnd()->GetDefaultDirAngel() * 182.04f ;
		info.nPlayer = frame->m_mapEditorBarPtr->GetObjectWnd()->GetPlayer();
		info.nScriptID = -1;
		info.fHP = 1.0f;
		// info.szLogic = "";
		const SGDBObjectDesc *pDesc = pODB->GetDesc( info.szName.c_str() );
		const SObjectBaseRPGStats *pObjDesc = static_cast< const SObjectBaseRPGStats*>( pODB->GetRPGStats( pDesc ) );
		
		if ( frame->ifFitToAI &&  pDesc->eGameType != SGVOGT_UNIT && pDesc->eGameType != SGVOGT_SQUAD ) 
			FitVisOrigin2AIGrid( &v, pObjDesc->GetOrigin( -1 ) );

		info.vPos = v;
		Vis2AI( &info.vPos );

		bool bNeedUpdateStorages = false;
		if ( pDesc->eGameType == SGVOGT_FLAG )
		{
			frame->SetPlayerForFlag( &info );
		}
		else if ( pDesc->eGameType == SGVOGT_BUILDING )
		{
			CGDBPtr<SBuildingRPGStats> pStats = dynamic_cast<const SBuildingRPGStats*>( pODB->GetRPGStats( pDesc ) );
			if ( ( pStats->eType == SBuildingRPGStats::TYPE_MAIN_RU_STORAGE ) ||
					 ( pStats->eType == SBuildingRPGStats::TYPE_TEMP_RU_STORAGE ) )
			{
				bNeedUpdateStorages = true;
			}
		}

		frame->AddObjectByAI( info );
		frame->SetMapModified();

		if ( bNeedUpdateStorages )
		{
			frame->ShowStorageCoverage();
		}
		else if ( g_frameManager.GetMiniMapWindow() )
		{
			g_frameManager.GetMiniMapWindow()->UpdateMinimap( true );
		}
		frame->ShowFireRange( frame->bFireRangePressed );		
	}
	else 
	{
		// select the object
		frame->GetTileIndexBy2DPoint( point.x, point.y, frame->m_firstSelectPoint.x, frame->m_firstSelectPoint.y );
		ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
		const STerrainInfo	&info = dynamic_cast<ITerrainEditor *>(terra)->GetTerrainInfo();
		ValidatePoint( CTRect<int>( 0, 0, info.tiles.GetSizeX(), info.tiles.GetSizeY() ), &( frame->m_firstSelectPoint ) ); 		
		std::pair<IVisObj*, CVec2> *pObjectsTmp;
		int num;
		frame->Pick( p, &pObjectsTmp, &num, SGVOGT_UNKNOWN );
		
		std::vector< std::pair<IVisObj*, CVec2> > pObjects( pObjectsTmp, pObjectsTmp + num );
		for ( std::vector< std::pair<IVisObj*, CVec2> >::iterator it = pObjects.begin(); it != pObjects.end(); )
		{
			SMapObject *pMapObject = frame->FindByVis( it->first );
			if ( pMapObject )
			{
				IRefCount *pSquad = GetSingleton<IAIEditor>()->GetFormationOfUnit( pMapObject->pAIObj );
				if ( pSquad )
				{
					SEditorObjectItem *tmpEditiorObj =	frame->m_objectsAI[ pMapObject ];
					if ( tmpEditiorObj )
					{
						if( tmpEditiorObj->pLink )
						{
							if( tmpEditiorObj->pLink->IsValid() )
							{
								it = pObjects.erase( it );
								continue;
							}
						}
					}
				}
			}
			++it;
		}
		num = pObjects.size();
		if ( !pObjects.empty() )
		{
			// if there is a pick element in the selected group, then you need to enable the “drag group” mode
			for ( int i = 0; i != num; ++i )
			{
				for ( std::vector<SMapObject*>::iterator it2 = frame->m_currentMovingObjectsAI.begin(); it2 != frame->m_currentMovingObjectsAI.end(); ++it2 )
				{
					if( (*it2)->pVisObj == pObjects[i].first )
					{
						frame->m_ifCanMovingMultiGroup = true;	
						frame->SetFocus();
						CalculateShiftsForMultiSelect( frame, v );
						frame->RedrawWindow();
						return;
					}
				} 
			
			}
			// If you clicked, you just need to add the person to the group
			if(  ( GetAsyncKeyState( VK_CONTROL  ) & 32768 ) )
			{
				CalculateObjectSelection( GetObjectsSelection( frame->FindByVis( pObjects[ 0 ].first ), frame ), frame );				
			}
			else
			{
				// if something was selected then it needs to be reset
				ClearAllSelection( frame );
				bool bSquad = false;
				std::vector<SMapObject*>	vSelObjects = GetObjectsSelection( frame->FindByVis( pObjects[ 0 ].first ), frame, &bSquad );
				if ( vSelObjects.size() == 1 && !bSquad )
				{	// i.e. they bumped into a regular unit
					CalculateObjectSelection( vSelObjects, frame, VNULL3, bSquad );
					frame->m_currentObjectShift = pObjects[ 0 ].second;
				}
				else
				{
					// fell into a platoon of several soldiers
					frame->m_ifCanMovingMultiGroup = true;	
					CalculateObjectSelection( vSelObjects, frame, VNULL3, bSquad );
					CalculateShiftsForMultiSelect( frame, v );
				}
				
				// info for undo (moving)
				if( frame->m_currentMovingObjectPtrAI )
					frame->m_oldMovingPosition = CPoint( frame->m_currentMovingObjectPtrAI->pVisObj->GetPosition().x,frame-> m_currentMovingObjectPtrAI->pVisObj->GetPosition().y);
			}
			// fill out the window with properties for this unit
			if ( !frame->isStartCommandPropertyActive )
			{
				if ( frame->m_currentMovingObjectPtrAI )
				{
					UpdatePropertie( frame->GetEditorObjectItem( frame->m_currentMovingObjectPtrAI ), frame  );
					// frame->DrawUnitsSelection();
					frame->RedrawWindow();		
				}
				else
				{
					UpdatePropertie( 0, frame  );
					// frame->DrawUnitsSelection();
					frame->RedrawWindow();		
				}
			}

			if ( CMainFrame* pMainFrame = theApp.GetMainFrame() )
			{
				int currentSel = pMainFrame->pwndFireRangeFilterComboBox->GetCurSel();
				if ( currentSel != CB_ERR  )
				{
					CString currentFilter;
					pMainFrame->pwndFireRangeFilterComboBox->GetLBText( currentSel, currentFilter );	
					if ( currentFilter.Compare( CSetupFilterDialog::SELECTED_UNITS ) == 0 )
					{
						frame->ShowFireRange( frame->bFireRangePressed );		
					}
				}
			}
		}
		else
		{
			if ( !frame->isStartCommandPropertyActive )
			{
				if ( frame->dlg )
				{
					frame->dlg->DestroyWindow();
					delete frame->dlg;
					frame->dlg = 0;
				}
			}

			// multiselect works only when nothing is selected (i.e. the mouse didn’t hit anything)
			ClearAllSelection( frame );
			frame->m_ifCanMultiSelect = true;
			CVec2 selectedPoint( point.x , point.y );
			CVec3 selectedVec3;
			GetSingleton<IScene>()->GetPos3( &selectedVec3, selectedPoint );

			GetSingleton<IScene>()->GetFrameSelection()->Begin( selectedVec3 );

		}
		frame->m_curPickNum = 0;
		frame->m_pickedObjects.clear();
		for ( int i = 0; i != num; ++i )
			frame->m_pickedObjects.push_back( pObjects[i] );
	}
	
	if ( frame->isReservePositionActive )
	{
		if ( frame->m_currentMovingObjectPtrAI )
		{
			// add an element
			bool isNotExists = true;
			TMutableReservePositionList::iterator it = frame->m_reservePositions.begin();
			for ( ; it != frame->m_reservePositions.end(); ++it )
			{
				if ( ( it->pArtilleryObject == frame->m_currentMovingObjectPtrAI ) ||
						 ( it->pTruckObject == frame->m_currentMovingObjectPtrAI ) )
				{
					isNotExists = false;
					break;
				}
			}
			if ( isNotExists )
			{
				if ( frame->m_currentMovingObjectPtrAI->pDesc->eGameType == SGVOGT_UNIT )
				{
					CGDBPtr<SMechUnitRPGStats> pArtilleryStats = 0;
					if ( frame->m_CurentReservePosition.pArtilleryObject )
					{
						pArtilleryStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( frame->m_CurentReservePosition.pArtilleryObject->pDesc ) );
					}
					CGDBPtr<SMechUnitRPGStats> pTruckStats = 0;
					if ( frame->m_CurentReservePosition.pTruckObject )
					{
						pTruckStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( frame->m_CurentReservePosition.pTruckObject->pDesc ) );
					}
					CGDBPtr<SMechUnitRPGStats> pStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( frame->m_currentMovingObjectPtrAI->pDesc ) );

					if ( pArtilleryStats || pTruckStats )
					{
						if ( pArtilleryStats &&
								 ( ( pStats->type == RPG_TYPE_TRN_CARRIER ) || ( pStats->type == RPG_TYPE_TRN_TRACTOR ) ) &&
								 ( !pArtilleryStats->vPeoplePoints.empty() ) &&
								 IsArtillery( pArtilleryStats->type ) &&
							   ( pStats->fTowingForce > pArtilleryStats->fWeight ) )
						{
							frame->AddTruckToCurrentReservePosition();
						}
						else if ( ( pTruckStats == 0 ) &&
											( IsArtillery( pStats->type ) || 
												IsSPG( pStats->type ) || 
												IsArmor( pStats->type ) ||
												( pStats->type == RPG_TYPE_TRAIN_SUPER ) ) )
						{
							frame->AddArtilleryToCurrentReservePosition();
						}
						else if ( pTruckStats &&
											IsArtillery( pStats->type ) &&
											( !pStats->vPeoplePoints.empty() ) && 
											( pTruckStats->fTowingForce > pStats->fWeight ) )
						{
							frame->AddArtilleryToCurrentReservePosition();
						}
					}
					else
					{
						if ( pStats )
						{
							if ( ( pStats->type == RPG_TYPE_TRN_CARRIER ) || 
									 ( pStats->type == RPG_TYPE_TRN_TRACTOR ) )
							{
								frame->AddTruckToCurrentReservePosition();
							}
							else if ( IsArtillery( pStats->type ) || 
												IsSPG( pStats->type ) || 
												IsArmor( pStats->type ) ||
												( pStats->type == RPG_TYPE_TRAIN_SUPER ) )
							{
								frame->AddArtilleryToCurrentReservePosition();
							}
						}
					}
				}
			}
			else
			{
				if ( ( frame->m_CurentReservePosition.vPos == VNULL2 ) &&
					   ( frame->m_CurentReservePosition.pArtilleryObject == 0 ) &&
						 ( frame->m_CurentReservePosition.pTruckObject == 0 )  && 
						 ( it != frame->m_reservePositions.end() ) )
				{
					frame->m_CurentReservePosition = *it;
					frame->m_reservePositions.erase( it );
					frame->SetMapModified();
				}
			}
		}
		else
		{
			CVec3 v;
			CVec2 p ( point.x , point.y );
			GetSingleton<IScene>()->GetPos3( &v, p );
			Vis2AI( &v );
			frame->AddPosToCurrentReservePosition( v );
		}
	}
	frame->RedrawWindow();
}


void CObjectPlacerState::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags, CTemplateEditorFrame *frame)
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_KEYDOWN, CTPoint<int>( 0, 0 ), frame ) )
	{
		return;
	}
	if ( ( nChar == VK_RETURN ) || ( nChar == VK_SPACE ) )
	{
		if ( frame->isReservePositionActive )
		{
			frame->SaveReservePosition();
			return;
		}
		else
		{
			ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
			if ( !frame->dlg && terra && !frame->isStartCommandPropertyActive ) 
			{
				if ( frame->m_currentMovingObjectPtrAI || !frame->m_currentMovingObjectsAI.empty() )
				{
					frame->dlg = new CPropertieDialog;
					frame->dlg->Create( CPropertieDialog::IDD, frame );
					// fill out the window with properties for this unit
					UpdatePropertie( 0, frame  );
					// frame->DrawUnitsSelection();
					frame->RedrawWindow();
					frame->SetMapModified();
					return;
				}
			}
		}
	}

	if ( nChar == VK_DELETE ) 
	{
		if ( frame->isReservePositionActive )
		{
			frame->m_CurentReservePosition.vPos = VNULL2;
			frame->m_CurentReservePosition.pTruckObject = 0;
			frame->m_CurentReservePosition.pArtilleryObject = 0;
			frame->m_ReservePositionSequence.clear();
			// frame->DrawReservePositionRedLines();
			// frame->DrawUnitsSelection();
			frame->RedrawWindow();
			return;
		}

		// ClearAllSelection( frame );
		bool bNeedUpdateStorages = frame->NeedUpdateStorages();
		if ( frame->m_currentMovingObjectPtrAI )
		{
			if ( frame->dlg ) 
			{
				frame->dlg->DestroyWindow();
				delete frame->dlg;
				frame->dlg = 0;
				frame->isStartCommandPropertyActive = false;
			}

			/* *
			 */
			
			// static_cast<IScene*>( GetSingleton()->Get( SCNGRPH_SCENE_GRAPH ) )->RemoveObject( m_currentMovingObjectPtr );
			frame->RemoveObject( frame->m_currentMovingObjectPtrAI );
			frame->m_currentMovingObjectPtrAI = 0;
			// frame->m_currentObjectForPastePtrAI = frame->m_currentMovingObjectPtrAI;
			
			frame->SetMapModified();
			if ( bNeedUpdateStorages )
			{
				frame->ShowStorageCoverage();
			}
			else if ( g_frameManager.GetMiniMapWindow() )
			{
				g_frameManager.GetMiniMapWindow()->UpdateMinimap( true );
			}
		}
		// if multiselect
		if ( !frame->m_currentMovingObjectsAI.empty() )
		{
			if ( frame->dlg ) 
			{
				frame->dlg->DestroyWindow();
				delete frame->dlg;
				frame->dlg = 0;
				frame->isStartCommandPropertyActive = false;
			}
			
			// std::vector<SObjectDellDisciption> dellObjects;
			for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
			{
			  // CVec3 v = ( *it )->pVisObj->GetPosition();
				// dellObjects.push_back( SObjectDellDisciption( frame->GetEditorObjectItem(*it)->sDesc,frame->GetEditorObjectItem(*it)->nPlayer, v, (*it)->pVisObj->GetDirection(), frame->GetEditorObjectItem(*it)->nScriptID, 
				if ( frame->m_objectsAI.find( *it ) != frame->m_objectsAI.end() )
				{
					frame->RemoveObject( *it );
				}
			}
			// for Undo
			/* *
			 */
			frame->m_currentMovingObjectsAI.clear();
			frame->m_shiftsForMovingObjectsAI.clear();
			frame->m_squadsShiftsForMovingObjectsAI.clear();
			
			frame->SetMapModified();
			if ( bNeedUpdateStorages )
			{
				frame->ShowStorageCoverage();
			}
			else if ( g_frameManager.GetMiniMapWindow() )
			{
				g_frameManager.GetMiniMapWindow()->UpdateMinimap( true );
			}
		}
		
		ClearAllSelection( frame );
		frame->RedrawWindow();
	}
}


void CObjectPlacerState::OnLButtonUp(UINT nFlags, const CTPoint<int> &rMousePoint, CTemplateEditorFrame *frame )
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_LBUTTONUP, rMousePoint, frame ) )
	{
		return;
	}
	CPoint point( rMousePoint.x, rMousePoint.y );
	frame->m_cursorName = MAKEINTRESOURCE( IDC_ARROW );

	if ( frame->dlg && frame->isStartCommandPropertyActive )
	{
		CVec3 v;
		CVec2 p ( point.x , point.y );
		GetSingleton<IScene>()->GetPos3( &v, p );
		Vis2AI( &v );
		
		IManipulator* pMan = frame->dlg->GetCurrentManipulator();
		if ( pMan )
		{
			pMan->SetValue( "Position:x", v.x / ( 2 * SAIConsts::TILE_SIZE ) );
			pMan->SetValue( "Position:y", v.y / ( 2 * SAIConsts::TILE_SIZE ) );
			
			frame->dlg->UpdateObjectProp();
			
			frame->RecalculateStartCommandRedLines( v );
			// frame->DrawAIStartCommandRedLines();
			// frame->DrawUnitsSelection();
		}
		frame->RedrawWindow();
		return;
	}

	bool bNeedUpdateStorages = frame->NeedUpdateStorages();

	if ( frame->m_currentMovingObjectPtrAI ) 
	{
		/* *
		 */

		if ( bNeedUpdateStorages )
		{
			frame->ShowStorageCoverage();
		}
		else if ( g_frameManager.GetMiniMapWindow() )
		{
			g_frameManager.GetMiniMapWindow()->UpdateMinimap( true );
		}
	} 
	
	// =========================
	// if they took a group and let them go over the house -> we need to put soldiers there
	// =========================
	// bool bClearSelection = false;
	if ( frame->m_ifCanMovingMultiGroup )
	{
		std::pair<IVisObj*, CVec2> *pObjects;
		int num;
		CVec3 v;
		CVec2 p ( point.x , point.y );
		GetSingleton<IScene>()->GetPos3( &v, p );
		frame->Pick( p, &pObjects, &num, SGVOGT_UNKNOWN, true, true );
		// in a picked group, you need to look to see if they clicked on an object that can put itself in this group
		
		for ( int i = 0; i != num; ++i )
		{
			bool ifInGroup = false;
			for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
			{
				if( (*it)->pVisObj == pObjects[i].first )
					ifInGroup = true;
			}
			if( !ifInGroup )
			{
				// pObjects[i].first - it does not belong to the selected group
				int nType = 0;
				if(	CheckForInserting( frame->m_currentMovingObjectsAI, frame->FindByVis(pObjects[i].first ), &nType ) )
				{
					// that is, you can put m_current MovingObjecsAi in pObjects[i].first
					for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
					{
						if ( nType == 0 )
						{
							// when we put it in, we need to place it in the center of the building
							CVec2 vPos = GetSingleton<IAIEditor>()->GetCenter(  frame->FindByVis(pObjects[i].first )->pAIObj );
							// GetSingleton<IAIEditor>()->MoveObject( (*it)->pAIObj, vPos.x - 30, vPos.y + 30);
							frame->MoveObject( (*it)->pAIObj, vPos.x - 30, vPos.y + 30);
						}

						frame->GetEditorObjectItem( *it )->pLink = frame->FindByVis( pObjects[i].first );
						// frame->m_currentMovingObjectPtrAI = frame->GetEditorObjectItem( *it )->pLink;
						// bClearSelection = true;
					}
					frame->SetMapModified();
					break;
				}
			}
		}
		/* *
		 */
		if ( bNeedUpdateStorages )
		{
			frame->ShowStorageCoverage();
		}
		else if ( g_frameManager.GetMiniMapWindow() )
		{
			g_frameManager.GetMiniMapWindow()->UpdateMinimap( true );
		}
	}

	// ================================================================
	// you also need to be able to put one soldier in a house
	// ================================================================
	if ( frame->m_currentMovingObjectPtrAI ) // && !bClearSelection )
	{
		std::pair<IVisObj*, CVec2> *pObjects;
		int num;
		CVec3 v;
		CVec2 p ( point.x , point.y );
		GetSingleton<IScene>()->GetPos3( &v, p );
		frame->Pick( p, &pObjects, &num, SGVOGT_UNKNOWN, true, true );
		// in the pick group you need to see if you clicked on an object that can plant this soldier
		for ( int i = 0; i != num; ++i )
		{
			if( pObjects[i].first != frame->m_currentMovingObjectPtrAI->pVisObj )
			{
				std::vector< SMapObject* > v;v.push_back( frame->m_currentMovingObjectPtrAI );
				int nType = 0;
				if(	CheckForInserting( v, frame->FindByVis(pObjects[i].first ), &nType ) )
				{
					// GetSingleton<IAIEditor>()->MoveObject( (*it)->pAIObj, vPos.x - 30, vPos.y + 30);
					if ( nType == 0 )
					{
						CVec2 vPos = GetSingleton<IAIEditor>()->GetCenter(  frame->FindByVis(pObjects[i].first )->pAIObj );
						frame->MoveObject( frame->m_currentMovingObjectPtrAI->pAIObj, vPos.x - 30, vPos.y + 30);
					}
					frame->GetEditorObjectItem( frame->m_currentMovingObjectPtrAI )->pLink = frame->FindByVis( pObjects[i].first );
					// frame->m_currentMovingObjectPtrAI = frame->GetEditorObjectItem( frame->m_currentMovingObjectPtrAI )->pLink;
					frame->SetMapModified();
					break;
				}
			}
		}
	}
	// =========================
	// if we selected an area
	// =========================

	// deselect
	ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
	int worldHeight = 0;
	if ( terra )
	{		
		dynamic_cast<ITerrainEditor*>(terra)->SetMarker( NULL, 0 );
		worldHeight = terra->GetSizeY();
	}
	// if possible, we will highlight
	if ( ( frame->m_currentMovingObjectForPlacementPtr == 0 ) && frame->m_ifCanMultiSelect ) 
	{

		// Let's take the last point
		frame->GetTileIndexBy2DPoint( point.x, point.y, frame->m_lastSelectPoint.x, frame->m_lastSelectPoint.y );
		const STerrainInfo	&info = dynamic_cast<ITerrainEditor *>(terra)->GetTerrainInfo();
		ValidatePoint( CTRect<int>( 0, 0, info.tiles.GetSizeX(), info.tiles.GetSizeY() ), &( frame->m_lastSelectPoint ) ); 		
		
		// deselect the old selection
		ClearAllSelection( frame );
		GRect rect( 
			Min(frame-> m_lastSelectPoint.x, frame->m_firstSelectPoint.x ) * fWorldCellSize,
			Min( worldHeight - frame->m_lastSelectPoint.y - 1, worldHeight - frame->m_firstSelectPoint.y - 1) * fWorldCellSize,
			( Max( frame->m_lastSelectPoint.x, frame->m_firstSelectPoint.x ) + 1 ) * fWorldCellSize,
			( Max( worldHeight - frame->m_lastSelectPoint.y - 1, worldHeight - frame->m_firstSelectPoint.y - 1 ) + 1 )  * fWorldCellSize
			);

		// we got the frame, now let’s convert it with asking the selected units
		CVec3 vEnd = GetSingleton<IScene>()->GetFrameSelection()->GetEndPoint();
		CVec3 vBegin = GetSingleton<IScene>()->GetFrameSelection()->GetBeginPoint();
		CVec2 pEnd,pBegin;
		GetSingleton<IScene>()->GetPos2( &pEnd, vEnd );
		GetSingleton<IScene>()->GetPos2( &pBegin, vBegin );

		std::pair<IVisObj*, CVec2> *pObjects;
		int num;
		CTRect<float> rectForSelect( pBegin, pEnd);
		rectForSelect.Normalize();
		frame->Pick( rectForSelect, &pObjects, &num, SGVOGT_UNKNOWN);
		if(  !( GetAsyncKeyState( VK_CONTROL  ) & 32768 ) )
		{
			std::vector< std::pair<IVisObj*, CVec2> >  pObjectsReal( pObjects, pObjects + num );
			for ( int i = 0; i != num; ++i )
			{
				pObjectsReal[ i ].first->Select( SGVOSS_SELECTED );
				
				bool bSquad = false;
				std::vector<SMapObject*>	vTmp = GetObjectsSelection(  frame->FindByVis( pObjectsReal[ i ].first ),  frame, &bSquad );
				FilterObjects( vTmp, frame );
				CalculateObjectSelection( vTmp, frame, VNULL3, bSquad );
			}
		}
		else
		{
			for ( std::hash_map< SMapObject*, SEditorObjectItem*, SDefaultPtrHash >::iterator it = frame->m_objectsAI.begin(); it != frame->m_objectsAI.end(); ++it )
			{
				if ( ( it->first->pDesc->eGameType != SGVOGT_ENTRENCHMENT ) && ( it->first->pDesc->eGameType != SGVOGT_BRIDGE ) )
				{
					CVec3 v  = it->first->pVisObj->GetPosition();
					if ( rect.contains( v.x, v.y ) )
					{
						it->first->pVisObj->Select( SGVOSS_SELECTED );
						bool bSquad = false;
						std::vector<SMapObject*>	vTmp = GetObjectsSelection(  it->first,  frame, &bSquad );
						FilterObjects( vTmp, frame );
						CalculateObjectSelection( vTmp, frame, VNULL3, bSquad );
					}
				}
			}
		}
		if ( num && !frame->isStartCommandPropertyActive )
		{
			UpdatePropertie( NULL, frame  );
		}
		GetSingleton<IScene>()->GetFrameSelection()->End();

		if ( CMainFrame* pMainFrame = theApp.GetMainFrame() )
		{
			int currentSel = pMainFrame->pwndFireRangeFilterComboBox->GetCurSel();
			if ( currentSel != CB_ERR  )
			{
				CString currentFilter;
				pMainFrame->pwndFireRangeFilterComboBox->GetLBText( currentSel, currentFilter );	
				if ( currentFilter.Compare( CSetupFilterDialog::SELECTED_UNITS ) == 0 )
				{
					frame->ShowFireRange( frame->bFireRangePressed );		
				}
			}
		}
	}
	frame->m_ifCanMultiSelect = false;
	frame->m_ifCanMovingMultiGroup = false; 
	frame->RedrawWindow();
}


void CObjectPlacerState::OnRButtonUp(UINT nFlags, const CTPoint<int> &rMousePoint, CTemplateEditorFrame *frame)
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_RBUTTONUP, rMousePoint, frame ) )
	{
		return;
	}
	CPoint point( rMousePoint.x, rMousePoint.y );
	if ( frame->dlg && frame->isStartCommandPropertyActive )
	{
		frame->RedrawWindow();
		return;
	}

	if ( frame->isReservePositionActive )
	{
		if ( !frame->m_ReservePositionSequence.empty() )
		{
			// 0 - pos
			// 1 - artillery
			// 2 - truck
			switch( ( *( frame->m_ReservePositionSequence.begin() ) ) )
			{
				case 0:
				{
					frame->m_CurentReservePosition.vPos = VNULL2;
					break;
				}
				case 1:
				{
					frame->m_CurentReservePosition.pArtilleryObject = 0;
					break;
				}
				default:
				{
					frame->m_CurentReservePosition.pTruckObject = 0;
					break;
				}
			}
			frame->m_ReservePositionSequence.pop_front();
		}
		// frame->DrawReservePositionRedLines();
	}

	int iItem = -1;
  while ( (iItem = frame->m_mapEditorBarPtr->GetObjectWnd()->m_imageList.GetNextItem(iItem, LVNI_ALL)) > -1 )
    frame->m_mapEditorBarPtr->GetObjectWnd()->m_imageList.SetItemState( iItem, 0, LVIS_SELECTED );          

	// Resetting objects with advanced clipboard
	if ( frame->m_currentMovingPasteGroupName != "" )
	{
		// First, let's soak the previous pictures
		for( std:: vector<CPtr<IVisObj> >::iterator it = frame->m_currentMovingObjectsForPlacementPtr.begin();
		it != frame->m_currentMovingObjectsForPlacementPtr.end(); ++it )
		{
			frame->RemoveObject( *it );
		}
		frame->m_currentMovingPasteGroupName = "";
		frame->m_currentMovingObjectsForPlacementPtr.clear();
	}

	if ( !( nFlags & MK_LBUTTON ) ) // if the button is not pressed, then simply reset the selection
	{
		/* for delete 
		 */
		if ( frame-> m_currentMovingObjectForPlacementPtr ) // cut out the translucent picture
		{
			// static_cast<IScene*>( GetSingleton()->Get( SCNGRPH_SCENE_GRAPH ) )->RemoveObject( m_currentMovingObjectForPlacementPtr );
			frame->RemoveObject( frame->m_currentMovingObjectForPlacementPtr );
			frame->m_currentMovingObjectForPlacementPtr = 0;
		}
		// let's kill the select
		ClearAllSelection( frame );
		/* for delete 
		 */
		if ( frame->dlg != 0 )
			frame->dlg->DestroyWindow();
			frame->dlg = 0;
			frame->isStartCommandPropertyActive = false;
	}
	else  // and if pressed, we will iterate through the selected objects
	{
		if ( !frame->m_ifCanMultiSelect )
		{
			ClearAllSelection( frame );
			/* for delete 
			 */
			if ( frame->m_pickedObjects.size() )
			{
				++(frame->m_curPickNum);
				if ( frame->m_curPickNum == frame->m_pickedObjects.size() ) 
					frame->m_curPickNum = 0;
				if( SMapObject *tmpPtr =  frame->FindByVis( frame->m_pickedObjects[frame->m_curPickNum].first) )
				{
					CalculateObjectSelection( GetObjectsSelection( tmpPtr, frame ), frame );
					frame->m_currentObjectShift = frame->m_pickedObjects[frame->m_curPickNum].second;
				}
				// frame->m_currentMovingObjectPtrAI->pVisObj->Select( SGVOSS_SELECTED );
				// !!!ADD
				// UpdatePropertie( frame->GetEditorObjectItem(frame->m_currentMovingObjectPtrAI), frame );
			}
			if ( CMainFrame* pMainFrame = theApp.GetMainFrame() )
			{
				int currentSel = pMainFrame->pwndFireRangeFilterComboBox->GetCurSel();
				if ( currentSel != CB_ERR  )
				{
					CString currentFilter;
					pMainFrame->pwndFireRangeFilterComboBox->GetLBText( currentSel, currentFilter );	
					if ( currentFilter.Compare( CSetupFilterDialog::SELECTED_UNITS ) == 0 )
					{
						frame->ShowFireRange( frame->bFireRangePressed );		
					}
				}
			}
		}
	}
	frame->RedrawWindow();
}


void CObjectPlacerState::OnObjectPaste(CTemplateEditorFrame *frame	)
{
	/* *
	 */
	// if ( frame->m_currentForPasteObjectsAI.size() )
	// {
		// Let's soak the selection first
		// ClearAllSelection( frame );
		/* for delete
		 */
		// this means we can supply a lot of units
		/* *
		 */
	// }
}

void CObjectPlacerState::OnLButtonDblClk(UINT nFlags, const CTPoint<int> &rMousePoint, CTemplateEditorFrame *frame )
{
	if ( !stateParameter.Update( CInputStateParameter::ISE_LBUTTONDBLCLK, rMousePoint, frame ) )
	{
		return;
	}
	CPoint point( rMousePoint.x, rMousePoint.y );
	if ( frame->dlg && frame->isStartCommandPropertyActive )
	{
		frame->RedrawWindow();
		return;
	}

	if ( frame->isReservePositionActive )
	{
		if ( frame->m_CurentReservePosition.vPos != VNULL2 && 
				 frame->m_CurentReservePosition.pArtilleryObject )
		{
			bool bAdd = true;
			if ( frame->m_CurentReservePosition.pTruckObject == 0 )
			{
				CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();
				CGDBPtr<SMechUnitRPGStats> pArtilleryStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( frame->m_CurentReservePosition.pArtilleryObject->pDesc ) );
				if ( IsArtillery( pArtilleryStats->type ) && 
						 ( !pArtilleryStats->vPeoplePoints.empty() ) ) 
				{
					bAdd = false;
				}
			}
			if ( bAdd )
			{
				frame->m_reservePositions.push_front( frame->m_CurentReservePosition );
				frame->SetMapModified();
			}
		}
		
		frame->m_CurentReservePosition.vPos = VNULL2;
		frame->m_CurentReservePosition.pTruckObject = 0;
		frame->m_CurentReservePosition.pArtilleryObject = 0;
		frame->m_ReservePositionSequence.clear();

		// frame->DrawReservePositionRedLines();
		// frame->DrawUnitsSelection();
		frame->RedrawWindow();		
		return;
	}
	
	ITerrain *terra = GetSingleton<IScene>()->GetTerrain();
	if ( !frame->dlg && terra && !frame->isStartCommandPropertyActive ) 
	{
		CVec2 p ( point.x , point.y );
		std::pair<IVisObj*, CVec2> *pObjectsTmp;
		int num;
		frame->Pick( p, &pObjectsTmp, &num, SGVOGT_UNKNOWN );
		std::vector< std::pair<IVisObj*, CVec2> > pObjects( pObjectsTmp, pObjectsTmp + num );
		for ( std::vector< std::pair<IVisObj*, CVec2> >::iterator it = pObjects.begin(); it != pObjects.end(); )
		{
			SMapObject *pMapObject = frame->FindByVis( it->first );
			if ( pMapObject )
			{
				IRefCount *pSquad = GetSingleton<IAIEditor>()->GetFormationOfUnit( pMapObject->pAIObj );
				if ( pSquad )
				{
					SEditorObjectItem *tmpEditiorObj =	frame->m_objectsAI[ pMapObject ];
					if ( tmpEditiorObj )
					{
						if( tmpEditiorObj->pLink )
						{
							if( tmpEditiorObj->pLink->IsValid() )
							{
								it = pObjects.erase( it );
								continue;
							}
						}
					}
				}
			}
			++it;
		}
		num = pObjects.size();
		if ( ( num > 0 ) || frame->m_currentMovingObjectPtrAI || !frame->m_currentMovingObjectsAI.empty() )
		{	
			frame->dlg = new CPropertieDialog;
			frame->dlg->Create( CPropertieDialog::IDD, frame );
			// fill out the window with properties for this unit
			if ( num )
			{
				UpdatePropertie( frame->GetEditorObjectItem(frame->FindByVis( pObjects[ 0 ].first )), frame  );
			}
			else
			{
				UpdatePropertie( 0, frame  );
			}
			frame->SetMapModified();
		}
		else
		{
			// and now let's try to do this for the trenches
			GetSingleton<IScene>()->Pick( p, &pObjectsTmp, &num, SGVOGT_UNKNOWN );
			std::vector< std::pair<IVisObj*, CVec2> > pObjects2( pObjectsTmp, pObjectsTmp + num );
			for ( std::vector< std::pair<IVisObj*, CVec2> >::iterator it = pObjects2.begin(); it != pObjects2.end(); )
			{
				SMapObject *pMapObject = frame->FindByVis( it->first );
				if ( pMapObject )
				{
					IRefCount *pSquad = GetSingleton<IAIEditor>()->GetFormationOfUnit( pMapObject->pAIObj );
					if ( pSquad )
					{
						SEditorObjectItem *tmpEditiorObj =	frame->m_objectsAI[ pMapObject ];
						if ( tmpEditiorObj )
						{
							if( tmpEditiorObj->pLink )
							{
								if( tmpEditiorObj->pLink->IsValid() )
								{
									it = pObjects2.erase( it );
									continue;
								}
							}
						}
					}
				}
				++it;
			}
			num = pObjects2.size();
			if ( num > 0 )
			{
				bool bExists = false;
				for ( int i = 0; i != num; ++i )
				{
					if ( frame->IsExistByVis( pObjects2[i].first ) && frame->FindByVis( pObjects2[i].first )->pDesc->eGameType == SGVOGT_ENTRENCHMENT )
					{
						if ( frame->dlg == 0 )
						{
							frame->dlg = new CPropertieDialog;
							frame->dlg->Create( CPropertieDialog::IDD, frame );
						}
						UpdatePropertie( frame->GetEditorObjectItem(frame->FindByVis( pObjects2[i].first ) ), frame  );
						frame->SetMapModified();
					}
				}
			}
		}
	}
	frame->RedrawWindow();		
}

// Let's see if group (v) can be placed in object p
bool CObjectPlacerState::CheckForInserting( std::vector< SMapObject* > &v, SMapObject *p, int *pType )
{
	// 1 - train
	// 2 - artillery
	if ( pType )
	{
		( *pType ) = 0;
	}
	// First, let’s check if everyone in this group can log into the SUnitBaseRPGStats machine
	bool bCan = true; 
	CPtr<IObjectsDB> pODB = GetSingleton<IObjectsDB>();

	for ( std::vector<SMapObject*>::iterator it = v.begin(); it != v.end(); ++it )
	{
		CGDBPtr<SUnitBaseRPGStats> pStats = dynamic_cast<const SUnitBaseRPGStats*>( pODB->GetRPGStats( (*it)->pDesc ) );
		if ( pStats &&  ! pStats ->IsInfantry() )
			bCan = false;
		if( !pStats )
			bCan = false;
	}	
	if ( p->pDesc->eGameType == SGVOGT_BUILDING )
	{
		CGDBPtr<SBuildingRPGStats> pStats = dynamic_cast<const SBuildingRPGStats*>( pODB->GetRPGStats( p->pDesc ) );
		if ( !pStats ) 
		{
			bCan = false;
		}
		else if ( ( pStats->slots.size() + pStats->nRestSlots + pStats->nMedicalSlots ) < v.size() )
		{	
			bCan = false;
		}
	}
	else if ( p->pDesc->eGameType == SGVOGT_ENTRENCHMENT )
	{
		/* *
		 */
	}
	else if ( p->pDesc->eGameType == SGVOGT_UNIT )
	{
		CGDBPtr<SMechUnitRPGStats> pStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( p->pDesc ) );
		if ( pStats )
		{
			if ( ( pStats->vEntrancePoint == VNULL2 ) || ( pStats->nPassangers < v.size() ) )
			{
				bCan = false;
			}
		}
		else
		{
			bCan = false;
		}
	}
	else
	{
		bCan = false;
	}
	if ( !bCan && !v.empty() )
	{
		CGDBPtr<SMechUnitRPGStats> pStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( v[0]->pDesc ) );
		if ( pStats &&
				 ( ( pStats->type == RPG_TYPE_TRN_CARRIER ) || ( pStats->type == RPG_TYPE_TRN_TRACTOR ) ) &&
				 ( pStats->vTowPoint != VNULL2 ) && 
				 ( pStats->fTowingForce > 0 ) )
		{
			if ( p->pDesc->eGameType == SGVOGT_UNIT )
			{
				CGDBPtr<SMechUnitRPGStats> pStats2 = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( p->pDesc ) );
				if ( pStats2 &&
					   IsArtillery( pStats2->type ) &&
						 ( !pStats2->vPeoplePoints.empty() ) && 
						 ( pStats->fTowingForce > pStats2->fWeight ) )
				{
					if ( pType != 0 )
					{
						( *pType ) = 2;
					}
					bCan = true;
				}
			}
		}
	}
	if ( !bCan && !v.empty() )
	{
		CGDBPtr<SMechUnitRPGStats> pStats = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( v[0]->pDesc ) );
		if ( pStats && IsTrain( pStats->type ) )
		{
			if ( p->pDesc->eGameType == SGVOGT_UNIT )
			{
				CGDBPtr<SMechUnitRPGStats> pStats2 = dynamic_cast<const SMechUnitRPGStats*>( pODB->GetRPGStats( p->pDesc ) );
				if ( pStats2 &&
					   IsTrain( pStats2->type ) )
				{
					if ( pType != 0 )
					{
						( *pType ) = 1;
					}
					bCan = true;
				}
			}
		}
	}
	return bCan;


	/* *
	 */
}


void CObjectPlacerState::UpdatePropertie( SEditorObjectItem *ptr, CTemplateEditorFrame *frame )
{
	if ( frame->dlg )
	{
		frame->dlg->ClearVariables();
		if ( frame->m_currentMovingObjectsAI.empty() )		
		{
			if( ptr )
			{
				ptr->notActivatedLogicGroupId = timeGetTime(); // for group logic
				IManipulator *pTmp = ptr->GetManipulator();
				frame->dlg->AddObjectWithProp( pTmp );
			}
			else if ( frame->m_currentMovingObjectPtrAI )
			{
				ptr = frame->GetEditorObjectItem( frame->m_currentMovingObjectPtrAI );
				if ( ptr )
				{
					ptr->notActivatedLogicGroupId = timeGetTime(); // for group logic
					IManipulator *pTmp = ptr->GetManipulator();
					frame->dlg->AddObjectWithProp( pTmp );
				}
			}
		}
		else
		{	// group is currently selected
			int nID= timeGetTime(); // for group logic

			CMultiManipulator *pTmp = new CMultiManipulator ;
			for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
			{
				frame->GetEditorObjectItem( (*it) )->notActivatedLogicGroupId = nID;
				pTmp->AddManipulator( frame->GetEditorObjectItem( (*it) )->GetManipulator() );
			}
		/* MultiUnitManipulator *pTmp = new CMultiUnitManipulator;
			 */
			frame->dlg->AddObjectWithProp( pTmp );
		}
	}
}

 
void CObjectPlacerState::ClearAllSelection( CTemplateEditorFrame *frame  )
{
	// First, let's reset the selection (it doesn't matter multi or single) and then erase it from frame
	for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
	{
		if ( frame->m_objectsAI.find( *it ) != frame->m_objectsAI.end() )
		{
			( *it )->pVisObj->Select( SGVOSS_UNSELECTED );;
		}
	}
	if( frame->m_currentMovingObjectPtrAI ) 
		frame->m_currentMovingObjectPtrAI->pVisObj->Select( SGVOSS_UNSELECTED );


		for( std:: vector<CPtr<IVisObj> >::iterator it = frame->m_currentMovingObjectsForPlacementPtr.begin();
		it != frame->m_currentMovingObjectsForPlacementPtr.end(); ++it )
		{
			frame->RemoveObject( *it );
		}
		frame->m_currentMovingObjectsForPlacementPtr.clear();

	// wet
	frame->m_currentMovingObjectPtrAI = 0;
	frame->m_currentMovingObjectsAI.clear();
	frame->m_shiftsForMovingObjectsAI.clear();
	frame->m_squadsShiftsForMovingObjectsAI.clear();
}



std::vector<SMapObject*> CObjectPlacerState::GetObjectsSelection(  std::vector<SMapObject*> &objects, CTemplateEditorFrame *frame, bool *pbSquad )
{
	std::vector<SMapObject*> retObjects;
	for(  std::vector<SMapObject*>::iterator it = objects.begin(); it != objects.end(); ++it )
	{
		IRefCount* ptr = GetSingleton<IAIEditor>()->GetFormationOfUnit( (*it)->pAIObj );
		if( ptr )
		{
			IRefCount **pUnits;
			int nLength;
			GetSingleton<IAIEditor>()->GetUnitsInFormation( (*it)->pAIObj, &pUnits, &nLength);	
			for( int i = 0 ; i != nLength; ++i )
			{
				retObjects.push_back( frame->FindByAI( pUnits[i] ) );
			}
		}
		else
		{ // this is definitely not a platoon
			retObjects.push_back( (*it) );
		}
		
	}
	return retObjects;
} 


std::vector<SMapObject*> CObjectPlacerState::GetObjectsSelection(  SMapObject* object, CTemplateEditorFrame *frame, bool *pbSquad )
{
	if ( pbSquad )
	{
		( *pbSquad ) = false;
	}
	std::vector<SMapObject*> retObjects;
	IRefCount* ptr = GetSingleton<IAIEditor>()->GetFormationOfUnit( object->pAIObj );
	if( ptr )
	{
		IRefCount **pUnits;
		int nLength;
		GetSingleton<IAIEditor>()->GetUnitsInFormation( ptr, &pUnits, &nLength);	
		for( int i = 0 ; i != nLength; ++i )
		{
			retObjects.push_back( frame->FindByAI( pUnits[i] ) );
		}
		if ( pbSquad )
		{
			( *pbSquad ) = true;
		}
	}
	else
	{ // this is definitely not a platoon
		retObjects.push_back( object );
	}
	return retObjects;
}



void fSetObjectSelect( SMapObject* obj )
{
	obj->pVisObj->Select( SGVOSS_SELECTED );
}

void fSetObjectUnSelect( SMapObject* obj )
{
	obj->pVisObj->Select( SGVOSS_UNSELECTED );
}


void CObjectPlacerState::CalculateObjectSelection(  std::vector<SMapObject*> &objects, CTemplateEditorFrame *frame, const CVec3 &center, bool bSquad )
{
	// one object has been selected, we want to add more -> multi select
	if( frame->m_currentMovingObjectPtrAI &&  !frame->m_currentMovingObjectsAI.size() )
	{
		frame->m_currentMovingObjectsAI.push_back( frame->m_currentMovingObjectPtrAI );
		frame->m_currentMovingObjectPtrAI = 0;
		frame->m_currentMovingObjectsAI.insert( frame->m_currentMovingObjectsAI.begin(), objects.begin(), objects.end() );
		std::for_each( frame->m_currentMovingObjectsAI.begin(), frame->m_currentMovingObjectsAI.end(), fSetObjectSelect );
		return;
	}

	// just one object (i.e. there was nothing and it came that you need to add one object)
	if( !frame->m_currentMovingObjectPtrAI &&  !frame->m_currentMovingObjectsAI.size() && objects.size() == 1 && !bSquad )
	{
		frame->m_currentMovingObjectPtrAI = objects[0];
		fSetObjectSelect( frame->m_currentMovingObjectPtrAI );
		return;
	}

	if( ( !frame->m_currentMovingObjectPtrAI &&  !frame->m_currentMovingObjectsAI.size() && objects.size() > 1 ) || bSquad )
	{
		frame->m_currentMovingObjectsAI.insert( frame->m_currentMovingObjectsAI.begin(), objects.begin(), objects.end() );
		std::for_each( frame->m_currentMovingObjectsAI.begin(), frame->m_currentMovingObjectsAI.end(), fSetObjectSelect );
		return;
	}

	
	if( !frame->m_currentMovingObjectPtrAI &&  frame->m_currentMovingObjectsAI.size()  )
	{
		frame->m_currentMovingObjectsAI.insert( frame->m_currentMovingObjectsAI.begin(), objects.begin(), objects.end() );
		std::for_each( frame->m_currentMovingObjectsAI.begin(), frame->m_currentMovingObjectsAI.end(), fSetObjectSelect );
		return;
	}

	// multi select (i.e. it appears that you need to add several objects -> in any case it will be a multi select)
	if( !frame->m_currentMovingObjectPtrAI &&  !objects.empty() && !frame->m_currentMovingObjectsAI.empty() )
	{
		frame->m_currentMovingObjectsAI.insert( frame->m_currentMovingObjectsAI.begin(), objects.begin(), objects.end() );
		std::for_each( frame->m_currentMovingObjectsAI.begin(), frame->m_currentMovingObjectsAI.end(), fSetObjectSelect );
		return;
	}
}

// count the offset
void CObjectPlacerState::CalculateShiftsForMultiSelect( CTemplateEditorFrame *frame, const CVec3 &center, const CVec3 &rShift )
{
	// for each selected unit we need to store the offset from the point where we just clicked
	frame->m_shiftsForMovingObjectsAI.clear();							
	frame->m_squadsShiftsForMovingObjectsAI.clear();
	for ( std::vector<SMapObject*>::iterator it = frame->m_currentMovingObjectsAI.begin(); it != frame->m_currentMovingObjectsAI.end(); ++it )
	{
		if ( IRefCount* obj = GetSingleton<IAIEditor>()->GetFormationOfUnit( (*it)->pAIObj ) )
		{
			if ( frame->m_squadsShiftsForMovingObjectsAI.find( obj ) == frame->m_squadsShiftsForMovingObjectsAI.end() )
			{
				CVec3 v = CVec3( GetSingleton<IAIEditor>()->GetCenter( obj ), 0.0f );
				AI2Vis( &v );
				frame->m_squadsShiftsForMovingObjectsAI.insert( std::make_pair( obj, CVec2( v.x - center.x, v.y - center.y ) ) );	
			}
		}
		else
		{
			frame->m_shiftsForMovingObjectsAI.insert( std::make_pair( *it, CVec2( (*it)->pVisObj->GetPosition().x - center.x, (*it)->pVisObj->GetPosition().y - center.y ) ) );	
		}
	}
}

bool fIsObjectInSelection( CTemplateEditorFrame *frame,  SMapObject* object )
{
	if( frame->m_currentMovingObjectPtrAI == object )
		return true;
	if( std::find( frame->m_currentMovingObjectsAI.begin(),frame->m_currentMovingObjectsAI.end(), object ) !=  frame->m_currentMovingObjectsAI.end() )
		return true;

	return false;
}

void	CObjectPlacerState::FilterObjects( std::vector< struct SMapObject*> &objects, CTemplateEditorFrame *frame )
{
	objects.erase( std::remove_if( objects.begin(), objects.end(), std::bind1st( std::ptr_fun( fIsObjectInSelection ), frame ) ),
		objects.end() );
}

void	CObjectPlacerState::PlacePasteGroup( const std::string &name, CTemplateEditorFrame *frame )
{
	if ( name != "")
	{	
		{
			// Let's soak the selection first
			ClearAllSelection( frame );
			frame->m_currentMovingPasteGroupName = name;
		}
	}
}


void CObjectPlacerState::Enter()
{
	if ( CTemplateEditorFrame *pFrame = g_frameManager.GetTemplateEditorFrame() )
	{
		pFrame->m_mapEditorBarPtr->GetObjectWnd()->FillPlayers();
		pFrame->m_mapEditorBarPtr->GetObjectWnd()->UpdateControls();

		// Draw( pFrame );
		pFrame->RedrawWindow();
	}
}


void CObjectPlacerState::Leave()
{
	if ( CTemplateEditorFrame *pFrame = g_frameManager.GetTemplateEditorFrame() )
	{
		// pFrame->m_mapEditorBarPtr->GetObjectWnd()->UpdateControls();
		// Draw( pFrame );
		GetSingleton<IScene>()->GetFrameSelection()->End();
		pFrame->RedrawWindow();
	}
}


void CObjectPlacerState::Update()
{
	if ( CTemplateEditorFrame *pFrame = g_frameManager.GetTemplateEditorFrame() )
	{
		// pFrame->m_mapEditorBarPtr->GetObjectWnd()->UpdateControls();
		// Draw( pFrame );
		pFrame->RedrawWindow();
	}
}

