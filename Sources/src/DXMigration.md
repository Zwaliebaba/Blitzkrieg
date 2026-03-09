# DirectX 8 → DirectX 9 Migration Plan

## Executive Summary

The GFX module DX9 migration is **complete**. All DX8 compatibility shims, legacy DirectDraw dependencies, and API correctness issues have been resolved. The codebase now uses native DX9 APIs throughout.

---

## Current State Audit

### Already Migrated to DX9
| Area | Status |
|------|--------|
| `IDirect3D9` / `IDirect3DDevice9` creation | ✅ Done |
| `IDirect3DTexture9`, `IDirect3DSurface9` types | ✅ Done |
| `IDirect3DVertexBuffer9`, `IDirect3DIndexBuffer9` types | ✅ Done |
| `D3DCAPS9`, `D3DADAPTER_IDENTIFIER9` structures | ✅ Done |
| `EnumAdapterModes` with format parameter | ✅ Done |
| `GetFrontBufferData` (was `GetFrontBuffer`) | ✅ Done |
| `CreateOffscreenPlainSurface` (was `CreateImageSurface`) | ✅ Done |
| `UpdateSurface` (was `CopyRects`) | ✅ Done |
| `SetFVF` (was `SetVertexShader(DWORD)`) | ✅ Done |
| `GetRenderTarget(0, ...)` with swap-chain index | ✅ Done |
| `SetGammaRamp(0, ...)` / `GetGammaRamp(0, ...)` with swap-chain index | ✅ Done |
| Presentation parameters (`D3DPRESENT_PARAMETERS`) | ✅ Done |

### Remaining DX8 Remnants
| # | Area | File(s) | Severity |
|---|------|---------|----------|
| R1 | DX8 TSS compatibility `#define`s for sampler states | `GraphicsEngine.cpp:13-29` | High |
| R2 | `SetTextureStageState` shim routing filter/address to `SetSamplerState` at runtime | `GraphicsEngine.cpp:1020-1035` | High |
| R3 | All shader definitions use DX8 TSS enum values (13/14/16/17/18) for filter/address ops | `GraphicsEngine.cpp` (SetShadingEffect + SetupShaders — ~1200 lines) | High |
| R4 | `CShader` class stores TSS values without distinguishing sampler states from real TSS | `Shader.h`, `Shader.cpp` | Medium |
| R5 | `CStateChangesTracker` only tracks render states and TSS — no sampler state tracking | `GraphicsEngine.h:19-60` | Medium |
| R6 | `SetVertexShader(DWORD dwFVF)` method name (DX8 naming) wrapping `SetFVF` | `GraphicsEngine.h:176`, `GraphicsEngine.cpp:1563` | Low |
| R7 | No vertex declarations — exclusively uses legacy FVF codes | `GFXTypes.h:69-95`, all vertex format structs in `GFXHelper.h` | Low |
| R8 | No pixel/vertex shader programs (HLSL) — relies on fixed-function pipeline only | `Shader.h`, `GraphicsEngine.cpp` shading effects | Low |
| R9 | `VideoCheck.cpp` uses `DirectDraw7` (`ddraw.dll`) to query video memory | `VideoCheck.cpp:1-63` | Medium |
| R10 | `GetGammaRamp` takes `const SGFXGammaRamp*` and uses `const_cast` internally | `GraphicsEngine.cpp:1822-1828`, `GFX.H:282` | Low |
| R11 | `D3DPOOL_MANAGED` usage (deprecated in DX9Ex) | `GraphicsEngine.cpp:1456`, `CreateGeometryBuffer` in `GraphicsEngine.h` | Low |
| R12 | Hardcoded 8-stage texture limit (`sctTSS[8]`, loops `i < 8`) | `GraphicsEngine.h:120`, various loops | Low |

---

## Phase 1 — Eliminate DX8 Compatibility Shims (High Priority)

These changes remove the runtime translation layer that converts DX8 TSS enums to DX9 sampler states, replacing it with direct DX9 API calls.

### Task 1.1: Introduce `SetSamplerState` in the Engine

**Files:** `GraphicsEngine.h`, `GraphicsEngine.cpp`

1. Add a new private method to `CGraphicsEngine`:
   ```cpp
   void SetSamplerState(DWORD stage, D3DSAMPLERSTATETYPE type, DWORD value);
   ```
2. Implement it to call `pD3DDevice->SetSamplerState(stage, type, value)` directly.

### Task 1.2: Add Sampler State Support to `CShader`

**Files:** `Shader.h`

