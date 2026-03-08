/* ----------------------------------------------------------------------
   */

#ifndef _DIAGASSERT_H
#define _DIAGASSERT_H

#ifndef __cplusplus
extern "C"
{

#endif  // __cplusplus

/* ////////////////////////////////////////////////////////////////////
                                 */
// Keep the core stuff available in both release and debug builds.
// Uses the global assert flags.
#define DA_USEDEFAULTS      0x0000
// Turns on showing the assert in a messagebox.  
#define DA_SHOWMSGBOX       0x0001
// Turns on showing the assert as through OutputDebugString.  
// the default.
#define DA_SHOWODS          0x0002
// Shows a stack trace in the assert.  
// ASSERT macro, and on in the SUPERASSERT macro.
#define DA_SHOWSTACKTRACE   0x0004

/* ----------------------------------------------------------------------
 */
DWORD STDCALL SetDiagAssertOptions(DWORD dwOpts);

/* ----------------------------------------------------------------------
 */
BOOL STDCALL AddDiagAssertModule(HMODULE hMod);

/* ----------------------------------------------------------------------
 */
BOOL STDCALL DiagAssert(DWORD dwOverrideOpts, LPCTSTR szMsg, LPCSTR szFile, DWORD dwLine);

/* ----------------------------------------------------------------------
 */
void STDCALL DiagOutput(LPCTSTR szFmt, ...);

/* ////////////////////////////////////////////////////////////////////
                            */
#ifdef _DEBUG

/* ////////////////////////////////////////////////////////////////////
                                 */
// The different options that can be set with SetDiagAssertOptions to
// set the global options.  
// the first parameter, then that value will override whatever the
// global settings are.

// Undefine the things I want to take over.
#ifdef ASSERT
#undef ASSERT
#endif

#ifdef assert
#undef assert
#endif

#ifdef VERIFY
#undef VERIFY
#endif

// The assert macro used by ASSERT and SUPERASSERT
#define ASSERTMACRO(a,x)                                            \
    do                                                              \
    {                                                               \
        if ( !(x)                                               &&  \
             DiagAssert ( a , _T ( #x ) , __FILE__  , __LINE__)    )\
        {                                                           \
                DebugBreak ( ) ;                                    \
        }                                                           \
    } while (0)

// The normal assert.  
#define ASSERT(x) ASSERTMACRO(DA_USEDEFAULTS,x)

// Do the lowercase one.
#define assert ASSERT

// Trust, but verify.
#define VERIFY(x)   ASSERT(x)

// Full blow assert with all the trimmings.
#define SUPERASSERT(x) ASSERTMACRO ( DA_SHOWSTACKTRACE |    \
                                        DA_SHOWMSGBOX  |    \
                                        DA_SHOWODS      ,   \
                                     x                  , )

// The options macro.
#define SETDIAGASSERTOPTIONS(x) SetDiagAssertOptions(x)

// The add module macro.
#define ADDDIAGASSERTMODULE(x) AddDiagAssertModule(x)

#ifdef TRACE
#undef TRACE
#endif

#ifdef TRACE0
#undef TRACE0
#endif
#ifdef TRACE1
#undef TRACE1
#endif
#ifdef TRACE2
#undef TRACE2
#endif
#ifdef TRACE3
#undef TRACE3
#endif

// The TRACE macros.
#ifdef __cplusplus
#define TRACE   ::DiagOutput
#endif

#define TRACE0(sz)              ::DiagOutput(_T("%s"), _T(sz))
#define TRACE1(sz, p1)          ::DiagOutput(_T(sz), p1)
#define TRACE2(sz, p1, p2)      ::DiagOutput(_T(sz), p1, p2)
#define TRACE3(sz, p1, p2, p3)  ::DiagOutput(_T(sz), p1, p2, p3)

#else   // !_DEBUG
/* ////////////////////////////////////////////////////////////////////
                        */
// remove any previous ASSERT definitions
#ifdef ASSERT
#undef ASSERT
#endif // ASSERT

#define ASSERTMACRO(a,x)
#define ASSERT(x)
#define VERIFY(x)   ((void)(x))
#define SUPERASSERT(x)
#define SETDIAGASSERTOPTIONS(x)
#define ADDDIAGASSERTMODULE(x)

#ifdef __cplusplus
// inline void TraceOutput(LPCTSTR, ...) { }
#define TRACE   (void)0
#endif

#define TRACE0(fmt)
#define TRACE1(fmt,arg1)
#define TRACE2(fmt,arg1,arg2)
#define TRACE3(fmt,arg1,arg2,arg3)

#endif  // _DEBUG


#ifndef __cplusplus
}
#endif  // __cplusplus

#endif  // _DIAGASSERT_H