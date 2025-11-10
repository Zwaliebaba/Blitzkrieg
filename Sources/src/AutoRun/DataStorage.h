#if !defined(__AUTO_RUN_DATA_STORAGE__)
#define __AUTO_RUN_DATA_STORAGE__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ARBitmap.h"
#include "ARSound.h"
#include "ARText.h"
#include "ARConfiguration.h"

// //////////////////////////////////////////////////////////// 
/* *
 */

// //////////////////////////////////////////////////////////// 
class CDataStorage
{
	static const std::string BITMAP_EXTENTION;
	static const std::string TEXT_EXTENTION;
	static const std::string SOUND_EXTENTION;
	static const std::string CONFIGURATION_EXTENTION;

	std::hash_map<std::string, CARBitmap> bitmaps;									// pictures
	std::hash_map<std::string, CARSound> sounds;										// sounds
	std::hash_map<std::string, CARText> texts;											// texts
	std::hash_map<std::string, CARConfiguration> configurations;		// INI file

public:
	void Clear()
	{
		bitmaps.clear();
		sounds.clear();
		texts.clear();
		configurations.clear();
	}
	bool Load( const std::string &rszFileName, CDC *pDC );
	void SetCodePage( int nCodePage );

	const CARBitmap* GetBitmap( const std::string& rszBitmapName );
	const CARSound* GetSound( const std::string& rszSoundName );
	const CARText* GetText( const std::string& rszTextName );
	const CARConfiguration* GetConfiguration( const std::string& rszConfigurationName );
};
// //////////////////////////////////////////////////////////// 
#endif // !defined(__AUTO_RUN_DATA_STORAGE__)
