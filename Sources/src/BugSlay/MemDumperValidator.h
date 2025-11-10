/* ----------------------------------------------------------------------
 */

#ifndef _MEMDUMPERVALIDATOR_H
#define _MEMDUMPERVALIDATOR_H
// //////////////////////////////////////////////////////////// 
// This file should not be included directly, include Bugslayer.h
// instead.
#ifndef __BUGSLAYER_H__
#error "Include BugSlayer.h instead of this file directly!"
#endif  // __BUGSLAYER_H__
// Include the main header.
#include "MSJDBG.h"
// //////////////////////////////////////////////////////////// 
#ifdef __cplusplus
extern "C"
{
#endif      // __cplusplus
// //////////////////////////////////////////////////////////// 
// This library can only be used in _DEBUG builds.
#ifdef _DEBUG

// ////////////////////////////////////////////////////////////////////
// The typedefs for the dumper and validator functions.
// ////////////////////////////////////////////////////////////////////
// The memory dumper function.  
// block of memory.  
// block any way it likes but it might be nice if it uses the same
// Debug CRT reporting mechanism that everything else in the runtime
// uses.
using PFNMEMDUMPER = void(*)(const void *);
// The validator function.  
// validate and the second parameter is the context information passed
// to the ValidateAllBlocks function.
using PFNMEMVALIDATOR = void(*)(const void *, const void *);

// ////////////////////////////////////////////////////////////////////
// Useful Macros.
// ////////////////////////////////////////////////////////////////////
// The macro used to set a client block value.  
// approved means of setting a value for the dwValue field in the
// DVINFO structure below.
#define CLIENT_BLOCK_VALUE(x) (_CLIENT_BLOCK|(x<<16))
// A macro to pick out the subtype.
#define CLIENT_BLOCK_SUBTYPE(x) ((x >> 16) & 0xFFFF)

// ////////////////////////////////////////////////////////////////////
// The header used to initialize the dumper and validator for a specific
// type of client block.
// ////////////////////////////////////////////////////////////////////
typedef struct tag_DVINFO
{
  // The value for the client blocks.  
  // CLIENT_BLOCK_VALUE macro above.  
  // for how to have the library assign this number.
  unsigned long dwValue;
  // The pointer to the dumper function.
  PFNMEMDUMPER pfnDump;
  // The pointer to the dumper function.
  PFNMEMVALIDATOR pfnValidate;
} DVINFO, *LPDVINFO;

/* ----------------------------------------------------------------------
 */
int STDCALL AddClientDV(LPDVINFO lpDVInfo);

/* ----------------------------------------------------------------------
 */
void STDCALL ValidateAllBlocks(void *pContext);

#ifdef __cplusplus
// ////////////////////////////////////////////////////////////////////
// Helper C++ class macros.
// ////////////////////////////////////////////////////////////////////
// Declare this macro in your class just like the MFC ones.
#define DECLARE_MEMDEBUG(classname)                                 \
public   :                                                          \
    static DVINFO  m_stDVInfo ;                                     \
    static void ClassDumper ( const void * pData ) ;                \
    static void ClassValidator ( const void * pData ,               \
                                     const void * pContext )       ;\
    static void * operator new ( size_t nSize )                     \
    {                                                               \
        if ( 0 == m_stDVInfo.dwValue )                              \
        {                                                           \
            m_stDVInfo.pfnDump     = classname::ClassDumper ;       \
            m_stDVInfo.pfnValidate = classname::ClassValidator ;    \
            AddClientDV ( &m_stDVInfo ) ;                           \
        }                                                           \
        return ( _malloc_dbg ( nSize                   ,            \
                               (int)m_stDVInfo.dwValue ,            \
                               __FILE__                ,            \
                               __LINE__                 ) ) ;       \
    }                                                               \
    static void * operator new ( size_t nSize        ,              \
                                 char * lpszFileName ,              \
                                 int    nLine         )             \
    {                                                               \
        if ( 0 == m_stDVInfo.dwValue )                              \
        {                                                           \
            m_stDVInfo.pfnDump     = classname::ClassDumper ;       \
            m_stDVInfo.pfnValidate = classname::ClassValidator ;    \
            AddClientDV ( &m_stDVInfo ) ;                           \
        }                                                           \
        return ( _malloc_dbg ( nSize                   ,            \
                               (int)m_stDVInfo.dwValue ,            \
                               lpszFileName            ,            \
                               nLine                    ) ) ;       \
    }                                                               \
    static void operator delete ( void * pData )                    \
    {                                                               \
        _free_dbg ( pData , (int)m_stDVInfo.dwValue ) ;             \
    }

// Declare this one at the top of the CPP file.
#define IMPLEMENT_MEMDEBUG(classname)                               \
    DVINFO  classname::m_stDVInfo

// The macro for memory debugging allocations.  
// then it can be used.
#ifdef DEBUG_NEW
#define MEMDEBUG_NEW DEBUG_NEW
#else
#define MEMDEBUG_NEW new ( __FILE__ , __LINE__ )
#endif

#endif      // __cplusplus defined.

// ////////////////////////////////////////////////////////////////////
// Helper C macros.
// ////////////////////////////////////////////////////////////////////

// For C style allocations, here is the macro to use.  
// with C it is not so easy to use the auto-increment feature of
// AddClientDV.
#define INITIALIZE_MEMDEBUG(bType , pfnD , pfnV )   \
    {                                               \
        DVINFO dvInfo ;                             \
        dvInfo.dwValue = bType ;                    \
        dvInfo.pfnDump = pfnD ;                     \
        dvInfo.pfnValidate = pfnV ;                 \
        AddClientDV ( &dvInfo ) ;                   \
    }

// The macros that map the C-style allocations.  
// you use macros to wrap these so you don't have to remember which
// client block value to drag around with each memory usage function.
#define MEMDEBUG_MALLOC(bType , nSize)  \
            _malloc_dbg ( nSize , bType , __FILE__ , __LINE__ )
#define MEMDEBUG_REALLOC(bType , pBlock , nSize)    \
            _realloc_dbg( pBlock , nSize , bType , __FILE__ , __LINE__ )
#define MEMDEBUG_EXPAND(bType , pBlock , nSize )    \
            _expand_dbg( pBlock , nSize , bType , __FILE__ , __LINE__ )
#define MEMDEBUG_FREE(bType , pBlock)   \
            _free_dbg ( pBlock , bType )
#define MEMDEBUG_MSIZE(bType , pBlock)  \
            _msize_dbg ( pBlock , bType )

// Macro to call ValidateAllBlocks
#define VALIDATEALLBLOCKS(x)   ValidateAllBlocks ( x )

#else       // _DEBUG is NOT defined

#ifdef __cplusplus
#define DECLARE_MEMDEBUG(classname)
#define IMPLEMENT_MEMDEBUG(classname)
#define MEMDEBUG_NEW new
#endif      // __cplusplus

#define MEMDEBUG_MALLOC(bType , nSize)              malloc ( nSize )
#define MEMDEBUG_REALLOC(bType , pBlock , nSize)    \
                                              realloc ( pBlock , nSize )
#define MEMDEBUG_EXPAND(bType , pBlock , nSize)     \
                                              _expand ( pBlock , nSize )
#define MEMDEBUG_FREE(bType , pBlock)               free ( pBlock )
#define MEMDEBUG_MSIZE(bType , pBlock)              _msize ( pBlock )

#define VALIDATEALLBLOCKS(x)
// //////////////////////////////////////////////////////////// 
#endif      // _DEBUG
// //////////////////////////////////////////////////////////// 
#ifdef __cplusplus
}
#endif      // __cplusplus
// //////////////////////////////////////////////////////////// 
#endif      // _MEMDUMPERVALIDATOR_H