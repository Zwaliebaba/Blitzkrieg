// KeyBasedData.h: interface for the CKeyBasedData class.
//
// /////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYBASEDDATA_H__A200BF93_5018_4053_AF5A_78E9878C0636__INCLUDED_)
#define AFX_KEYBASEDDATA_H__A200BF93_5018_4053_AF5A_78E9878C0636__INCLUDED_

#pragma ONCE

#include "SParticleKey.h"
// //////////////////////////////////////////////////////////// 
struct SParticleSetup
{
	CTrack	trackSpin;												// angular velocity coefficient (1 - 0) (time 0 - 100)
	CTrack	trackGenerateSpin;								// (angular velocity at departure) (0 - 2)
	CTrack	trackLife;												// how long does a particle live after generation (10 - 1000) (time 0 - 100)
	CTrack	trackDensity;											// number of particles born per unit. 
	CVec3		vWind;														// wind ( 0,0,0 )
	CTrack	trackWeight;										  // particle mass (0 - 10) (default 1) (time 0 - 100)
	CTrack	trackSpeed;											  // speed coefficient (1 - 0) (time 0 - 100) (time 0 - 100)
	CTrack	trackGenerateArea;							  // size of the area from which particles fly out (1 - 100) (time 0 - 100)
	CVec3		vDirection;												// direction ( 0,0,1 )
	int 		nGenerateAngel;										// 0 - max, 100000 ~ min (rotation angle)
	CTrack	trackBeginSpeed;									// initial speed of the particle upon departure (0 - 2) (default 0.20)
	CVec3		vPosition;
	CTrack 	trackGenerateAngle;								// rotation angle 0 - max, 100000 ~ min - (time from 0 - 100)
	CTrack 	trackSize;												// particle size (1 - 200) (time 0 - 100)
	int			nTextureDX;												// resolution for animation texture - 1
	int			nTextureDY;												// - 1
	float		fGravity;													// g - "just g" :) - ( 0.0001)
	std::string szTextureName;								//
	CTrack 	trackGenerateOpacity;							// transparency during generation (0 - 255) (time 0 - 100)
	CTrack 	trackOpacity;											// transparency coefficient (0 - 1) (time 0 - 100)
	int			nLifeTime;												// life time
	CTrack	trackGenerateSpinRand;						// rand() size per rotation (percentage) ( 0 - 100 )
	CTrack  trackTextureFrame;								// frame in texture [0..1]
	//
	int operator&( IStructureSaver &ss );
	int operator&( IDataTree &ss );
};
// //////////////////////////////////////////////////////////// 
class CKeyBasedData  : public ISharedResource
{
	OBJECT_COMPLETE_METHODS( CKeyBasedData );
	SHARED_RESOURCE_METHODS( nRefData.a );
public:
	SParticleSetup keyData;
	//
	virtual void STDCALL SwapData( ISharedResource *pResource )
	{
		CKeyBasedData *pRes = dynamic_cast<CKeyBasedData*>( pResource );
		NI_ASSERT_TF( pRes != 0, "shared resource is not a CKeyBasedData", return );
		// slow
		std::swap( keyData, pRes->keyData );
	}
	// internal container clearing
	virtual void STDCALL ClearInternalContainer() {  }
	//
	virtual int STDCALL operator&( IDataTree &ss );
};
// //////////////////////////////////////////////////////////// 
#endif // !defined(AFX_KEYBASEDDATA_H__A200BF93_5018_4053_AF5A_78E9878C0636__INCLUDED_)