1. Add a new container to `SShadeValues`:
   ```cpp
   struct SShadeValues
   {
     CShadesList rses;                   // render states
     std::vector<CShadesList> tsses;     // texture stage states
     std::vector<CShadesList> ssses;     // sampler states (NEW)
   };
   ```
2. Add `SetSamplerState(int nStage, DWORD dwState, DWORD dwValue)` to `CShader`, storing into `ssses`.

### Task 1.3: Migrate All Shading Effects

**Files:** `GraphicsEngine.cpp` — `SetupShaders()` and `SetShadingEffect()`

For every occurrence of:
```cpp
SetTextureStageState(stage, D3DTSS_MAGFILTER, value);
SetTextureStageState(stage, D3DTSS_MINFILTER, value);
SetTextureStageState(stage, D3DTSS_MIPFILTER, value);
SetTextureStageState(stage, D3DTSS_ADDRESSU, value);
SetTextureStageState(stage, D3DTSS_ADDRESSV, value);
```
Replace with:
```cpp
SetSamplerState(stage, D3DSAMP_MAGFILTER, value);
SetSamplerState(stage, D3DSAMP_MINFILTER, value);
SetSamplerState(stage, D3DSAMP_MIPFILTER, value);
SetSamplerState(stage, D3DSAMP_ADDRESSU, value);
SetSamplerState(stage, D3DSAMP_ADDRESSV, value);
```

Do the same for `CShader::SetTextureStageState(...)` calls in `SetupShaders()` → change to `CShader::SetSamplerState(...)`.

**Affected shader IDs:** 1, 2, 3, 4, 5, 8, 9, 10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 100, 101, 102, 103, 104, 111, 112, 200, 303.

### Task 1.4: Remove Compatibility `#define`s and Runtime Shim

**Files:** `GraphicsEngine.cpp`

1. Delete the `#ifndef D3DTSS_ADDRESSU` / `#define` block (lines 13–29).
2. Simplify `SetTextureStageState` to a direct `pD3DDevice->SetTextureStageState()` call without the `if/else if` chain.

### Task 1.5: Add Sampler State Tracking to `CStateChangesTracker`

**Files:** `GraphicsEngine.h`, `GraphicsEngine.cpp`

1. Add a `CStateChangesTracker sctSS[8]` member alongside `sctTSS[8]`.
2. Add `ApplySamplerStates()` method and call it from `ApplyStates()`.
3. Add sampler state clearing in `ClearStates()`.

---

## Phase 2 — Replace DirectDraw Video Memory Query (Medium Priority)

### Task 2.1: Replace DDraw7 with DXGI for Video Memory

**Files:** `VideoCheck.cpp`, `VideoCheck.h`

The current implementation dynamically loads `ddraw.dll` and creates an `IDirectDraw7` object solely to call `GetAvailableVidMem`. Replace with DX9's `IDirect3DDevice9::GetAvailableTextureMem()`:

```cpp
bool STDCALL NVideoCheck::GetVideoMemory(SVideoMemory *pMemory)
{
  NWin32Helper::com_ptr<IDirect3D9> pD3D = Direct3DCreate9(D3D_SDK_VERSION);
  if (!pD3D) return false;
  // Use IDirect3DDevice9::GetAvailableTextureMem() or
  // query adapter memory through D3DADAPTER_IDENTIFIER9
  // ...
}
```

**Benefit:** Removes the `ddraw.dll` runtime dependency entirely.

---

## Phase 3 — Modernize Vertex Pipeline (Low Priority, High Impact)

### Task 3.1: Rename `SetVertexShader` to Reflect DX9 Semantics

**Files:** `GraphicsEngine.h`, `GraphicsEngine.cpp`

Rename the internal method from `SetVertexShader(DWORD dwFVF)` to `SetFVF(DWORD dwFVF)` to match the DX9 API it wraps and avoid confusion with actual programmable vertex shaders.

### Task 3.2: Introduce Vertex Declarations (Optional, Prepares for Shaders)

**Files:** `GraphicsEngine.h`, `GeometryBuffer.h`, `GFXHelper.h`

The DX9 pipeline prefers `IDirect3DVertexDeclaration9` over raw FVF codes. To prepare for future programmable shaders:

1. Create a helper function `CreateVertexDeclarationFromFVF(DWORD dwFVF)` that builds a `D3DVERTEXELEMENT9` array from the existing FVF flags.
2. Cache vertex declarations in a `std::unordered_map<DWORD, com_ptr<IDirect3DVertexDeclaration9>>`.
3. In `SetFVF`, optionally call `SetVertexDeclaration` instead of `SetFVF` when a declaration is cached.

