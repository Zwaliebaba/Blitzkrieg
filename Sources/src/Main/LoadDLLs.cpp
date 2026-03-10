#include "pch.h"

#include "../Misc/Win32Helper.h"
#include "../Misc/FileUtils.h"
#include "../RandomMapGen/Registry_Types.h"

using namespace NWin32Helper;

namespace NMain
{
  static constexpr char GAME_REGISTRY_FOLDER[] = "Software\\Nival Interactive\\Blitzkrieg";
  static constexpr char GAME_REGISTRY_KEY[] = "InstallFolder";

  bool SetGameDirectory()
  {
#if defined(_FINALRELEASE) || defined(_BETARELEASE)
    std::string szModulePath;
    std::string szGameFolder;
    const std::string szModuleName("game.exe");

    // get module path and compare it with installed game folder
    {
      char buffer[2048];
      memset(buffer, 0, 2048);
      ::GetModuleFileName(0, buffer, 2048);
      szModulePath = buffer;
      szModulePath.resize(szModulePath.rfind('\\') + 1);
      NStr::ToLower(szModulePath);
    }

    // get registry name
    bool bNeedWriteRegistry = false;
    {
      CRegistrySection registrySection(HKEY_LOCAL_MACHINE, KEY_READ, GAME_REGISTRY_FOLDER);
      registrySection.LoadString(GAME_REGISTRY_KEY, &szGameFolder, "");
      NStr::ToLower(szGameFolder);
      bNeedWriteRegistry = (szGameFolder != (szModulePath + szModuleName));
    }

    ::SetCurrentDirectory(szModulePath.c_str());
    // write to registry
    if (bNeedWriteRegistry)
    {
      CRegistrySection registrySection(HKEY_LOCAL_MACHINE, KEY_WRITE, GAME_REGISTRY_FOLDER);
      return registrySection.SaveString(GAME_REGISTRY_KEY, szModulePath + szModuleName);
    }
#endif // defined(_FINALRELEASE) || 

    return true;
  }

};

class CGameDirectoryAutoMagic
{
public:
  CGameDirectoryAutoMagic()
  {
    NMain::SetGameDirectory();
  }
};

static CGameDirectoryAutoMagic gameDirectoryAutoMagic;