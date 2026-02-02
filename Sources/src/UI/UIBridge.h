#ifndef __USER_INTERFACE_BRIDGE_H__
#define __USER_INTERFACE_BRIDGE_H__

#define DEFINE_UIELEMENT_BRIDGE																																								\
	virtual int operator&( IDataTree &ss ) { return CSuper::operator&( ss ); }													\
	virtual int operator&( IStructureSaver &ss ) { return CSuper::operator&( ss ); }										\
	virtual bool Update( const NTimer::STime &currTime ) { return CSuper::Update( currTime ); }					\
	virtual void Reposition( const CTRect<float> &rcParent ) { CSuper::Reposition( rcParent ); }				\
	virtual void Draw( IGFX *pGFX ) { CSuper::Draw( pGFX ); }																						\
	virtual void Visit( interface ISceneVisitor *pVisitor ) { CSuper::Visit( pVisitor ); }							\
	virtual bool OnLButtonDblClk( const CVec2 &vPos ) { return CSuper::OnLButtonDblClk( vPos ); }				\
	virtual bool OnMouseMove( const CVec2 &vPos, EMouseState mouseState ) { return CSuper::OnMouseMove( vPos, mouseState ); }			\
	virtual bool OnLButtonDown( const CVec2 &vPos, EMouseState mouseState ) { return CSuper::OnLButtonDown( vPos, mouseState ); }	\
	virtual bool OnLButtonUp( const CVec2 &vPos, EMouseState mouseState ) { return CSuper::OnLButtonUp( vPos, mouseState ); }			\
	virtual bool OnRButtonDown( const CVec2 &vPos, EMouseState mouseState ) { return CSuper::OnRButtonDown( vPos, mouseState ); }	\
	virtual bool OnRButtonUp( const CVec2 &vPos, EMouseState mouseState ) { return CSuper::OnRButtonUp( vPos, mouseState ); }			\
	virtual bool OnMouseWheel( const CVec2 &vPos, EMouseState mouseState, float fDelta ) { return CSuper::OnMouseWheel( vPos, mouseState, fDelta ); } \
	virtual bool IsInside( const CVec2 &vPos ) { return CSuper::IsInside( vPos ); }											\
	virtual void ShowWindow( int nCmdShow ) { CSuper::ShowWindow( nCmdShow ); }													\
	virtual void SetWindowTexture( IGFXTexture *pTexture ) { CSuper::SetWindowTexture( pTexture ); }		\
	virtual IGFXTexture* GetWindowTexture() { return CSuper::GetWindowTexture(); }											\
	virtual void SetWindowMap( const CTRect<float> &maps ) { CSuper::SetWindowMap( maps ); }						\
	virtual void SetWindowPlacement( const CVec2 *vPos, const CVec2 *vSize ) { CSuper::SetWindowPlacement( vPos, vSize ); }				\
	virtual void SetWindowID( int _nID ) { CSuper::SetWindowID( _nID ); }																\
	virtual bool OnChar( int nAsciiCode, int nVirtualKey, bool bPressed, DWORD keyState ) { return CSuper::OnChar( nAsciiCode, nVirtualKey, bPressed, keyState ); }	\
	virtual int  GetWindowID() { return CSuper::GetWindowID(); }																				\
	virtual void SetWindowText( int nState, const wchar_t *pszText ) { CSuper::SetWindowText( nState, pszText ); }		\
	virtual const wchar_t* GetWindowText( int nState ) { return CSuper::GetWindowText( nState ); }										\
	virtual void SetTextColor( DWORD dwColor ) { CSuper::SetTextColor( dwColor ); }											\
	virtual void SetBoundRect( const CTRect<float> &rc ) { CSuper::SetBoundRect( rc ); }								\
	virtual int GetWindowPlacement( CVec2 *pPos, CVec2 *pSize, CTRect<float> *pScreenRect ) { return CSuper::GetWindowPlacement( pPos, pSize, pScreenRect ); } \
	virtual int GetPositionFlag() { return CSuper::GetPositionFlag(); }																	\
	virtual IText* GetHelpContext( const CVec2 &vPos, CTRect<float> *pRect ) { return CSuper::GetHelpContext( vPos, pRect ); }		\
	virtual void SetHelpContext( int nState, const wchar_t *pszToolTipText ) { CSuper::SetHelpContext( nState, pszToolTipText ); }		\
	virtual void SetParent( IUIContainer *pParent ) { CSuper::SetParent( pParent ); }										\
	virtual IUIContainer* GetParent() { return CSuper::GetParent(); }																		\
	virtual IManipulator* GetManipulator() { return CSuper::GetManipulator(); }													\
	virtual IUIElement* PickElement( const CVec2 &vPos, int nRecursion ) { return CSuper::PickElement( vPos, nRecursion ); }			\
	virtual bool ProcessMessage( const SUIMessage &msg ) { return CSuper::ProcessMessage( msg ); }			\
	virtual bool IsVisible() { return CSuper::IsVisible(); }																						\
	virtual int  GetVisibleState() { return CSuper::GetVisibleState(); }																\
	virtual void EnableWindow( bool bEnable ) { CSuper::EnableWindow( bEnable ); }											\
	virtual bool IsWindowEnabled() { return CSuper::IsWindowEnabled(); }																\
	virtual void SetFocus( bool bFocus ) { CSuper::SetFocus( bFocus ); }																\
	virtual void SetState( int nState, bool bNotify ) { CSuper::SetState( nState, bNotify ); }	\
	virtual int GetState() { return CSuper::GetState(); }																				\
	virtual void GetTextSize( const int nState, int *pSizeX, int *pSizeY ) const { CSuper::GetTextSize( nState, pSizeX, pSizeY );} \
	virtual bool IsModal() const { return CSuper::IsModal(); }

#define DEFINE_UICONTAINER_BRIDGE																																							\
	DEFINE_UIELEMENT_BRIDGE																																											\
	virtual void AddChild( IUIElement *pWnd ) { CSuper::AddChild( pWnd ); }															\
	virtual void RemoveChild( IUIElement *pWnd ) { CSuper::RemoveChild( pWnd ); }												\
	virtual void RemoveAllChildren() { CSuper::RemoveAllChildren(); }																		\
	virtual IUIElement* GetChildByID( int nChildID ) { return CSuper::GetChildByID( nChildID ); }				\
	virtual void MoveWindowUp( IUIElement *pWnd ) { CSuper::MoveWindowUp( pWnd ); }											\
	virtual void MoveWindowDown( IUIElement *pWnd ) { CSuper::MoveWindowDown( pWnd ); }									\
	virtual void SetModalFlag( bool bFlag ) { CSuper::SetModalFlag( bFlag ); }													\
	virtual void SetFocusedWindow( IUIElement *pNewFocusWindow ) { CSuper::SetFocusedWindow( pNewFocusWindow ); }

#define DEFINE_UISCREEN_BRIDGE																																								\
	DEFINE_UICONTAINER_BRIDGE																																										\
	virtual int Load( const char *pszResourceName, bool bRelative ) { return CSuper::Load( pszResourceName, bRelative ); }				\
	virtual void ProcessGameMessage( const SGameMessage &msg ) { CSuper::ProcessGameMessage( msg ); }		\
	virtual bool GetMessage( SGameMessage *pMsg ) { return CSuper::GetMessage( pMsg ); }								\
	virtual void ClearStrings() { CSuper::ClearStrings(); }


#endif // __USER_INTERFACE_BRIDGE_H__