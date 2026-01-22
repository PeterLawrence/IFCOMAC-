# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Common Commands

### Build

- Debug build (x64):

  ```
  msbuild IfcSpacialImport.vcxproj /p:Configuration=Debug /p:Platform=x64
  ```

- Release build (x64):

  ```
  msbuild IfcSpacialImport.vcxproj /p:Configuration=Release /p:Platform=x64
  ```

- Using Visual Studio command line:

  ```
  devenv IfcSpacialImport.vcxproj /Build Debug
  ```

### Clean

```
msbuild IfcSpacialImport.vcxproj /t:Clean /p:Configuration=Debug /p:Platform=x64
```

### Run / Test

The project builds a DLL (`IfcSpacialImport.dll`). There are no automated tests in this repository. For manual testing, load the DLL in an application that calls the exported function `FullIfcImporter`. Example usage in C++:

```cpp
#include <windows.h>
#include "IfcBuildingModel.h" // from IfcOpenShell

bool (*FullIfcImporter)(const char*, IFCBuildingModel&) =
    (bool(*)(const char*, IFCBuildingModel&))GetProcAddress(
        LoadLibrary(L"IfcSpacialImport.dll"),
        "FullIfcImporter");

IFCBuildingModel model;
FullIfcImporter("path\\to\\file.ifc", model);
```

### Lint / Code Quality

No lint configuration is included in this repository. Use a tool of your choice or rely on Visual Studio's diagnostics.

## Project Structure & Architecture

- **Project Type**: Visual Studio C++ DLL (`IfcSpacialImport.vcxproj`).
- **Target Platform**: Windows x64.
- **Core Functionality**: Provides the exported function `FullIfcImporter`, which loads an IFC file using the IfcOpenShell library (`ifcparse`, `ifcgeom`) and populates an `IFCBuildingModel` object via `ExodusIFCGeomUtils`.
- **Key Source Files**
  - `dllmain.cpp`: Defines the DLL entry point and the `FullIfcImporter` function.
  - `exodusifcutils.cpp/h`: Utilities for extracting geometry and properties.
  - `exodusIFCClasses.cpp/h`: Definitions of helper classes used by `exodusifcutils`.
  - `extractIFCData.cpp`: Additional extraction routines.
  - `getpsets.cpp/h`: Handles property set extraction.
  - `extractgeom.h`: Header for geometry extraction.
- **Dependencies**
  - **IfcOpenShell** (header and source included via relative paths, e.g., `../src/...`). Ensure the IfcOpenShell source is present in the repository.
  - **vld.h** (optional Visual Leak Detector) – enabled by defining `USE_VLD`.
- **Build Output**: DLL and associated PDB files located in `x64\Debug` or `x64\Release`.
- **Testing**: None; manual tests are expected.

## Using the DLL in Other Projects

1. **C/C++**: Use `LoadLibrary` and `GetProcAddress` to obtain the `FullIfcImporter` function as shown above.
2. **Python**: Load the DLL with `ctypes` or `cffi`. Example with `ctypes`:

   ```python
   import ctypes
   lib = ctypes.CDLL(r"path\IfcSpacialImport.dll")
   # Define argument and return types, then call lib.FullIfcImporter(...)
   ```

3. **C#**: Use `DllImport` attribute.

## File Conventions

- All source files use `#pragma once` for headers.
- Windows headers are included with `WIN32_LEAN_AND_MEAN` to reduce build time.
- Build warnings are suppressed for certain identifiers to avoid noise.

## Additional Notes

- The repository contains a minimal README that only declares the project name.
- No CI configuration is present; consider adding GitHub Actions for builds if desired.
- No `.clang-format` or `.editorconfig` files are provided; style is consistent with standard Visual Studio C++ conventions.
