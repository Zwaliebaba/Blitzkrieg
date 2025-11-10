/* * Copyright (c) 1999 
  */

/* *Purpose of this file:
  */


// ===========================================================================
// User-settable macros that control compilation:
// Features selection
// ===========================================================================

/* __STL_NO_SGI_IOSTREAMS: 
  */

#define  __STL_NO_SGI_IOSTREAMS	1


/* * This macro only works in non-SGI iostreams mode.
  */

// #define __STL_NO_NEW_IOSTREAMS 1

/* * Use this switch for embedded systems where no iostreams are available
  */
// # define __STL_NO_IOSTREAMS 1

/* * Set __STL_DEBUG to turn the "Debug Mode" on.
  */
#if defined(_DEBUG) && !defined(_STL_FAST_DEBUG)
#define __STL_DEBUG 1
#endif // defined(_DEBUG) && !defined(_STL_FAST_DEBUG)

/* * By default, STLport uses proxy technique to enable operator -> for
  */

// # define __STL_NO_PROXY_ARROW_OPERATOR 1
// # define __STL_NO_AUTO_PTR_PROXY_ARROW_OPERATOR 1


/* * If __STL_USE_OWN_NAMESPACE is in effect, STLport will try to rename std:: for the user
  */
// # define __STL_DONT_REDEFINE_STD 1

/* * Edit relative path below (or put full path) to get native 
  */
// # undef __STL_NATIVE_INCLUDE_PATH
// # define __STL_NATIVE_INCLUDE_PATH ../include
// same for C library headers like <cstring>
// # undef __STL_NATIVE_CPP_C_INCLUDE_PATH
// # define __STL_NATIVE_CPP_C_INCLUDE_PATH ../include
// same for C headers like <string.h>
// # undef __STL_NATIVE_C_INCLUDE_PATH
// # define __STL_NATIVE_C_INCLUDE_PATH ../include


/* * __STL_NO_RELOPS_NAMESPACE: if defined, don't put the relational
  */

// #define __STL_NO_RELOPS_NAMESPACE 1

/* * Use this option to catch uninitialized members in your classes.
  */

#if defined(_DEBUG) && !defined(_STL_FAST_DEBUG)
#define __STL_DEBUG_UNINITIALIZED 1
#endif // defined(_DEBUG) && !defined(_STL_FAST_DEBUG)

/* * Uncomment and provide a definition for the byte with which raw memory
  */
#if defined(_DEBUG) && !defined(_STL_FAST_DEBUG)
#define __STL_SHRED_BYTE 0xCD
#endif // defined(_DEBUG) && !defined(_STL_FAST_DEBUG)

// ===========================================================================
// Compatibility section
// ===========================================================================

/* * Define this macro to disable anachronistic constructs (like the ones used in HP STL and
  */
// define __STL_NO_ANACHRONISMS 1

/* * Define this macro to disable SGI/STLport extensions (for example, to make sure your code will 
  */
// define __STL_NO_EXTENSIONS 1

/* * Use obsolete overloaded template functions iterator_category(), value_type(), distance_type()
  */
// # define __STL_USE_OLD_HP_ITERATOR_QUERIES
// # define __STL_NO_OLD_HP_ITERATOR_QUERIES

/* * You should define this macro if compiling with MFC - STLport <stl/_config.h>
  */

// # define __STL_USE_MFC 1

/* * Use minimum set of default arguments on template classes that have more
  */

// # define __STL_MINIMUM_DEFAULT_TEMPLATE_PARAMS 1


/* * __STL_USE_SGI_STRING : Forces use of SGI string even if
  */

// #define __STL_USE_SGI_STRING 1

/* * __STL_WHOLE_NATIVE_STD : only meaningful in __STL_USE_OWN_NAMESPACE mode.
  */
// # define __STL_WHOLE_NATIVE_STD

// ===========================================================================



// Local Variables:
// mode:C++
// End:
