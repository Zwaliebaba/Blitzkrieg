#include "stdafx.h"

#include "Registry_Types.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ////////////////////////////////////////////////
CRegistrySection::CRegistrySection(HKEY hKey, REGSAM samDesired, LPCTSTR pszRegistrySection)
{
  LONG result = ERROR_SUCCESS;
  DWORD dwDisposition;
  result = ::RegCreateKeyEx(hKey,
                            pszRegistrySection,
                            0,
                            nullptr,
                            REG_OPTION_NON_VOLATILE,
                            samDesired,
                            nullptr,
                            &hRegistrySection,
                            &dwDisposition);
  if (result != ERROR_SUCCESS) { hRegistrySection = nullptr; }
}

// ////////////////////////////////////////////////
CRegistrySection::~CRegistrySection()
{
  if (hRegistrySection != nullptr)
  {
    RegCloseKey(hRegistrySection);
    hRegistrySection = nullptr;
  }
}

// ////////////////////////////////////////////////
LONG CRegistrySection::LoadString(LPCTSTR pszRegistryKey, std::string *pszLoadValue, const std::string &rszDefaultValue) const
{
  if ((pszLoadValue != nullptr) && (hRegistrySection != nullptr))
  {
    (*pszLoadValue) = rszDefaultValue;

    DWORD dwLoadValueType;
    DWORD dwLoadValueLength = 0xFFF;
    BYTE pBuffer[0xFFF];
    LONG result = ERROR_SUCCESS;
    result = ::RegQueryValueEx(hRegistrySection,
                               pszRegistryKey,
                               nullptr,
                               &dwLoadValueType,
                               pBuffer,
                               &dwLoadValueLength);
    if ((result != ERROR_SUCCESS) || (dwLoadValueType != REG_SZ)) { (*pszLoadValue) = rszDefaultValue; }
    else { (*pszLoadValue) = std::string(reinterpret_cast<LPCTSTR>(pBuffer)); }
    return result;
  }
  return ERROR_INVALID_PARAMETER;
}

// ////////////////////////////////////////////////
LONG CRegistrySection::SaveString(LPCTSTR pszRegistryKey, const std::string &szSaveValue) const
{
  return ::RegSetValueEx(hRegistrySection,
                         pszRegistryKey,
                         0,
                         REG_SZ,
                         reinterpret_cast<const BYTE *>(szSaveValue.c_str()),
                         szSaveValue.size() + 1);
}

// ////////////////////////////////////////////////