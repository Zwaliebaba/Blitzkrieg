#include "StdAfx.h"

#include "GameWnd.h"
#include "TemplateEditorFrame1.h"
#include "frames.h"
#include "MiniMapBar.h"

CFrameManager g_frameManager;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif

/* *
 */
/* *
 */
/* *
 */
/* *
 */
/* *
 */
/* *
 */
/* *
 */
CFrameManager::CFrameManager()
{
	activeFrameType = E_UNKNOWN_FRAME;
	pActiveFrame = 0;
	pGameWnd = 0;

	pTemplateEditorFrame = 0;
	pwndMiniMapDialog = 0;
}

void CFrameManager::SetActiveFrame( CWnd *pNewActiveFrame )
{
	if ( pActiveFrame == pNewActiveFrame )
		return;
	
	if ( !pActiveFrame )
	{
		if ( pTemplateEditorFrame )
			pTemplateEditorFrame->ShowFrameWindows( SW_HIDE );
	}
	else
	{
		if( pActiveFrame == pTemplateEditorFrame )
			pTemplateEditorFrame->ShowFrameWindows( SW_HIDE );
	}
	pActiveFrame = pNewActiveFrame;

	if ( pActiveFrame == pTemplateEditorFrame )
	{
		pTemplateEditorFrame->ShowFrameWindows( SW_SHOW );
		activeFrameType = E_TEMPLATE_FRAME;
	}

	else
	{
		activeFrameType = E_UNKNOWN_FRAME;
		ASSERT( 0 );
	}
}

CWnd *CFrameManager::GetFrameWindow( int nID )
{
	switch ( nID )
	{
		case E_TEMPLATE_FRAME:
			return pTemplateEditorFrame;
		default:
			return pTemplateEditorFrame;
	}
}
