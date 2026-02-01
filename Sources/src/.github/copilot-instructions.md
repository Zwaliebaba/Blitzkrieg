# Blitzkrieg Game Engine - Copilot Instructions

## Project Overview
This is the **Blitzkrieg** game engine source code - a C++ Win32 real-time strategy game engine built with Visual Studio. The codebase follows a modular architecture with ~30 interconnected static libraries and DLLs.

## Architecture

### Core Module Hierarchy (see [ProjectDependencies.dgml](../ProjectDependencies.dgml))
- **Misc/** - Foundation layer: `IRefCount`, smart pointers (`CPtr<T>`), geometry (`CVec2`, `CVec3`), utilities
- **StreamIO/** - File I/O, serialization (`IStructureSaver`), data storage, INI/XML parsing
- **Formats/** - Game data formats (maps, animations, sprites, meshes, terrain)
- **Image/** - Image loading/processing
- **GFX/** - DirectX graphics abstraction (`IGFX`, textures, shaders, fonts)
- **SFX/** - Sound system (`ISFX`, `ISound`, playlists)
- **Scene/** - Rendering scene graph (`IScene`, `IVisObj`, cameras, particle effects)
- **AILogic/** - Game AI, pathfinding, unit behaviors, war fog
- **Main/** - Game loop (`IMainLoop`), multiplayer, save/load, transceivers
- **Game/** - Main executable entry point
- **GameTT/** - Mission/campaign logic, UI screens, menus
- **UI/** - UI framework (`IUIElement`, lists, buttons, trees)
- **editor/** - MFC-based content editor application

### Build Configurations
Use **A7.sln** for main game, **SCOTools.sln** for tools. Key configurations:
- `Debug` / `FastDebug` - Development builds
- `Release` / `BetaRelease` - Optimized builds  
- `Profiler` - Performance profiling builds
- Platform: **Win32 (x86)** only, toolset **v145**

## Key Patterns & Conventions

### Interface-Based Design
All major systems use COM-style interfaces inheriting from `IRefCount`:
```cpp
interface IMySystem : IRefCount {
  enum { tidTypeID = MY_SYSTEM_ID };  // Type ID for factory
  virtual void STDCALL DoSomething() = 0;
};
```

### Smart Pointers (defined in [Misc/Basic.h](../Misc/Basic.h))
- `CPtr<T>` - Standard ref-counted pointer (use for most cases)
- `CObj<T>` - Object-lifetime pointer (releases on object destruction)
- `CGDBPtr<T>` - Game database pointer (non-ref-counted, for static data)

### Object Factory & Serialization
Classes must implement these macros for factory registration and serialization:
```cpp
class CMyClass : public CTRefCount<IMyInterface> {
  OBJECT_SERVICE_METHODS(CMyClass);  // Factory + DestroyContents
  DECLARE_SERIALIZE;                  // Serialization support
};
```

### Singleton Access
Global services accessed via `GetSingleton<T>()`:
```cpp
GetSingleton<IScene>()->AddSound(...);
GetSingleton<ITextureManager>()->GetTexture(...);
GetSingleton<IGameTimer>()->GetGameTime();
```

### Precompiled Headers
Every `.cpp` file must include `#include "StdAfx.h"` as first line. Each module has its own `StdAfx.h` that includes `../Misc/Basic.h`.

### Include Paths
Use relative paths from module root: `#include "../Formats/fmtMap.h"`

## Geometry Types (in [Misc/Geometry.h](../Misc/Geometry.h))
- `CVec2`, `CVec3`, `CVec4` - Float vectors
- `CMatrix`, `SHMatrix` - Transformation matrices
- `CTRect<T>` - Rectangle template

## Serialization System
Uses chunk-based binary format via `IStructureSaver`:
```cpp
int CMyClass::operator&(IStructureSaver &ss) {
  ss.StartChunk('DATA');
  ss.DataChunk('val1', &myValue, sizeof(myValue));
  ss.FinishChunk();
  return 0;
}
```

## AI & Game Logic
- Unit types: Infantry (sprites), Mechanical (meshes) - see [Common/MapObject.h](../Common/MapObject.h)
- `IAILogic` handles simulation, `IAIEditor` handles map editing
- Diplomacy: `EDI_NEUTRAL`, `EDI_ENEMY`, `EDI_FRIEND`

## File Naming Conventions
- `*Internal.h/cpp` - Private implementation details
- `*Serialize.cpp` - Serialization code separated from logic
- `fmt*.h/cpp` - File format definitions (in Formats/)
- `i*.h` - Interface-heavy headers (e.g., `iMain.h`, `iMission.h`)

## Important Notes
- Use `STDCALL` calling convention for all interface methods
- Use `DECLARE_SUPER(BaseClass)` typedef when overriding
- Check `IsValid()` before using ref-counted objects
- Module boundaries enforced via DLL exports (`.def` files)
