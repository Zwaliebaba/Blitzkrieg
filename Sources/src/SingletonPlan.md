# Plan: Remove DLL Plugin System — Convert to Static Libraries

## Status Summary (validated against codebase)

| Phase | Description | Status |
|-------|-------------|--------|
| **Phase 1** | Convert .vcxproj to StaticLibrary | ✅ **DONE** |
| **Phase 2** | Add project references to executables | ✅ **DONE** |
| **Phase 3** | Expose factory accessors | ✅ **DONE** |
| **Phase 4** | Rewrite Initialization.cpp | ✅ **DONE** |
| **Phase 5** | Remove DLL loading from LoadDLLs.cpp | ✅ **DONE** |
| **Phase 6** | Consolidate GlobalsLoader + fix static init order | ✅ **DONE** |
| **Phase 7** | Handle special cases | ✅ **DONE** |
| **Phase 8** | Remove dead code | ✅ **DONE** |
| **Phase 9** | Clean up iMain.h and headers | ✅ **DONE** |

All phases complete. Build verified.

## 1. Problem Statement

The engine currently loads subsystem modules (Image, GFX, SFX, etc.) as **DLLs at runtime** via
`LoadAllModules()`, which scans the working directory for `*.dll` files, calls
`GetModuleDescriptor()` to obtain an `SModuleDescriptor`, and uses `pDesc->pFactory` to create
objects. This mechanism is unnecessary — the set of modules is fixed at compile time and does not
vary at runtime. Converting all DLLs to **static libraries (.lib)** will:

- Eliminate runtime DLL discovery and loading complexity.
- Remove the `GlobalsLoader` hook system used to share globals across DLL boundaries.
- Enable link-time dead-code elimination (smaller binaries).
- Simplify debugging (no cross-DLL symbol issues).
- Remove `.def` export files and `SModuleDescriptor` / `IModuleChecker` infrastructure.

---

## 2. Current Architecture

### 2.1 Module Loading Flow

```
Game.exe startup
  └─ CModuleLoadAutoMagic (static global in Main\LoadDLLs.cpp)
       ├─ LoadAllModules(".")          — scans *.dll, calls GetModuleDescriptor()
       │    └─ stores SDllModule { CDLLHandle*, SModuleDescriptor* } in list
       └─ for each module:
            ├─ GetSLS()->AddFactory(pDesc->pFactory)
            └─ pDesc->pChecker->SetModuleFunctionalityLimits()  (if checker exists)
```

### 2.2 Object Creation via Module Descriptor

In `Main\Initialization.cpp`, objects from DLL modules are created like:
```cpp
const SModuleDescriptor *pDesc = GetModuleDesc(IMAGE_IMAGE);
CPtr<IImageProcessor> pIP = CreateObject<IImageProcessor>(pDesc->pFactory, IMAGE_PROCESSOR);
```

Objects from static-lib modules (Main, GameTT) already use the simpler form:
```cpp
CPtr<ITextManager> pTextMan = CreateObject<ITextManager>(TEXT_MANAGER);
// uses GetCommonFactory() internally — searches all registered factories
```

### 2.3 Globals Sharing Across DLL Boundaries

Each DLL has a `GlobalsLoader.cpp` that loads `StreamIO.dll` at startup to obtain shared global
pointers via exported hook functions:

| Exported Hook               | Global it populates            | Defined in                   |
|-----------------------------|-------------------------------|------------------------------|
| `GetSLS_Hook`               | `g_pGlobalSaveLoadSystem`     | `StreamIO\SaveLoadSystem.cpp`|
| `GetSingletonGlobal_Hook`   | `g_pGlobalSingleton`          | `StreamIO\Singleton.cpp`     |
| `GetTempRawBuffer_Hook`     | `g_pfnGlobalGetTempRawBuffer` | `StreamIO\Singleton.cpp`     |

**When all modules are static libs**, these globals exist in a single address space and can be
initialized directly — no hook exports needed. The existing `StreamIOLib` project already
demonstrates this pattern (see `StreamIOLib\GetSLS.cpp`).

### 2.4 Projects Currently Built as DLLs

| Project     | .def File           | Has IModuleChecker | Has DllMain | Special Exports |
|-------------|--------------------|--------------------|-------------|----------------|
| AILogic     | AILogic.def        | No                 | No          | —              |
| Anim        | Animation.def      | No                 | No          | —              |
| GFX         | GFX.def            | **Yes** (CGFXModuleChecker) | No | —        |
| Image       | Image.def          | No                 | No          | —              |
| Input       | Input.def          | No                 | No          | —              |
| Net         | net.def            | No                 | No          | —              |
| Scene       | Scene.def          | No                 | **Yes** (hDLLInstance) | —   |
| SFX         | Sound.def          | No                 | No          | —              |
| StreamIO    | StreamIO.def       | No                 | No          | GetSLS_Hook, GetSingletonGlobal_Hook, GetTempRawBuffer_Hook |
| UI          | UI.def             | No                 | No          | —              |
| GameTT      | GameTT.def         | No                 | No          | —              |

