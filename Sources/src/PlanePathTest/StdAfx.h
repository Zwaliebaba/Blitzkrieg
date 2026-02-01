// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if !defined(AFX_STDAFX_H__E62D8585_ECF0_4202_AA6D_260587C04D03__INCLUDED_)
#define AFX_STDAFX_H__E62D8585_ECF0_4202_AA6D_260587C04D03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define _STLP_USE_MFC 1




#include <assert.h>

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <comutil.h>

#endif // _AFX_NO_AFXCMN_SUPPORT

// STL headers
#pragma warning( disable : 4503 4018 4786 4800 4290 4146 4244 4284 )
#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>

// Compatibility aliases for old hash_map/hash_set
namespace std {
  template<class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Alloc = std::allocator<std::pair<const Key, T>>>
  using hash_map = std::unordered_map<Key, T, Hash, Pred, Alloc>;
  template<class Key, class T, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Alloc = std::allocator<std::pair<const Key, T>>>
  using hash_multimap = std::unordered_multimap<Key, T, Hash, Pred, Alloc>;
  template<class Key, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Alloc = std::allocator<Key>>
  using hash_set = std::unordered_set<Key, Hash, Pred, Alloc>;
}

//
typedef UINT FAR *LPUINT;
using int64 = __int64;									// due to lack of 'long long' type support
using QWORD = unsigned __int64;					// quadra word
#define STDCALL __stdcall								// to use with interface function calls
// define 'interface' keyword
#ifndef interface
#define interface struct
#endif // interface
// define pragma once
#if _MSC_VER > 1000
#define ONCE once
#else
#define ONCE message ""
#endif // _MSC_VER > 1000

#include "..\Misc\Basic.h"							// base interfaces
#include "..\BugSlay\BugSlayer.h"				// cool asserts :)
#include "..\Misc\Tools.h"							// different usefull tools
#include "..\Misc\Geometry.h"						// geometry primitives and operations (vectors, matrix, quaternion, etc.)
#include "..\Misc\2DArray.h"						// 2-D array
#include "..\Misc\HashFuncs.h"					// different usefull hash functions
#include "..\Misc\StrProc.h"						// string processing functions

#include "..\StreamIO\Globals.h"				// globals - singleton, global var system, temp buffers, console, etc.
#include "..\StreamIO\StreamIO.h"				// stream I/O base interfaces
#include "..\StreamIO\DBIO.h"						// database I/O base interfaces
#include "..\StreamIO\StructureSaver.h"	// structure saver base interfaces
#include "..\StreamIO\SSHelper.h"				// structure saver helper classes
#include "..\StreamIO\DTHelper.h"				// data tree helper classes

#include "..\Main\GameTimer.h"
#include "..\Main\GameDB.h"

// in the file 'Specific.h' one can define own project-specific includes
#include "Specific.h"

// {{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E62D8585_ECF0_4202_AA6D_260587C04D03__INCLUDED_)
