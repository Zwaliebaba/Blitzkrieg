#ifndef __NOTIFICATION_H__
#define __NOTIFICATION_H__
// //////////////////////////////////////////////////////////// 
// base class for exceptions
struct IGenericException
{
  virtual ~IGenericException() {}
};

// //////////////////////////////////////////////////////////// 
// exception containing a string and an error code
struct ICommonException : IGenericException
{
  virtual const char *GetString() const = 0;
  virtual DWORD GetCode() const { return 0; }
};

// //////////////////////////////////////////////////////////// 
struct IGuardException : ICommonException
{
  const char *GetString() const override = 0;
  virtual void Append(const char *pszFormat, ...) = 0;
};

// //////////////////////////////////////////////////////////// 
const char *DXErrorToString(HRESULT hErrorCode);
// //////////////////////////////////////////////////////////// 
// throw exception with appropriate formatting and HRESULT-to-string conversion
void ThrowExceptionHR(HRESULT dxrval, const char *pszFormat, ...) throw ( ICommonException *);
void ThrowException(const char *pszFormat, ...) throw ( ICommonException *);
void ThrowGuardException(const char *pszFormat, ...) throw ( IGuardException *);
// error notification
bool ReportErrorHR(HRESULT dxrval, const char *pszFormat, ...);
bool ReportError(const char *pszFormat, ...);
// warning notification
bool ReportWarningHR(HRESULT dxrval, const char *pszFormat, ...);
bool ReportWarning(const char *pszFormat, ...);
// just plain notification
bool ReportInfoHR(HRESULT dxrval, const char *pszFormat, ...);
bool ReportInfo(const char *pszFormat, ...);
// //////////////////////////////////////////////////////////// 
// program debug break
void BreakHere();
// issuing a message in the appropriate format
int ShowError(const char *pszString);
int ShowWarning(const char *pszString);
int ShowReport(const char *pszString);
// 'Show...()' functions work through ShowMBox() callbacks, which are added and removed by the following functions
using NotificationShowMBox = int(*)(const char *pszTitle, const char *pszString, UINT uType);
// for¤ ShowError(), which¤ is used¤ in ReportError()
void AddNotifyErrorShowMBoxFunction(NotificationShowMBox funct);
void RemoveNotifyErrorShowMBoxFunction(NotificationShowMBox funct);
// for¤ ShowWarning(), which¤ is used¤ in ReportWarning()
void AddNotifyWarningShowMBoxFunction(NotificationShowMBox funct);
void RemoveNotifyWarningShowMBoxFunction(NotificationShowMBox funct);
// for¤ ShowReport(), which¤ is used¤ in ReportReport()
void AddNotifyReportShowMBoxFunction(NotificationShowMBox funct);
void RemoveNotifyReportShowMBoxFunction(NotificationShowMBox funct);
// //////////////////////////////////////////////////////////// 
// ************************************************************************************************************************ //
// ** Guard macros for call stack display.
// ************************************************************************************************************************ //
// GUARD/UNGUARD macros.
// For showing calling stack when errors occur in major functions.
// Meant to be enabled in release builds.
#if defined( _DEBUG ) || !defined( _DO_GUARD )
#define GUARD( func )     { static const TCHAR __FUNC_NAME__[] = TEXT( #func );
#define UNGUARD           }
#else
#define GUARD( func ) { static const TCHAR __FUNC_NAME__[] = TEXT( #func ); try {
#define UNGUARD                                                                                   \
}                                                                                                 \
catch ( IGuardException *pException )                                                             \
{                                                                                                 \
  pException->Append( __FUNC_NAME__ );                                                            \
  throw pException;                                                                               \
}                                                                                                 \
catch ( ICommonException *pException )                                                            \
{                                                                                                 \
  ThrowGuardException( "\"%s\" in the function \"%s\"", pException->GetString(), __FUNC_NAME__ ); \
}                                                                                                 \
catch ( ... )                                                                                     \
{                                                                                                 \
  ThrowGuardException( __FUNC_NAME__ );                                                           \
}                                                                                                 \
}
#endif // defined( _DEBUG ) || 
// GUARD_SLOW/UNGUARD_SLOW macros.
// For showing calling stack when errors occur in performance-critical functions.
// Meant to be disabled in release builds.
#if defined( _DEBUG ) || !defined( _DO_GUARD ) || !defined( _DO_GUARD_SLOW )
#define GUARD_SLOW( func ) {
#define UNGUARD_SLOW       }
#else
#define GUARD_SLOW( func )   GUARD( func )
#define UNGUARD_SLOW         UNGUARD
#endif // defined( _DEBUG ) || 
// //////////////////////////////////////////////////////////// 
#endif // __NOTIFICATION_H__