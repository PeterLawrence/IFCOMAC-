# IfcSpacialImport Unit Tests

This directory contains unit tests for the IfcSpacialImport library using Google Test (gtest).

## Building Tests

To build and run unit tests, configure CMake with the `BUILD_TESTS` option enabled:

```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DBUILD_TESTS=ON
cmake --build build --config Release --target IfcSpacialImportTests
```

## Running Tests

Run all tests using CTest:

```bash
cd build
ctest -C Release --output-on-failure
```

Or run the test executable directly:

```bash
build\bin\tests\Release\IfcSpacialImportTests.exe
```

## Test Files

### test_exodus_ifc_utils.cpp

Tests for `ExodusIFCGeomUtils` namespace functionality:
- `EXODUSIFCExtract()` - Semantic data extraction from IFC files
- `ExtractGeom()` - Geometry extraction (integration testing)

#### Test Cases

1. **ExtractFromValidIFCFile** - Validates extraction from a valid IFC file
2. **ExtractFromInvalidFile** - Error handling for invalid/missing files
3. **BuildingModelInitialization** - Tests initial state of building model
4. **StoreyAddition** - Validates adding storeys to building model
5. **MultipleStoreysAddition** - Stress test with multiple storeys
6. **DoorModelProperties** - Tests door element creation and properties
7. **UpdateAssociations** - Tests post-processing association updates
8. **SpaceModelCreation** - Tests space/room element creation
9. **PointRepresentation** - Tests 3D point data structure
10. **BaseDataAttributes** - Tests common IFC entity attributes (Name, LongName)

## Test Data

To run tests that require actual IFC files, place sample IFC files in `test_data/`:

```
tests/
├── test_data/
│   └── sample.ifc
├── test_exodus_ifc_utils.cpp
└── CMakeLists.txt
```

Tests that cannot find required test data will be skipped automatically.

## Dependencies

- Google Test (gtest) - downloaded automatically via CMake FetchContent when `BUILD_TESTS=ON`
- IfcOpenShell libraries (same as main project)
- C++17 standard library

## Notes

- Tests are optional and disabled by default to keep the main build lean
- Use `-DBUILD_TESTS=ON` when configuring CMake to enable tests
- Google Test is fetched from GitHub if not available locally
- Test executables are built as standalone applications (not part of the DLL)
