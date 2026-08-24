# Building IfcSpacialImport with CMake and Visual Studio 2017

This project includes a CMakeLists.txt file for building with CMake instead of the native Visual Studio project file.

## Prerequisites

- **CMake 3.20+** ([Download](https://cmake.org/download/))
- **Visual Studio 2017 or 2022** (with C++ development tools)
- **IfcOpenShell** built and dependencies installed (see CLAUDE.md)

## Quick Start

### 1. Configure the Build

```powershell
# From the IfcSpacialImport directory
mkdir build
cd build

# Generate Visual Studio 2017 project files (x64)
cmake .. -G "Visual Studio 15 2017" -A x64
```

### 2. Build

**Using CMake:**
```powershell
cmake --build . --config Debug
# or
cmake --build . --config Release
```

**Using Visual Studio:**
```powershell
devenv IfcSpacialImport.sln /Build Debug
```

### 3. Output

Built DLL will be in: `build\bin\Debug\IfcSpacialImport.dll` (Debug) or `build\bin\Release\IfcSpacialImport.dll` (Release)

## Configuration

If your IfcOpenShell paths differ from the defaults, edit CMakeLists.txt or pass options to cmake:

```powershell
cmake .. `
  -G "Visual Studio 15 2017" `
  -A x64 `
  -DIFCOPENSHELL_ROOT="C:\path\to\IfcOpenshell" `
  -DIFCSPACIALIMPORT_TOOLSET="vs2017"
```

### Available Options

| Option | Values | Default |
|--------|--------|---------|
| `IFCOPENSHELL_ROOT` | Path to IfcOpenShell | `E:/GitProjects/BIM-IFC/IfcOpenshell` |
| `IFCSPACIALIMPORT_ARCH` | `x64`, `Win32` | `x64` |
| `IFCSPACIALIMPORT_TOOLSET` | `vs2017`, `vs2022` | `vs2017` |

## Troubleshooting

### "Cannot find IfcParse.lib"
- Ensure IfcOpenShell is built at the expected path
- Verify `IFCOPENSHELL_ROOT` points to the correct directory
- Check that build output exists at `{IFCOPENSHELL_ROOT}/_build-{toolset}-{arch}/Debug` (for Debug builds)

### "Boost library not found"
- Verify Boost is at `{IFCOPENSHELL_ROOT}/_deps/boost_1_79_0`
- For different Boost versions, update library names in CMakeLists.txt

### Header include errors
- Check paths in the "INCLUDE DIRECTORIES" section of CMakeLists.txt
- Ensure all dependencies are installed in `{IFCOPENSHELL_ROOT}/_deps-{toolset}-{arch}-installed`

### Linker errors (LNK1104, LNK2019)
- Verify all library paths are correct and exist
- Check that OCCT is installed at `{IFCOPENSHELL_ROOT}/_deps-{toolset}-{arch}-installed/opencascade-7.7.2`
- Ensure toolset matches the compiled libraries (v141 for VS2017, v143 for VS2022)

## Architecture Support

### x64 (Recommended)
```powershell
cmake .. -G "Visual Studio 15 2017" -A x64
```

### Win32 (32-bit)
```powershell
cmake .. -G "Visual Studio 15 2017" -A Win32
```

Note: Ensure dependencies are also built for the target architecture.

## Clean Build

```powershell
cd build
rm -r *
cmake .. -G "Visual Studio 15 2017" -A x64
cmake --build . --config Debug
```

## Integration with IDE

### Visual Studio
After running cmake with the Visual Studio generator, open the generated `IfcSpacialImport.sln` in Visual Studio:
```powershell
start IfcSpacialImport.sln
```

Then build and debug as normal.

### Visual Studio Code

1. Install the CMake extension
2. Open the project folder
3. Select the Visual Studio 15 2017 kit
4. Build and Debug from the Command Palette (Ctrl+Shift+P)

## Differences from MSBuild (.vcxproj)

- CMake-based build is more portable but requires CMake installation
- Configuration options are more explicit and easier to modify
- Output directory structure is slightly different (build/bin/config/ vs x64/config/)
- All dependencies must be at the paths specified in CMakeLists.txt

## Platform Considerations

### Boost Library Names
The CMakeLists.txt assumes Boost 1.79 built with MSVC v141 (Visual Studio 2017 toolset). For different versions:
1. Find library filenames in `{IFCOPENSHELL_ROOT}/_deps/boost_X_Y_Z/stage/{toolset}-{arch}/lib/`
2. Update library names in the Boost section of CMakeLists.txt

### OpenCASCADE (OCCT)
Currently hardcoded for OCCT 7.7.2. To use a different version:
1. Update include path: `"${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-X.Y.Z/inc"`
2. Update library path: `"${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-X.Y.Z/win64/lib"`
3. Update TK* library names if they changed
