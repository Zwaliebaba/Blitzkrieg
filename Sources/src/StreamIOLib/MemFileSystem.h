#ifndef __MEMFILESYSTEM_H__
#define __MEMFILESYSTEM_H__

#pragma ONCE

class CMemFileStream : public IDataStream
{
	OBJECT_NORMAL_METHODS( CMemFileStream );
	DECLARE_SERIALIZE;
	//
	CPtr<IDataStorage> pStorage;					// parent storage
	std::vector<BYTE> data;								// memory data of this stream
	int nBeginPos;												// locked begin position
	int nCurrPos;													// current stream position
	// file stats
	std::string szName;
	SStorageElementStats stats;
	//
	void ResizeToFit( int nSize )
	{
		if ( nSize > data.size() )
		{
			data.reserve( int(nSize * 1.3) );
			data.resize( nSize );
		}
	}
public:
	CMemFileStream() : nBeginPos( 0 ), nCurrPos( 0 ) { data.reserve( 1024 ); }
	CMemFileStream( const CMemFileStream &stream ) : data( stream.data ), nBeginPos( 0 ), nCurrPos( 0 ) {  }
	CMemFileStream( BYTE *pData, int nLength ) : data( pData, pData + nLength ), nBeginPos( 0 ), nCurrPos( 0 ) {  }
	explicit CMemFileStream( int nSize, IDataStorage *_pStorage ) : pStorage( _pStorage ), data( nSize ), nBeginPos( 0 ), nCurrPos( 0 ) {  }
	//
	void* GetBuffer() { return &(data[0]); }
	void SetStats( const SStorageElementStats &_stats )
	{
		stats = _stats;
		szName = _stats.pszName;
		stats.pszName = szName.c_str();
	}
	//
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
};

class CMemFileSystem : public IDataStorage
{
	OBJECT_MINIMAL_METHODS( CMemFileSystem );
	std::string szBase;
	DWORD dwStorageAccessMode;
	typedef std::hash_map< std::string, CPtr<IDataStream> > CStreamsMap;
	CStreamsMap streams;
public:
	CMemFileSystem( DWORD dwAccessMode );
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

#endif // __MEMFILESYSTEM_H__