### 2.5 Projects Already Static Libraries

| Project   | Notes |
|-----------|-------|
| Misc      | Foundation layer |
| Common    | Shared game types |
| Formats   | Data format definitions |
| Main      | Game loop, initialization |

### 2.6 Executables That Link Modules

| Executable | Currently References |
|------------|---------------------|
| **Game**   | Main, Misc, Formats, StreamIO, GameTT, Net, BugSlay, LuaLib, RandomMapGen + DLLs via runtime loading |
| **editor** | Main, Misc, Common, Formats, StreamIO, AILogic, Anim, GFX, Image, Input, GameTT, Scene, LuaLib, RandomMapGen, BugSlay + DLLs via runtime loading |

---

## 3. Target Architecture

```
Game.exe / editor.exe
  └─ Links statically against ALL module .lib files
  └─ Single GlobalsLoader.cpp initializes globals directly (no DLL hooks)
  └─ Initialization registers all factories with GetSLS()->AddFactory(...)
  └─ CreateObject<T>(TYPE_ID) works via GetCommonFactory() for all types
  └─ No SModuleDescriptor, no GetModuleDesc, no LoadAllModules
```

---

## 4. Step-by-Step Migration Plan

### Phase 1: Convert .vcxproj Files from DynamicLibrary to StaticLibrary ✅ DONE

**Files to modify** (change `<ConfigurationType>DynamicLibrary</ConfigurationType>` → `<ConfigurationType>StaticLibrary</ConfigurationType>` in all non-Debug configurations):

- `AILogic\AILogic.vcxproj`
- `Anim\Anim.vcxproj`
- `GFX\GFX.vcxproj`
- `Image\Image.vcxproj`
- `Input\Input.vcxproj`
- `Net\Net.vcxproj`
- `Scene\Scene.vcxproj`
- `SFX\SFX.vcxproj`
- `StreamIO\StreamIO.vcxproj`
- `UI\UI.vcxproj`
- `GameTT\GameTT.vcxproj`

Also remove `<ModuleDefinitionFile>` entries pointing to `.def` files from each `.vcxproj`.

**Risk**: LOW — changing project type is a straightforward MSBuild property change.

---

### Phase 2: Add Project References to Executables ✅ DONE

Since DLLs were loaded at runtime, `Game.vcxproj` does not currently reference all of them.
Add `<ProjectReference>` entries so the linker pulls in each static lib.

**Game.vcxproj** — add references to:
- `AILogic\AILogic.vcxproj`
- `Anim\Anim.vcxproj`
- `GFX\GFX.vcxproj`
- `Image\Image.vcxproj`
- `Input\Input.vcxproj`
- `Scene\Scene.vcxproj`
- `SFX\SFX.vcxproj`
- `UI\UI.vcxproj`

(StreamIO, GameTT, Net are already referenced.)

**editor.vcxproj** — verify all are already referenced (they appear to be). Add `Net` and `SFX` if
missing.

Also add any system `.lib` dependencies that were previously resolved by each DLL's own linker
settings (e.g., `d3d8.lib`, `dsound.lib`, `dinput8.lib`, `dxguid.lib`, `ws2_32.lib`) to the
executable's `<AdditionalDependencies>`.

**Risk**: MEDIUM — may uncover missing references; requires iterative build-fix.

---

### Phase 3: Expose Factory Accessors from Each Module ✅ DONE

Each module already has a static factory object (e.g., `theGFXObjectFactory`). Add a public
accessor function following the existing `GetMainObjectFactory()` pattern.

**For each module, in its `*ObjectFactory.cpp`**:

| Module   | Accessor to add                      | Factory variable              |
|----------|--------------------------------------|-------------------------------|
| AILogic  | `IObjectFactory* GetAILogicObjectFactory()` | `theAILogicObjectFactory` |
| Anim     | `IObjectFactory* GetAnimObjectFactory()`    | (check variable name)     |
| GFX      | `IObjectFactory* GetGFXObjectFactory()`     | `theGFXObjectFactory`     |
| Image    | `IObjectFactory* GetImageObjectFactory()`   | `theImageObjectFactory`   |
| Input    | `IObjectFactory* GetInputObjectFactory()`   | (check variable name)     |
| Net      | `IObjectFactory* GetNetObjectFactory()`     | (check variable name)     |
| Scene    | `IObjectFactory* GetSceneObjectFactory()`   | `theSceneObjectFactory`   |
| SFX      | `IObjectFactory* GetSFXObjectFactory()`     | (check variable name)     |
| StreamIO | `IObjectFactory* GetStreamIOObjectFactory()` | `theStreamIOObjectFactory` |
| UI       | `IObjectFactory* GetUIObjectFactory()`      | (check variable name)     |
| GameTT   | `IObjectFactory* GetMissionObjectFactory()` | Already exists             |

