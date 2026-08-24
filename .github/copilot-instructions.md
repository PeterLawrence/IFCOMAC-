# Copilot Instructions for IfcSpacialImport

This guide helps Copilot work effectively in the IfcSpacialImport repository.

## Build, Test, and Lint

### Build Commands

**Debug build (x64):**
```powershell
msbuild IfcSpacialImport.vcxproj /p:Configuration=Debug /p:Platform=x64
```

**Release build (x64):**
```powershell
msbuild IfcSpacialImport.vcxproj /p:Configuration=Release /p:Platform=x64
```

**Using Visual Studio (requires VS installed):**
```powershell
devenv IfcSpacialImport.vcxproj /Build Debug
```

### Clean Build

```powershell
msbuild IfcSpacialImport.vcxproj /t:Clean /p:Configuration=Debug /p:Platform=x64
```

### Testing

There are no automated unit tests. This project builds a DLL that must be tested by loading it in an external application. To test manually:

1. Build the project (outputs to `x64\Debug\IfcSpacialImport.dll` or `x64\Release\`)
2. Load the DLL in an application that imports `FullIfcImporter`
3. Call `FullIfcImporter(filename, model)` with an IFC file path

See [Using the DLL](#using-the-dll-in-other-projects) for integration examples.

### Lint / Code Quality

No linting tools are configured. Rely on Visual Studio's built-in diagnostics (Error List, Code Analysis).

## Architecture

### Core Purpose

IfcSpacialImport is a Windows x64 DLL that parses IFC (Industry Foundation Classes) building information models using IfcOpenShell and extracts geometric and semantic data into an `IFCBuildingModel` object.

### Execution Flow

1. **Entry point:** `FullIfcImporter()` in `dllmain.cpp`
   - Accepts IFC file path and reference to `IFCBuildingModel`
   - Returns `bool` (success/failure)

2. **Schema validation:** Loads IFC file via `IfcParse::IfcFile`, checks schema name (IFC 4.3 by default)

3. **Data extraction (two main phases):**
   - **Semantic data:** `ExodusIFCGeomUtils::EXODUSIFCExtract()` → extracts building structure, properties, property sets
   - **Geometry:** `ExodusIFCGeomUtils::ExtractGeom()` → converts geometric entities using IfcOpenShell's converter

4. **Post-processing:** `MyBuilding.UpdateAssociations()` → links related objects

### Key Components

#### Main Classes (IFC Model Data Structure)

- **`IFCBuildingModel`** – Top-level container for all building data
- **`IFCStoreyModel`** – Represents a building storey/floor with spaces and elements
- **`IFCSpaceModel`** – Represents a building space (room)
- **`IFCBaseData`** – Base class for Name, LongName, GUID, and other common attributes
- **`IFCPoint`** – 3D point representation with bounding box operations

#### Processing Modules

| File | Responsibility |
|------|-----------------|
| `dllmain.cpp` | DLL entry point, `FullIfcImporter()` export, orchestration |
| `exodusifcutils.cpp/h` | Core extraction logic for semantic data; calls IfcOpenShell converters |
| `extractgeom.cpp/h` | Geometry extraction; traverses IFC model and builds triangulated geometry |
| `extractIFCData.cpp` | Supplementary data extraction routines (may extract specific entity types) |
| `getpsets.cpp/h` | Property set (IfcPropertySet) extraction and organization |
| `ExodusIFCClasses.h` | Data model definitions for IFCStoreyModel, IFCSpaceModel, IFCPoint, etc. |

### Dependency Chain

```
IfcSpacialImport
  ├── IfcOpenShell (headers in ../src)
  │   ├── ifcparse (IFC file parsing)
  │   ├── ifcgeom (geometry conversion engine)
  │   └── geometry_mapping_* (IFC version-specific converters for IFC2x3, 4, 4.1, 4.2, 4.3, 4.3_tc1, 4.3_add1, 4.3_add2)
  │       ├── geometry_kernel_opencascade
  │       ├── geometry_kernel_cgal_simple
  │       └── geometry_kernel_cgal
  └── Core Dependencies
      ├── OCCT 7.7.1 (OpenCASCADE Technology)
      │   └── TK* libraries (TKernel, TKMath, TKBRep, TKGeomBase, TKGeomAlgo, TKG3d, TKG2d, TKShHealing, TKTopAlgo, TKMesh, TKPrim, TKBool, TKBO, TKFillet, TKXSBase, TKOffset, TKHLR, TKBin, TKIGES, TKSTEP*)
      ├── OpenCOLLADA (3D asset interchange)
      │   └── GeneratedSaxParser, MathMLSolver, OpenCOLLADABaseUtils, OpenCOLLADAFramework, OpenCOLLADASaxFrameworkLoader, OpenCOLLADAStreamWriter, UTF, buffer, ftoa, pcre
      ├── Boost 1.86.0 (C++ utilities, headers only in this project)
      ├── CGAL (computational geometry)
      ├── Eigen (linear algebra, header-only)
      └── MPIR/MPFR (arbitrary precision arithmetic)
```

**Include Paths & Library Locations:**
See `VisualStudioAdditionalDependencies.txt` for complete list of:
- Include directories (e.g., `\IfcOpenshell\src`, Boost, OCCT, Eigen)
- Library directories (IfcOpenShell build output, Boost stage, OCCT, OpenCOLLADA)
- Static library dependencies (IfcParse.lib, IfcGeom.lib, geometry_mapping_*.lib, TK*.lib, OpenCOLLADA libraries, MPIR.lib, MPFR.lib)

**Important:** IfcOpenShell source must be available at relative path `../src` from this project. Build outputs (IfcParse.lib, IfcGeom.lib) are expected at `\IfcOpenshell\_build-vs2022-x64\Release`.

## Key Conventions

### IFC Schema Version

- **Default schema:** IFC 4.3 (`ExoIFCSchema = EXOIfc4x3`)
- **Alternative:** IFC 4.3 Add1 (set `ExoIFCSchema = EXOIfc4x3_add1`)
- Configure in `exodusifcutils.h` macro definitions

### Warning Suppression

```cpp
#pragma warning(disable : 4018 4267 4250 4984 4985 4307)
```

These are suppressed because they come from IfcOpenShell headers and are not relevant to this project. Do not suppress other warnings without documenting why.

### Header Protection

All headers use `#pragma once` instead of include guards.

### Debug Output

- Use `OutputDebugStringA()` when `_DEBUG` is defined
- Use `std::cout` for console messages in Release builds
- Do not mix debug and release output mechanisms

### Optional: Visual Leak Detector

If `USE_VLD` is defined during compilation, `vld.h` will be included for memory leak detection in Debug builds. This is optional and should only be enabled when actively debugging memory issues.

### Coordinate System

- Geometry extraction uses world coordinates (`UseWorldCoords = true`)
- Element hierarchy is preserved during conversion (`UseElementHierarchy = true`)

## Using the DLL in Other Projects

### C/C++

```cpp
#include <windows.h>
#include "IfcBuildingModel.h" // from this project

// Dynamically load and call FullIfcImporter
typedef bool (*FullIfcImporterFunc)(const char*, IFCBuildingModel&);

HMODULE hModule = LoadLibrary(L"IfcSpacialImport.dll");
FullIfcImporterFunc importer = (FullIfcImporterFunc)GetProcAddress(hModule, "FullIfcImporter");

IFCBuildingModel model;
bool success = importer("path\\to\\file.ifc", model);

FreeLibrary(hModule);
```

### Python

```python
import ctypes

# Load DLL
lib = ctypes.CDLL(r"path\IfcSpacialImport.dll")

# Define function signature (adjust based on actual IFCBuildingModel binding)
lib.FullIfcImporter.argtypes = [ctypes.c_char_p, ctypes.c_void_p]
lib.FullIfcImporter.restype = ctypes.c_bool

# Call
success = lib.FullIfcImporter(b"path\\to\\file.ifc", model_ptr)
```

### C#

```csharp
[DllImport("IfcSpacialImport.dll", CallingConvention = CallingConvention.Cdecl)]
public static extern bool FullIfcImporter(string filename, ref IFCBuildingModel model);
```

## Platform & Tool Requirements

- **OS:** Windows x64 only
- **Toolset:** Visual Studio 2015+ (v141 toolset configured)
- **Platform Toolset:** v141 or higher
- **Target Platform Version:** 10.0.26100.0 (adjustable in .vcxproj)
- **Character Set:** Unicode

## Common Tasks

### Building with Visual Studio IDE

1. Open `IfcSpacialImport.vcxproj` in Visual Studio
2. Select configuration (Debug/Release) and platform (x64)
3. Build → Build IfcSpacialImport or (Ctrl+Shift+B)

### Rebuilding After Dependency Changes

If IfcOpenShell or external dependencies are updated:
1. Update include/library paths in `VisualStudioAdditionalDependencies.txt`
2. Update `.vcxproj` property sheets if paths changed
3. Run `msbuild /t:Clean` then rebuild

### Debugging

1. Build Debug configuration
2. Attach a debugger to an external application that calls `FullIfcImporter()`
3. Use Visual Studio's Debug → Attach to Process
4. Set breakpoints in source files

### Checking Build Output

- **Debug:** `x64\Debug\IfcSpacialImport.dll` + `IfcSpacialImport.pdb`
- **Release:** `x64\Release\IfcSpacialImport.dll`
- PDB files are essential for debugging; distribute with Release builds if needed

## MCP Servers

### GitHub MCP Server

The **GitHub MCP server** is recommended for this repository to enable:

- Searching and reading files from the repository
- Viewing pull requests and issues
- Checking commit history and branches
- Retrieving GitHub Actions logs (if CI is added)

This helps Copilot access repository context without manual file exploration.

## Additional Notes

- No CI/CD pipeline currently exists; consider adding GitHub Actions for automated builds on push
- No `.clang-format` or `.editorconfig`; follow Visual Studio's standard C++ style
- The project has no CONTRIBUTING.md guidelines; maintain consistency with existing code style (see CLAUDE.md for style notes)
