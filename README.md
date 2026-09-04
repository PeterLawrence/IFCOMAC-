# IFCOMAC++

This project builds the IfcSpacialImport DLL which uses IfcOpenShell and related native libraries.

## CMake (Windows / Visual Studio 2017 x64)

### Building the DLL

1. Configure (set IfcOpenShell root if different):
   ```
   cmake -S . -B build -G "Visual Studio 15 2017" -A x64
   ```
   
   Optional cache overrides:
   ```
   -DIFCOPENSHELL_ROOT="E:/GitProjects/BIM-IFC/IfcOpenshell"
   -DIFCSPACIALIMPORT_ARCH=x64
   -DIFCSPACIALIMPORT_TOOLSET=vs2017
   ```

2. Build:
   ```
   cmake --build build --config Release
   cmake --build build --config Debug
   ```

### Build Outputs

- Release DLL: `build\bin\Release\IfcSpacialImport.dll`
- Debug DLL:    `build\bin\Debug\IfcSpacialImport.dll`

### Building Test Applications

The project includes test applications that demonstrate loading and using the IfcSpacialImport DLL:

#### IfcSpacialImport_HotelExtractor

This application loads the DLL and extracts building information from `hotel-v12.ifc`:

**Quick Build (Release):**
```powershell
cmake --build build --config Release --target IfcSpacialImport_HotelExtractor
```

**Quick Build (Debug):**
```powershell
cmake --build build --config Debug --target IfcSpacialImport_HotelExtractor
```

**Output:**
- Release: `build\bin\apps\Release\IfcSpacialImport_HotelExtractor.exe`
- Debug: `build\bin\apps\Debug\IfcSpacialImport_HotelExtractor.exe`

**Run the test app:**
```powershell
.\build\bin\apps\Release\IfcSpacialImport_HotelExtractor.exe
```

**Alternative Build Methods:**

Using MSBuild directly:
```powershell
cd build
msbuild IfcSpacialImport_HotelExtractor.vcxproj /p:Configuration=Release /p:Platform=x64
cd ..
```

Using the build script (PowerShell):
```powershell
.\build.ps1 -Configuration Release
```

Using the build script (Command Prompt):
```cmd
build.bat Release
```

See `apps/README.md` for detailed usage examples and integration patterns.

### Building and Running Unit Tests

The project includes 10 Google Test cases in
`tests\test_exodus_ifc_utils.cpp`. They cover:

- IFC extraction from valid and invalid files
- Building model initialization and storey management
- Door properties and space creation
- Building association updates
- 3D point coordinates
- Common name and long-name attributes

Google Test is downloaded automatically by CMake when tests are enabled. From
the repository root, configure the test target and build it:

```powershell
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DBUILD_TESTS=ON
cmake --build build --config Release --target IfcSpacialImportTests
```

Run the tests with CTest:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

The test executable can also be run directly:

```powershell
.\build\bin\Release\IfcSpacialImportTests.exe
```

The valid-file extraction test looks for `tests\test_data\sample.ifc`. If this
file is not present, that test is skipped; the remaining tests can still run.
See `tests\README.md` for the complete test list and test-data details.

### Building All Targets (DLL, Tests, and Apps)

To build everything in one command:

```powershell
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DBUILD_TESTS=ON
cmake --build build --config Release
```

Output will include:
- `build\bin\Release\IfcSpacialImport.dll`
- `build\bin\Release\IfcSpacialImportTests.exe`
- `build\bin\apps\Release\IfcSpacialImport_HotelExtractor.exe`
- `build\bin\apps\Release\IfcSpacialImport_TestApp.exe`

## Notes & Troubleshooting

- The project links many IfcOpenShell variants. If you see duplicate-symbol LNK2005 errors, library version mismatches are likely.
- A temporary `/FORCE:MULTIPLE` linker flag was used during debugging to allow the build to complete. This is a quick-and-dirty workaround and may mask real symbol conflicts. Recommended long-term fix: rebuild IfcOpenShell and its mapping/serializer/geometry libs consistently for a single IFC 4.3 variant (e.g., ifc4x3_add2).
- Ensure `IFCOPENSHELL_ROOT` points to the IfcOpenShell source/build that matches the `.lib` files referenced by CMakeLists.txt.
- If unresolved externals referencing CgalKernel or SimpleCgalKernel occur, verify that the corresponding geometry kernel libraries are present and were built with the same toolset/arch.

## Developer Notes

See `CMakeLists.txt` for configurable cache variables, include/link paths, and explicit library lists used by the project.
