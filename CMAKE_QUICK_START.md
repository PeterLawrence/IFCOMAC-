# CMake Build Quick Reference for IfcSpacialImport

## Installation

1. **Install CMake 3.20+**
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

2. **Verify Installation**
   ```powershell
   cmake --version
   ```

## Build Commands

### Using Batch Script (Recommended for Windows)

**First Time Setup:**
```batch
build.bat
```

**Debug Build:**
```batch
build.bat Debug
```

**Release Build:**
```batch
build.bat Release
```

**Clean Build Artifacts:**
```batch
build.bat clean
```

**Complete Rebuild:**
```batch
build.bat CleanAll
build.bat Release
```

### Using PowerShell Script

**Debug Build:**
```powershell
.\build.ps1 -Configuration Debug
```

**Release Build:**
```powershell
.\build.ps1 -Configuration Release
```

**Custom IfcOpenShell Root:**
```powershell
.\build.ps1 -Configuration Debug -IfcOpenShellRoot "C:\path\to\IfcOpenShell"
```

**Clean All:**
```powershell
.\build.ps1 -CleanAll
```

### Using CMake Directly

**Initial Configuration (x64):**
```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A x64
```

**Build:**
```powershell
cmake --build . --config Debug
cmake --build . --config Release
```

**Build with Visual Studio (after configuration):**
```powershell
devenv IfcSpacialImport.sln /Build Debug
```

## File Organization

```
IfcSpacialImport/
├── CMakeLists.txt           # CMake configuration
├── CMAKE_BUILD.md           # Detailed build documentation
├── build.bat                # Windows batch build helper
├── build.ps1                # PowerShell build helper
├── build/                   # CMake build directory (created on first run)
│   ├── bin/
│   │   ├── Debug/           # Debug DLL output
│   │   └── Release/         # Release DLL output
│   ├── lib/                 # Libraries (if any)
│   └── IfcSpacialImport.sln # Visual Studio solution
├── dllmain.cpp
├── exodusifcutils.cpp
└── ... (other source files)
```

## Output Locations

| Build Type | Location |
|-----------|----------|
| Debug DLL | `build\bin\Debug\IfcSpacialImport.dll` |
| Release DLL | `build\bin\Release\IfcSpacialImport.dll` |
| Debug PDB | `build\bin\Debug\IfcSpacialImport.pdb` |
| Release PDB | `build\bin\Release\IfcSpacialImport.pdb` |

## Configuration Options

Edit CMakeLists.txt or pass via command line:

```powershell
cmake .. `
  -DIFCOPENSHELL_ROOT="E:/path/to/IfcOpenshell" `
  -DIFCSPACIALIMPORT_ARCH="x64" `
  -DIFCSPACIALIMPORT_TOOLSET="vs2017"
```

## Troubleshooting

### Cannot find cmake command
- Ensure CMake is installed
- Add CMake bin directory to PATH (e.g., C:\Program Files\CMake\bin)
- Restart PowerShell after adding to PATH

### "Visual Studio 15 2017" generator not found
- Install Visual Studio 2017 C++ workload
- Or use different generator: `"Visual Studio 16 2019"` or `"Visual Studio 17 2022"`

### Linker errors after configuration changes
```batch
build.bat CleanAll
build.bat Debug
```

### Library not found errors
- Verify IfcOpenShell is built
- Check `IFCOPENSHELL_ROOT` path is correct
- Ensure all dependencies exist at expected paths

## Integration with Visual Studio

After CMake configuration, the generated solution can be opened:

```powershell
start build\IfcSpacialImport.sln
```

Then:
1. Set startup project to IfcSpacialImport
2. Select Debug or Release configuration
3. Build and debug as normal

## Comparison: MSBuild vs CMake

| Feature | MSBuild (.vcxproj) | CMake |
|---------|-------------------|-------|
| Setup | Integrated with VS | Requires CMake install |
| Portability | Visual Studio only | Cross-platform |
| Customization | Limited, XML-based | Flexible, scripting |
| IDE Support | VS only | VS, VSCode, CLion, etc |
| Rebuild Effort | Manual .sln/project edits | Edit CMakeLists.txt |

## Next Steps

1. Run `build.bat` to build the project
2. DLL will be in `build\bin\Debug\IfcSpacialImport.dll`
3. See CMAKE_BUILD.md for detailed configuration options
4. See CLAUDE.md for other build/test information