Declare each in the corresponding `*ObjectFactory.h` (or a shared header) so `Initialization.cpp`
can call them.

**Risk**: LOW — mechanical addition.

---

### Phase 4: Rewrite `Main\Initialization.cpp` ✅ DONE

Replace all `GetModuleDesc(XXX)->pFactory` usage with direct factory registration and common-factory
`CreateObject`.

**Before** (current pattern):
```cpp
const SModuleDescriptor *pDesc = GetModuleDesc(IMAGE_IMAGE);
CPtr<IImageProcessor> pIP = CreateObject<IImageProcessor>(pDesc->pFactory, IMAGE_PROCESSOR);
```

**After** (target pattern):
```cpp
CPtr<IImageProcessor> pIP = CreateObject<IImageProcessor>(IMAGE_PROCESSOR);
```

This works because all factories will be registered with `GetSLS()->AddFactory(...)` at the start
of `Initialize()`.

**Changes to `NMain::Initialize()`**:

1. At the top, register all module factories:
   ```cpp
   GetSLS()->AddFactory(GetMainObjectFactory());
   GetSLS()->AddFactory(GetImageObjectFactory());
   GetSLS()->AddFactory(GetInputObjectFactory());
   GetSLS()->AddFactory(GetGFXObjectFactory());
   GetSLS()->AddFactory(GetSFXObjectFactory());
   GetSLS()->AddFactory(GetAnimObjectFactory());
   GetSLS()->AddFactory(GetSceneObjectFactory());
   GetSLS()->AddFactory(GetAILogicObjectFactory());
   GetSLS()->AddFactory(GetStreamIOObjectFactory());
   GetSLS()->AddFactory(GetUIObjectFactory());
   GetSLS()->AddFactory(GetMissionObjectFactory());
   ```

2. Replace every `GetModuleDesc(XXX)->pFactory` → `GetCommonFactory()` or just use the single-arg
   `CreateObject<T>(typeID)` form.

3. Move `CGFXModuleChecker::SetModuleFunctionalityLimits()` call to an explicit invocation during
   GFX initialization (it was previously called by `CModuleLoadAutoMagic`).

**Affected call sites** (10 instances in `Initialization.cpp`):

| Line (approx) | Current Module Lookup            | Replacement |
|---------------|----------------------------------|-------------|
| 79  | `GetModuleDesc(IMAGE_IMAGE)`     | Remove — use `CreateObject<IImageProcessor>(IMAGE_PROCESSOR)` |
| 91  | `GetModuleDesc(INPUT_INPUT)`     | Remove — use `CreateObject<IInput>(INPUT_INPUT)` |
| 98  | `GetModuleDesc(GFX_GFX)`        | Remove — use `CreateObject<IGFX>(GFX_GFX)` etc. |
| 117 | `GetModuleDesc(SFX_SFX)`        | Remove — use `CreateObject<ISFX>(SFX_SFX)` etc. |
| 131 | `GetModuleDesc(ANIM_ANIM)`      | Remove — use `CreateObject<IAnimationManager>(ANIM_ANIMATION_MANAGER)` |
| 139 | `GetModuleDesc(SCENE_SCENE)`    | Remove — use `CreateObject<ICamera>(SCENE_CAMERA)` etc. |
| 157 | `GetModuleDesc(AI_AI)`          | Remove — use `CreateObject<IAILogic>(AI_LOGIC)` etc. |
| 178 | `GetModuleDesc(SCENE_SCENE)` (2nd) | Remove — use `CreateObject<IScene>(SCENE_SCENE)` etc. |
| 191 | `GetModuleDesc(UI_BASE_VALUE)`  | Remove (already unused for creation, only mask manager created) |

**Risk**: LOW — the code already uses `CreateObject<T>(typeID)` for Main-factory objects; same
pattern is extended to all.

---

### Phase 5: Rewrite `Main\LoadDLLs.cpp` — Remove DLL Loading ✅ DONE

**Remove**:
- `SDllModule` struct
- `CModulesList modules` list
- `GetModuleDesc(int nType)` function
- `LoadAllModules()` function
- `UnloadAllModules()` — replace body with just `GetSingletonGlobal()->Done()`
- `CModuleLoadAutoMagic` class and its static instance
- `GetFirstModuleDesc()` / `GetNextModuleDesc()` / `GetModuleFileNameByDesc()` — search for callers
  first; likely only used by editor's module-info display, which can be removed or stubbed.

**Keep or refactor**:
- `SetGameDirectory()` — still needed for registry/working-directory setup
- `CheckBetaKey()` — still needed

**Update `iMain.h`**:
- Remove `GetModuleDesc()`, `LoadAllModules()`, `GetFirstModuleDesc()`, `GetNextModuleDesc()`,
  `GetModuleFileNameByDesc()` declarations from `NMain` namespace.

**Update `NMain::Finalize()`**:
- Replace `UnloadAllModules()` call with `GetSingletonGlobal()->Done()`.

**Risk**: MEDIUM — need to verify no other code calls these functions.

