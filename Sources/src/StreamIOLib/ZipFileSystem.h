#ifndef __ZIPFILESYSTEM_H__
#define __ZIPFILESYSTEM_H__

#pragma ONCE

#include "ZipFile.h"

struct SZipFileDesc : public CZipFile
{
	std::string szZipFileName;
};

typedef std::list<SZipFileDesc> CZipFilesList;
class CZipFileSystemEnumerator : public IStorageEnumerator
{
	OBJECT_MINIMAL_METHODS( CZipFileSystemEnumerator );
	//
	CPtr<IDataStorage> pStorage;					// parent storage
	const CZipFilesList &zipfiles;				// list of open zip files
	CZipFilesList::const_iterator posZipFile;	// current zip file
	int nFileInZip;												// current file inside zip
	std::string szFileName;								// current enumerated file name
	SStorageElementStats stats;						// temporary data storage to fill each call
	//
	bool NextEntry();
public:
	CZipFileSystemEnumerator( const CZipFilesList &_zipfiles, IDataStorage *pStorage );

	virtual void STDCALL Reset( const char *pszMask );
	virtual bool STDCALL Next();
	virtual const SStorageElementStats* STDCALL GetStats() const { return &stats; }
};

class CZipFileSystem : public IDataStorage
{
	OBJECT_MINIMAL_METHODS( CZipFileSystem );
	//
	struct SZipDirHeader;
	struct SZipDirFileHeader;
	struct SZipLocalHeader;
	struct SZipArchiveFileInfo
	{
		int nIndex;													// index of this file in the zip
		int nSize;													// size of this file
		SZipFileDesc *pZipFile;							// zip-file, which contains this file
		//
		SZipArchiveFileInfo() : nIndex( -1 ), nSize( -1 ), pZipFile( 0 ) {  }
	};
	//
	CZipFilesList zipfiles;								// list of open zip files
	typedef std::hash_map<std::string, SZipArchiveFileInfo> CFilesMap;
	CFilesMap files;											// correspondence between the file name and the information necessary to obtain it
	//
	std::string szBase;
	DWORD dwStorageAccessMode;
public:
	CZipFileSystem( const char *pszName, DWORD dwAccessMode );
	// check if such thread exists
	virtual const bool STDCALL IsStreamExist( const char *pszName );
	// create and open a stream with the specified name and access rights
	virtual IDataStream* STDCALL CreateStream( const char *pszName, DWORD dwAccessMode );
	// open an existing stream with the specified name and permissions
	virtual IDataStream* STDCALL OpenStream( const char *pszName, DWORD dwAccessMode );
	// get stream description
	virtual bool STDCALL GetStreamStats( const char *pszName, SStorageElementStats *pStats );
	// kill storage element
	virtual bool STDCALL DestroyElement( const char *pszName );
	// rename element
	virtual bool STDCALL RenameElement( const char *pszOldName, const char *pszNewName );
	// enumeration of elements
	virtual IStorageEnumerator* STDCALL CreateEnumerator();
	// get the name of this storage
	virtual const char* STDCALL GetName() const { return szBase.c_str(); }
	// external service
	bool AddZipFile( IDataStream *pStream, const std::string &szZipFileName );
	// add new MOD
	virtual bool STDCALL AddStorage( IDataStorage *pStorage, const char *pszName );
	// remove MOD
	virtual bool STDCALL RemoveStorage( const char *pszName );
};

#endif // __ZIPFILESYSTEM_H__
