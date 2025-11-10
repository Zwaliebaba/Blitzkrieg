#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#pragma ONCE

class CFileSystemEnumerator : public IStorageEnumerator
{
	OBJECT_MINIMAL_METHODS( CFileSystemEnumerator );
	//
	HANDLE hFind;													// handle to FindFile
	WIN32_FIND_DATA findinfo;							// result of the FileFind
	std::string szPath;                   // path to the file
	std::string szMask;										// enumeration mask
	std::string szBase;										// base dir for enumeration (from the storage)
	std::string szFoundFileName;					// filename
	SStorageElementStats stats;						// temporary data storage to fill each call
	//
	const bool IsFindValid() const { return hFind != INVALID_HANDLE_VALUE; }
private:
	bool FindFirstFile();
	bool FindNextFile();
	void Close();
	void FillStats();
	//
	// current file attributes check
	bool IsReadOnly() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) != 0; }
	bool IsSystem() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM ) != 0; }
	bool IsHidden() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) != 0; }
	bool IsTemporary() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY ) != 0; }
	bool IsNormal() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ) != 0; }
	bool IsArchive() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE ) != 0; }
	bool IsCompressed() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED ) != 0; }
	bool IsDirectory() const { return ( findinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0; }
	// special kind of directory: '.' 
	bool IsDots() const
	{
		return IsDirectory() &&
					 ( ( (findinfo.cFileName[0] == '.') ) &&
		         ( (findinfo.cFileName[1] == '\0') || ((findinfo.cFileName[1] == '.') && (findinfo.cFileName[2] == '\0')) ) );
	}
public:
	CFileSystemEnumerator( const std::string &_szBase )
		: hFind( INVALID_HANDLE_VALUE ), szBase( _szBase ) { Zero( stats ); }
	virtual ~CFileSystemEnumerator();
	//
	virtual void STDCALL Reset( const char *pszMask );
	virtual bool STDCALL Next();
	virtual const SStorageElementStats* STDCALL GetStats() const { return &stats; }
};

class CFileStream : public IDataStream
{
	OBJECT_MINIMAL_METHODS( CFileStream );
	//
	HANDLE hFile;													// file, associated with this stream
	int nStreamBegin;											// stream begin position
	DWORD dwAccessMode;										// access mode
	// file stats
	std::string szName;
	SStorageElementStats stats;
private:
	bool CanRead() const { return IsOpen() && (dwAccessMode & STREAM_ACCESS_READ); }
	bool CanWrite() const { return IsOpen() && (dwAccessMode & STREAM_ACCESS_WRITE); }
public:
	CFileStream( const char *pszFileName, DWORD dwAccessMode );
	virtual ~CFileStream();
	// read/write data
	virtual int STDCALL Read( void *pBuffer, int nLength );
	virtual int STDCALL Write( const void *pBuffer, int nLength );
	// declare the current position in the stream as the beginning of the stream
	virtual int STDCALL LockBegin();
	// return the start of the stream to the zero position
	virtual int STDCALL UnlockBegin();
	// current position in the stream
	virtual int STDCALL GetPos() const;
	// set the current position in the stream
	virtual int STDCALL Seek( int offset, STREAM_SEEK from );
	// get stream size
	virtual int STDCALL GetSize() const;
	// change stream size
	virtual bool STDCALL SetSize( int nSize );
	// copy 'nLength' byte from current stream position to current 'pDstStream' stream position
	virtual int STDCALL CopyTo( IDataStream *pDstStream, int nLength );
	// reset all cached data
	virtual void STDCALL Flush();
	// get information about the stream
	virtual void STDCALL GetStats( SStorageElementStats *pStats );
	//
	bool IsOpen() const { return hFile != INVALID_HANDLE_VALUE; }
};

class CFileSystem : public IDataStorage
{
	OBJECT_MINIMAL_METHODS( CFileSystem );
	std::string szBase;
	DWORD dwStorageAccessMode;
	//
	bool CreatePathRecursive( const std::string &szName );
public:
	CFileSystem( const char *pszName, DWORD dwAccessMode, bool bCreate );
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
	// add new MOD
	virtual bool STDCALL AddStorage( IDataStorage *pStorage, const char *pszName );
	// remove MOD
	virtual bool STDCALL RemoveStorage( const char *pszName );
};

#endif // __FILESYSTEM_H__