---

### Phase 6: Consolidate GlobalsLoader.cpp Files ✅ DONE

> **Implementation completed:**
> - 6.1 ✅ `GetSLS_Hook()` converted to Construct On First Use (Meyers' singleton)
> - 6.2 ✅ `GetSingletonGlobal_Hook()` converted to Construct On First Use (Meyers' singleton)
> - 6.2 ✅ `__REDUCED_SINGLETON__` blocks removed from `StreamIO\Singleton.cpp`
> - 6.2 ✅ `STempBufferAutomatic` replaced with self-initializing lambda in `GetTempRawBuffer_Hook()`
> - 6.3 ✅ `Game\GlobalsLoader.cpp` and `editor\GlobalsLoader.cpp` — removed `GetSingleton<IRandomGen>()` call from static constructor; 19 old library GlobalsLoader files deleted from disk
> - 6.4 ✅ `BugSlay` retains `_DONT_LOAD_SINGLETONS`/`_DONT_LOAD_STREAMIO` (acceptable — kept as DLL)
> - 6.5 ✅ `g_pGlobalRandomGen` initialized in `NMain::Initialize()` after factory registration
> - 6.6 ✅ `GetSingletonGlobal()` and `GetSLS()` have `NI_ASSERT` null guards

Currently **every** project (20 copies!) has its own `GlobalsLoader.cpp` with identical boilerplate
that loads `StreamIO.dll` to obtain `g_pGlobalSaveLoadSystem`, `g_pGlobalSingleton`, and
`g_pfnGlobalGetTempRawBuffer`.

With static linking, all modules share the same address space. Follow the `StreamIOLib` pattern:

1. **In `StreamIO\SaveLoadSystem.cpp`**: Fix **same static initialization order fiasco** as
   `theSingleton`. `CSaveLoadSystem theSaveLoadSystem` is a **file-scope static** (line 18).
   `GetSLS_Hook()` returns `&theSaveLoadSystem`, which suffers the identical vtable-null bug:
   if `CGlobalsLoader` runs first, the address is valid but the object is unconstructed.

   **Fix**: Apply the same Construct On First Use pattern:

   ```cpp
   // BEFORE (StreamIO\SaveLoadSystem.cpp line 18-19):
   CSaveLoadSystem theSaveLoadSystem;
   ISaveLoadSystem * STDCALL GetSLS_Hook() { return &theSaveLoadSystem; }

   // AFTER:
   ISaveLoadSystem * STDCALL GetSLS_Hook()
   {
     static CSaveLoadSystem theSaveLoadSystem;
     return &theSaveLoadSystem;
   }
   ```

2. **In `StreamIO\Singleton.cpp`**: Fix **static initialization order fiasco**.

   `CSingleton theSingleton` is currently a **file-scope static** (line 72). When a different
   translation unit's static `CGlobalsLoader` constructor runs first,
   `GetSingletonGlobal_Hook()` returns `&theSingleton` — a valid address (static storage is
   allocated at load time) — but the object's **vtable is still null** because its constructor
   has not yet executed. The null-pointer check in `GetSingleton<T>()` passes (the address is
   non-null), and the subsequent virtual call `pSingleton->Get(...)` dereferences the null
   vtable → **access violation (0xC0000005)**.

   **Fix**: Convert `theSingleton` to **Construct On First Use** (Meyers' Singleton) so it is
   guaranteed to be fully constructed before any code accesses it:

   ```cpp
   // BEFORE (StreamIO\Singleton.cpp line 72-73):
   CSingleton theSingleton;
   ISingleton * STDCALL GetSingletonGlobal_Hook() { return &theSingleton; }

   // AFTER:
   ISingleton * STDCALL GetSingletonGlobal_Hook()
   {
     static CSingleton theSingleton;
     return &theSingleton;
   }
   ```

   This guarantees `theSingleton` is constructed on the **first call** to
   `GetSingletonGlobal_Hook()`, regardless of which translation unit's static initializer
   runs first.

   **Remove `__REDUCED_SINGLETON__` path entirely**. After this change, `theSingleton` no
   longer exists at file scope, so the following code (line 75-77) **will not compile**:

   ```cpp
   #ifdef __REDUCED_SINGLETON__
   ISingleton *g_pGlobalSingleton = &theSingleton;  // ERROR: theSingleton not in scope
   #endif
   ```

   This path was designed for tool executables that only needed a subset of singletons. With
   static linking, the full `CSingleton` is always available — the reduced path is dead code.
   Delete the `#ifdef __REDUCED_SINGLETON__` blocks in both `StreamIO\Singleton.cpp` and
   `StreamIOLib\Singleton.cpp`, and remove the `__REDUCED_SINGLETON__` preprocessor define
   from any `.vcxproj` files that set it. `g_pGlobalSingleton` is instead assigned in
   `CGlobalsLoader` via `GetSingletonGlobal_Hook()` as shown in step 3.

   **`GetTempRawBuffer_Hook()` fragility warning**: This function accesses
   `tempbuffers[nIndex][0]` after calling `reserve()`. It depends on
   `static STempBufferAutomatic tempinitautomagic` having already run `resize(32)` on each
   vector. If any code ever *calls* this function pointer (not just stores it) during static
   init before `tempinitautomagic` constructs, the vectors will be empty and the dereference
   will be undefined behavior. Currently safe because `CGlobalsLoader` only stores the pointer
   without calling it, but this invariant is undocumented and fragile.

   **Defensive fix**: Move the buffer initialization into the function itself:

   ```cpp
   void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex)
   {
     static bool initialized = []() {
       for (int i = 0; i < 10; ++i) tempbuffers[i].resize(32);
       return true;
     }();
     NI_ASSERT_SLOW_TF(nIndex < 10, "Can use only 10 temp buffers", return 0);
     tempbuffers[nIndex].reserve(nSize);
     return &(tempbuffers[nIndex][0]);
   }
   ```

   Then remove `STempBufferAutomatic` and its static instance.

3. **Replace all `GlobalsLoader.cpp` files** (in AILogic, Anim, GFX, Image, Input, Net, Scene, SFX,
   UI, GameTT, Game, editor, and tool projects) with a simplified version that calls the hook
   functions **directly** (no DLL loading) and **does NOT call `GetSingleton<T>()` during static
   initialization**:

   ```cpp
   #include "StdAfx.h"

   ISaveLoadSystem * STDCALL GetSLS_Hook();
   ISingleton * STDCALL GetSingletonGlobal_Hook();
   void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex);
   using GETTEMPRAWBUFFER_HOOK = void* (STDCALL *)(int nAmount, int nBufferIndex);

   IRandomGen *g_pGlobalRandomGen = nullptr;
   ISaveLoadSystem *g_pGlobalSaveLoadSystem = nullptr;
   ISingleton *g_pGlobalSingleton = nullptr;
   GETTEMPRAWBUFFER_HOOK g_pfnGlobalGetTempRawBuffer = nullptr;

   struct CGlobalsLoader
   {
     CGlobalsLoader()
     {
       g_pGlobalSaveLoadSystem = GetSLS_Hook();
       g_pGlobalSingleton = GetSingletonGlobal_Hook();
       g_pfnGlobalGetTempRawBuffer = GetTempRawBuffer_Hook;
       // NOTE: Do NOT call GetSingleton<IRandomGen>() here.
       // g_pGlobalRandomGen is initialized in NMain::Initialize() instead.
     }
   };
   static CGlobalsLoader theGlobalsLoader;
   ```

   **Why `GetSingleton<IRandomGen>` must be removed from `CGlobalsLoader`**: Even with the
   Construct On First Use fix for `theSingleton` (step 2 above), calling `GetSingleton<T>()`
   during static initialization is fragile — it depends on the singleton's internal registry
   being fully populated, which in turn depends on the `CSingleton` constructor having finished
   registering `IRandomGen`, `IGlobalVars`, `IConsoleBuffer`, etc. Mixing virtual calls with
   static initialization is inherently unsafe. The `g_pGlobalRandomGen` global must instead be
   set during **explicit initialization** (see Phase 6.5 below).

   **All 20 `GlobalsLoader.cpp` files** currently call `GetSingleton<IRandomGen>()` inside
   their static `CGlobalsLoader` constructor:
   - `Game\GlobalsLoader.cpp` (simplified form, already updated but still has the call)
   - Old-style DLL-loading form (guarded by `_DONT_LOAD_SINGLETONS`): AILogic, Anim, GFX,
     Image, Input, Net, Scene, SFX, UI, GameTT, editor, ELK, FontGen, a7exportmodel,
     bzmconvertor, excelexporter, imagedefrag, MapEditor, StreamIO

   **All** must have the `GetSingleton<IRandomGen>` call removed.

   **Important**: With static libs, only **one** translation unit can define each global. The
   globals (`g_pGlobalSaveLoadSystem`, etc.) must be defined in exactly one place — likely
   `StreamIO\Singleton.cpp` and `StreamIO\SaveLoadSystem.cpp`. All other `GlobalsLoader.cpp` files
   that define these same globals will cause **linker duplicate-symbol errors** and must be removed
   or reduced to `extern` declarations only.

   **Recommended approach**: Keep globals defined in the StreamIO lib, declare them `extern` in
   headers (`StreamIO\StructureSaver.h` already has `extern ISaveLoadSystem *g_pGlobalSaveLoadSystem`),
   and remove `GlobalsLoader.cpp` from all other projects. Only the **executable** project
   (`Game`, `editor`) needs a single `GlobalsLoader.cpp` that performs the one-time initialization.

   **Global variable ownership** — each global must be defined in exactly one TU:

   | Global Variable                | Defined In (owner TU)               | Declared `extern` In            | Assigned By                       |
   |--------------------------------|--------------------------------------|---------------------------------|-----------------------------------|
   | `g_pGlobalSaveLoadSystem`      | `StreamIO\SaveLoadSystem.cpp`       | `StreamIO\StructureSaver.h`     | `CSaveLoadSystem` constructor (via Meyers' singleton) |
   | `g_pGlobalSingleton`           | Executable's `GlobalsLoader.cpp`     | `StreamIO\Globals.h`            | `CGlobalsLoader` ctor → `GetSingletonGlobal_Hook()` |
   | `g_pfnGlobalGetTempRawBuffer`  | Executable's `GlobalsLoader.cpp`     | `StreamIO\Globals.h`            | `CGlobalsLoader` ctor → `GetTempRawBuffer_Hook` |
   | `g_pGlobalRandomGen`           | Executable's `GlobalsLoader.cpp`     | `StreamIO\RandomGen.h`          | `NMain::Initialize()` → `GetSingleton<IRandomGen>()` |

   Any `GlobalsLoader.cpp` in a library project that defines these same symbols **must be
   removed** or the linker will emit duplicate-symbol errors.

4. **Remove preprocessor defines** from `.vcxproj` files:
   - Remove `_STREAMIO_DLL` from `StreamIO.vcxproj`
   - Remove `_DONT_LOAD_STREAMIO` from `StreamIO.vcxproj`
   - Remove `_DONT_LOAD_SINGLETONS` from `Image.vcxproj` and any others

5. **Initialize `g_pGlobalRandomGen` in `NMain::Initialize()`** (or equivalent explicit init
   function) instead of during static construction:

   In `Main\Initialization.cpp`, near the top of `NMain::Initialize()` (after factory
   registration, before any code that calls `Random()`):

   ```cpp
   // Initialize cached global random generator pointer
   g_pGlobalRandomGen = GetSingleton<IRandomGen>();
   ```

   This is safe because `NMain::Initialize()` runs long after all static constructors have
   completed and the singleton registry is fully populated.

   **Also update `RandomGen.h`** — the `Random()` inline function currently accesses
   `g_pGlobalRandomGen` without a null check. Add a guard or assert:

   ```cpp
   inline unsigned int Random()
   {
     NI_ASSERT(g_pGlobalRandomGen != nullptr, "Random() called before g_pGlobalRandomGen initialized");
     // ... rest unchanged
     return g_pGlobalRandomGen->Get();
   }
   ```

   For **tool executables** (FontGen, ELK, a7exportmodel, etc.) that may not call
   `NMain::Initialize()`, add a similar explicit init call in their `main()` or `WinMain()`
   entry point.

6. **Add null-guard asserts to inline helper functions** that blindly dereference globals.

   `StreamIO\Globals.h` — `RegisterSingleton()`, `UnRegisterSingleton()`, and
   `GetSingletonGlobal()` all dereference `g_pGlobalSingleton` without checking for null.
   `StreamIO\StructureSaver.h` — `GetSLS()`, `CreateStructureSaver()`, `AddFactory()`, and
   `GetCommonFactory()` all dereference `g_pGlobalSaveLoadSystem` without checking.

   After migration, a single misplaced call before `CGlobalsLoader` runs will crash silently
   with no diagnostic. Add asserts:

   ```cpp
   // StreamIO\Globals.h
   inline ISingleton *GetSingletonGlobal()
   {
     NI_ASSERT(g_pGlobalSingleton != nullptr, "GetSingletonGlobal() called before globals initialized");
     return g_pGlobalSingleton;
   }

   // StreamIO\StructureSaver.h
   inline ISaveLoadSystem *GetSLS()
   {
     NI_ASSERT(g_pGlobalSaveLoadSystem != nullptr, "GetSLS() called before globals initialized");
     return g_pGlobalSaveLoadSystem;
   }
   ```

**Risk**: HIGH — this is the most complex phase due to duplicate symbol resolution across 20
translation units and the initialization order change. **Must be done carefully with iterative
builds.**

---

### Phase 7: Handle Special Cases ✅ DONE

#### 7.1 Scene DllMain — `hDLLInstance` ✅ DONE

`Scene\SceneInternal.cpp` uses `DllMain` to capture the DLL's `HINSTANCE`, which is used in
`Scene\Cursor.cpp` to load cursor resources via `LoadCursor(hDLLInstance, ...)`.

**Fix**: When Scene is a static lib linked into Game.exe, the cursor resources must be embedded in
the executable's resources instead. Change `hDLLInstance` to use `GetModuleHandle(nullptr)` (the
EXE's HINSTANCE) or pass it in during `ICursor::Init()`. Remove the `DllMain` function entirely.

#### 7.2 GFX Module Checker — `CGFXModuleChecker` ✅ DONE

`CGFXModuleChecker::SetModuleFunctionalityLimits()` sets global variables based on video memory.
It was called by `CModuleLoadAutoMagic` during DLL loading. After removing that mechanism, call it
explicitly during GFX initialization in `NMain::Initialize()`:

```cpp
// After GFX factory is registered:
CGFXModuleChecker checker;
checker.SetModuleFunctionalityLimits();
```

Or integrate the logic directly into `IGFX::Init()`.

#### 7.3 BugSlay DLL

`BugSlay\BugSlayer.cpp` has its own `DllMain`. BugSlay is already referenced as a project by
Game.exe. Verify whether it needs to remain a DLL (it provides crash-handling functionality that
may need DLL semantics) or can also be converted.

**Recommendation**: Keep BugSlay as-is for now (it has no `GetModuleDescriptor` and is not part of
the module system).

#### 7.4 Destruction Order with Meyers' Singletons ✅ DONE

Converting `theSingleton` and `theSaveLoadSystem` to function-local statics reverses their
destruction order — they are destroyed in reverse order of construction at `atexit` time. If
`NMain::Finalize()` calls `GetSingletonGlobal()->Done()` to clear the registry, and then the
`CSingleton` destructor runs and destroys the `unordered_map` containing `CPtr<>` ref-counted
objects, those objects may attempt to access already-destroyed subsystems during their own
release.

**Invariant**: `Done()` must be called **before** static destruction begins (i.e., in
`NMain::Finalize()` while the program is still in `main()`). After `Done()`, the
`CSingleton::objects` map should be empty, and the destructor becomes a no-op. Add an assert
in `CSingleton`'s destructor to verify this:

```cpp
~CSingleton()
{
  NI_ASSERT(objects.empty(), "CSingleton destroyed with registered objects — call Done() first");
}
```

#### 7.5 `Random()` Debug Logging Teardown Risk ✅ DONE

In non-release builds, `RandomGen.h`'s `Random()` function calls
`GetSingleton<IConsoleBuffer>()->WriteASCII(...)` for debug logging when `lograndom` is set.
If `IConsoleBuffer` is unregistered (via `Done()`) before the last `Random()` call during
shutdown, `GetSingleton<IConsoleBuffer>()` returns `nullptr` and the `->WriteASCII()` call
crashes.

**Fix**: Guard the debug logging path:

```cpp
if (GetGlobalVar("lograndom", 0) != 0)
{
  if (IConsoleBuffer *pCB = GetSingleton<IConsoleBuffer>())
    pCB->WriteASCII(10, NStr::Format("r"), 0, true);
}
```

Apply this to all four `Random()` overloads in `RandomGen.h`.

---

### Phase 8: Remove Dead Code ✅ DONE

> **Implementation completed:**
> - 8.1 ✅ All 11 module `.def` files deleted + 2 orphan `.def` files (`AIPath.def`, `FileIO.def`) deleted
> - 8.2 ✅ `GetModuleDescriptor()`/`theModuleDescriptor` removed from all `*ObjectFactory.cpp`
> - 8.3 ✅ `SModuleDescriptor`/`GETMODULEDESCRIPTOR` removed from `Misc\Basic.h`; `IModuleChecker` retained (used by `CGFXModuleChecker`)
> - 8.3 ⚠️ 3 tool executables in `SCOTools.sln` (`ELK`, `FontGen`, `imagedefrag`) still reference removed types — **known issue, tools need separate migration**
> - 8.4 ✅ All 19 old library `GlobalsLoader.cpp` files deleted from disk; only `Game` and `editor` copies remain
> - 8.5 ✅ `StreamIOLib\` directory deleted entirely
> - 8.6 ✅ `__REDUCED_SINGLETON__` removed from `StreamIO\Singleton.cpp`

After all modules compile and link as static libs:

1. **Delete `.def` files**:
   - `AILogic\AILogic.def`
   - `Anim\Animation.def`
   - `GFX\GFX.def`
   - `Image\Image.def`
   - `Input\Input.def`
   - `Net\net.def`
   - `Scene\Scene.def`
   - `SFX\Sound.def`
   - `StreamIO\StreamIO.def`
   - `UI\UI.def`
   - `GameTT\GameTT.def`

2. **Remove `GetModuleDescriptor()` and `theModuleDescriptor`** from each `*ObjectFactory.cpp`:
   - `AILogic\AILogicObjectFactory.cpp`
   - `Anim\AnimObjectFactory.cpp`
   - `GFX\GFXObjectFactory.cpp`
   - `Image\ImageObjectFactory.cpp`
   - `Input\InputObjectFactory.cpp`
   - `Net\NetObjectFactory.cpp`
   - `Scene\SceneObjectFactory.cpp`
   - `SFX\SoundObjectFactory.cpp`
   - `StreamIO\StreamIOObjectFactory.cpp`
   - `UI\UIObjectFactory.cpp`
   - `GameTT\MissionObjectFactory.cpp`

3. **Remove from `Misc\Basic.h`**:
   - `interface IModuleChecker` (after migrating GFX checker logic)
   - `struct SModuleDescriptor`
   - `using GETMODULEDESCRIPTOR` typedef

4. **Remove `GlobalsLoader.cpp`** from all library projects (keep only in executable projects).

5. **Delete `StreamIOLib\` directory entirely**. It is a stale duplicate of `StreamIO\` — it
   uses outdated C++ (`std::hash_map`, C-style casts, no `override`) and has the same unfixed
   file-scope `CSingleton theSingleton` bug. With StreamIO now a static lib, `StreamIOLib` is
   redundant. If any tool project references `StreamIOLib`, update it to reference `StreamIO`
   instead.

6. **Remove `__REDUCED_SINGLETON__` preprocessor define** from all `.vcxproj` files and delete
   all `#ifdef __REDUCED_SINGLETON__` blocks in `StreamIO\Singleton.cpp`. This path is dead
   code after the Meyers' singleton change (see Phase 6 step 2).

**Risk**: LOW — straightforward deletion after successful build.

---

### Phase 9: Clean Up `iMain.h` and Headers ✅ DONE

- Remove `NMain::GetModuleDesc()` declaration
- Remove `NMain::LoadAllModules()` declaration
- Remove `NMain::UnloadAllModules()` declaration (or keep as simple `GetSingletonGlobal()->Done()`)
- Remove `NMain::GetFirstModuleDesc()`, `NMain::GetNextModuleDesc()`, `NMain::GetModuleFileNameByDesc()`
- Add declarations for new factory accessors if they're declared in module headers

---

## 5. Recommended Execution Order

| Step | Phase | Scope | Status | Risk |
|------|-------|-------|--------|------|
| 1 | Phase 3 | Add factory accessor functions | ✅ Done | LOW |
| 2 | Phase 1 | Convert vcxproj to StaticLibrary | ✅ Done | LOW |
| 3 | Phase 2 | Add project references to exe | ✅ Done | MED |
| 4 | Phase 6 | Consolidate GlobalsLoader.cpp + fix static init order | ✅ Done | HIGH |
| 5 | Phase 4 | Rewrite Initialization.cpp | ✅ Done | LOW |
| 6 | Phase 5 | Remove LoadDLLs.cpp DLL code | ✅ Done | MED |
| 7 | Phase 7 | Handle destruction order (7.4), Random() teardown (7.5) | ✅ Done | MED |
| 8 | Phase 8 | Delete StreamIOLib + `__REDUCED_SINGLETON__` + orphan files | ✅ Done | LOW |
| 9 | Phase 9 | Clean up headers | ✅ Done | LOW |

---

## 6. Verification Checklist

General (after each phase):
- [x] No `.dll` files are produced for converted modules
- [x] No linker errors for duplicate symbols (in A7.sln)
- [x] No linker errors for unresolved externals (in A7.sln)
- [x] Cursor rendering works (Scene DllMain fix — `GetModuleHandle(nullptr)`)
- [x] GFX video-memory limits are applied (`CGFXModuleChecker` called explicitly in `NMain::Initialize()`)
- [x] All configurations build successfully (Debug, FastDebug, Release, BetaRelease, Profiler)
- [ ] Game.exe launches and reaches main menu
- [ ] Editor.exe launches (if applicable)
- [ ] Sound initializes correctly
- [ ] AI logic initializes (unit creation works)

Remaining items (Phase 6 / 7 / 8):
- [x] `CSingleton theSingleton` uses Construct On First Use (function-local static in `GetSingletonGlobal_Hook()`)
- [x] `CSaveLoadSystem theSaveLoadSystem` uses Construct On First Use (function-local static in `GetSLS_Hook()`)
- [x] `GetTempRawBuffer_Hook()` self-initializes `tempbuffers` (no dependency on `STempBufferAutomatic`)
- [x] No `GetSingleton<>()` calls remain in any static constructor (`CGlobalsLoader` or similar)
- [x] `g_pGlobalRandomGen` is initialized in `NMain::Initialize()` (and tool entry points), not during static init
- [x] `GetSingletonGlobal()` and `GetSLS()` have `NI_ASSERT` null guards
- [x] `CSingleton` destructor asserts `objects.empty()` (verifies `Done()` was called)
- [x] `Random()` debug logging guards `GetSingleton<IConsoleBuffer>()` against null (teardown safety)
- [x] `NMain::Finalize()` calls `GetSingletonGlobal()->Done()` **before** returning to `main()` exit
- [x] No `#ifdef __REDUCED_SINGLETON__` blocks remain anywhere in the codebase
- [x] `StreamIOLib\` directory deleted; no tool project references it
- [x] Only one definition of each global variable across the entire link (GlobalsLoader.cpp removed from lib projects)
- [ ] `grep -rn "GetModuleDesc\|SModuleDescriptor\|GetModuleDescriptor\|LoadAllModules" src/` returns zero hits — **⚠️ 3 tool files in SCOTools.sln still reference these (ELK, FontGen, imagedefrag) — known, needs separate tool migration**
- [x] No `.def` files remain (except BugSlay if kept)
- [ ] Binary size comparison: exe should be larger (contains all code), no DLLs alongside it