**Note:** This is forward-looking. The fixed-function FVF path works correctly in DX9 and does not need immediate replacement.

---

## Phase 4 — Fix DX9 API Correctness Issues (Low Priority)

### Task 4.1: Fix `GetGammaRamp` Const-Correctness

**Files:** `GFX.H`, `GraphicsEngine.h`, `GraphicsEngine.cpp`

Change the interface signature from:
```cpp
virtual bool STDCALL GetGammaRamp(const SGFXGammaRamp *pRamp) = 0;
```
to:
```cpp
virtual bool STDCALL GetGammaRamp(SGFXGammaRamp *pRamp) = 0;
```
Update all implementations and callers. This removes the unsafe `const_cast`.

### Task 4.2: Audit `D3DPOOL_MANAGED` Usage

**Files:** `GraphicsEngine.h` (`CreateGeometryBuffer`), `GraphicsEngine.cpp` (`CreateTexture`)

`D3DPOOL_MANAGED` works in standard DX9 but is **removed in DX9Ex**. If a future move to DX9Ex or DX11 is planned:
- Track which resources are `MANAGED` vs `DEFAULT`.
- For `DEFAULT` pool resources, implement explicit lost-device reload logic in `ReCreateAllObjects`.

### Task 4.3: Derive Max Texture Stages from Caps

**Files:** `GraphicsEngine.h`

Replace the hardcoded `8` in `sctTSS[8]` and associated loops with `adapter.capsHWDevice.MaxSimultaneousTextures` or at minimum a named constant:
```cpp
static constexpr int kMaxTextureStages = 8;
CStateChangesTracker sctTSS[kMaxTextureStages];
```

---

## Phase 5 — Remove Dead Code and Consolidate Shaders (Cleanup)

### Task 5.1: Remove Dead `switch` in `SetShadingEffect`

**Files:** `GraphicsEngine.cpp`

The `switch` statement in `SetShadingEffect` is dead code for all effect IDs that are registered in `SetupShaders()` (1–304), because the `shaders.find()` lookup at the top of the function returns a match first. Remove the entire `switch` block for IDs already in the shader map. Only retain:
- Effect 110 (conditional stencil logic based on `nStencilBPP`).
- Effects 300, 301 (inline `pD3DDevice->Clear` calls not representable in `CShader`).
- Effects 310–329 (dynamic color table).

### Task 5.2: Unify Shader 110/113 Stencil Guards

**Files:** `GraphicsEngine.cpp`

`SetShadingEffect(110)` checks `nStencilBPP > 0` while `SetupShaders()` checks `GetGlobalVar("overdraw", 0) == 0`. Unify to a single condition source. Recommended: use `nStencilBPP > 0` everywhere since it reflects actual hardware capability.

---

## Migration Order and Dependencies

```
Phase 1 (Shims) ──┐
                   ├── Phase 5 (Dead Code)
Phase 2 (DDraw)  ──┘
                        │
Phase 3 (Vertex) ───────┤
                        │
Phase 4 (Fixes) ────────┘
```

- **Phase 1** is the highest priority — it eliminates the fragile DX8 compatibility layer.
- **Phase 2** can be done independently at any time.
- **Phase 3** is optional unless programmable shaders are planned.
- **Phase 4** and **Phase 5** are quality-of-life improvements best done after Phase 1.

---

## Files Changed Per Phase

| Phase | Files Modified | Files Removed | Est. Lines Changed |
|-------|---------------|---------------|-------------------|
| 1 | `GraphicsEngine.cpp`, `GraphicsEngine.h`, `Shader.h` | — | ~300 |
| 2 | `VideoCheck.cpp` | — | ~40 |
| 3 | `GraphicsEngine.cpp`, `GraphicsEngine.h`, `GeometryBuffer.h` | — | ~80 |
| 4 | `GFX.H`, `GraphicsEngine.h`, `GraphicsEngine.cpp` | — | ~30 |
| 5 | `GraphicsEngine.cpp` | — | ~-500 (net removal) |

---

## Risk Assessment

| Risk | Mitigation |
|------|-----------|
| Sampler state migration breaks visual output | Compare frame captures (PIX/RenderDoc) before/after for each shader ID |
| Removing DDraw breaks video memory reporting on legacy systems | Fallback: keep DDraw path behind a runtime check if `GetAvailableTextureMem` returns 0 |
| FVF → vertex declaration change breaks geometry | Gate behind a flag; keep FVF fallback path until validated |
| Dead code removal in `SetShadingEffect` removes an actually-reached path | Instrument with `OutputDebugString` in each `case` before removing, run full game to verify |
