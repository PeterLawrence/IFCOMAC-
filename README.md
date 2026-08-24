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

### Building Unit Tests

To build and run the unit tests suite (requires Google Test):

```
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DBUILD_TESTS=ON
cmake --build build --config Release --target IfcSpacialImportTests
```

Run tests with:
```
cd build && ctest -C Release --output-on-failure
```

See `tests/README.md` for detailed testing documentation.

## Notes & Troubleshooting

- The project links many IfcOpenShell variants. If you see duplicate-symbol LNK2005 errors, library version mismatches are likely.
- A temporary `/FORCE:MULTIPLE` linker flag was used during debugging to allow the build to complete. This is a quick-and-dirty workaround and may mask real symbol conflicts. Recommended long-term fix: rebuild IfcOpenShell and its mapping/serializer/geometry libs consistently for a single IFC 4.3 variant (e.g., ifc4x3_add2).
- Ensure `IFCOPENSHELL_ROOT` points to the IfcOpenShell source/build that matches the `.lib` files referenced by CMakeLists.txt.
- If unresolved externals referencing CgalKernel or SimpleCgalKernel occur, verify that the corresponding geometry kernel libraries are present and were built with the same toolset/arch.

## Developer Notes

See `CMakeLists.txt` for configurable cache variables, include/link paths, and explicit library lists used by the project.
