/* * Copyright (c) 1999 */

/* *Purpose of this file: */


// ===============================================================================
// User-settable macros that control compilation:
// Features selection
// ===============================================================================

/* _STLP_NO_OWN_IOSTREAMS: */

# define   _STLP_NO_OWN_IOSTREAMS	1


/* * This macro only works in non-SGI iostreams mode. */

// #define _STLP_NO_NEW_IOSTREAMS 1

/* * Use this switch for embedded systems where no iostreams are available */
// # define _STLP_NO_IOSTREAMS 1

/* * Set _STLP_DEBUG to turn the "Debug Mode" on. */
#if defined(_STL_FULL_DEBUG) || defined(_STL_RANGE_CHECK) || defined(_STL_MEM_CHECK)
#define   _STLP_DEBUG 1
#endif // defined(_STL_FULL_DEBUG) ||


/* * */
// #define _STLP_NO_CUSTOM_IO


/* * _STLP_NO_RELOPS_NAMESPACE: if defined, don't put the relational */

// #define _STLP_NO_RELOPS_NAMESPACE 1


/* * If _STLP_USE_OWN_NAMESPACE is in effect, STLport will try to rename std:: for the user */
// # define _STLP_DONT_REDEFINE_STD 1


/* * _STLP_WHOLE_NATIVE_STD : only meaningful in _STLP_USE_OWN_NAMESPACE mode. */
// # define _STLP_WHOLE_NATIVE_STD


/* * Use this option to catch uninitialized members in your classes. */
#if defined(_STL_FULL_DEBUG) || defined(_STL_MEM_CHECK)
# define _STLP_DEBUG_UNINITIALIZED 1
#endif // defined(_STL_FULL_DEBUG) ||

/* * Uncomment and provide a definition for the byte with which raw memory */
#if defined(_STL_FULL_DEBUG) || defined(_STL_MEM_CHECK)
#define _STLP_SHRED_BYTE 0xA3
#endif // defined(_STL_FULL_DEBUG) ||

/* * This option is for gcc users only and only affects systems where native linker */

// #define _STLP_GCC_USES_GNU_LD


// ===============================================================================
// Compatibility section
// ===============================================================================

/* * Define this macro to disable anachronistic constructs (like the ones used in HP STL and */
// define _STLP_NO_ANACHRONISMS 1

/* * Define this macro to disable STLport extensions (for example, to make sure your code will */
// define _STLP_NO_EXTENSIONS 1


/* * You should define this macro if compiling with MFC - STLport <stl/_config.h> */

// # define _STLP_USE_MFC 1


// boris : this setting is here as we cannot detect precedence of new Platform SDK automatically
// If you are using new PSDK with VC++ 6.0 or lower, please define this to get correct prototypes for InterlockedXXX functions
// # define _STLP_NEW_PLATFORM_SDK 1

/* * Use minimum set of default arguments on template classes that have more */

// # define _STLP_MINIMUM_DEFAULT_TEMPLATE_PARAMS 1

/* * Don't use SGI allocators - they produce memory leaks!!! */
#define _STLP_USE_NEWALLOC 1
// ===============================================================================

// Local Variables:
// mode:C++
// End:
