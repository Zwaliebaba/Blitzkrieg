#ifndef __MODFILESYSTEM_H__
#define __MODFILESYSTEM_H__

#pragma ONCE

class CModFileSystemEnumerator : public IStorageEnumerator
{
	OBJECT_MINIMAL_METHODS( CModFileSystemEnumerator );
	SStorageElementStats stats;						// temporary data storage to fill each call
	typedef std::map<std::string, SStorageElementStats> CStatsMap;
	CStatsMap files;											// all file stats
	CStatsMap::const_iterator itCurrFile;	// current iterated file
	bool bReset;
public:
	CModFileSystemEnumerator() {  }
	//
	void AddFile( const std::string &szName, const SStorageElementStats &stats ) { files[szName] = stats; }
	virtual void STDCALL Reset( const char *pszName );
	virtual bool STDCALL Next();
	virtual const SStorageElementStats* STDCALL GetStats() const { return &stats; }
};

class CModFileSystem : public IDataStorage
{
	OBJECT_MINIMAL_METHODS( CModFileSystem );
	//
	typedef std::pair<std::string, CPtr<IDataStorage> > SFileSystemDesc;
	typedef std::list<SFileSystemDesc> CFileSystemsList;
	CFileSystemsList filesystems;					// available file systems, last file system - base
	DWORD dwStorageAccessMode;						// access mode - READ-ONLY for this storage
public:
	CModFileSystem( const char *pszName, DWORD dwAccessMode );
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
	virtual const char* STDCALL GetName() const;
	// add new MOD
	virtual bool STDCALL AddStorage( IDataStorage *pStorage, const char *pszName );
	// remove MOD
	virtual bool STDCALL RemoveStorage( const char *pszName );
};

#endif // __MODFILESYSTEM_H__
