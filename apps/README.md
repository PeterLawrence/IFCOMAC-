# IfcSpacialImport Test Applications

This directory contains integration test applications that demonstrate how to use the IfcSpacialImport DLL.

## Applications

### 1. IfcSpacialImport_TestApp.exe

A simple demonstration application that shows how to:
- Load the IfcSpacialImport.dll dynamically using Windows API
- Locate and call the `FullIfcImporter` function
- Handle DLL loading errors gracefully

This application does not require linking against the DLL at compile time and can run standalone after the DLL is built.

**Usage:**
```bash
IfcSpacialImport_TestApp.exe [path_to_ifc_file]
```

### 2. IfcSpacialImport_HotelExtractor.exe

A more complete integration test that:
- Loads the IfcSpacialImport DLL and calls `FullIfcImporter`
- Processes the `hotel-v12.ifc` file from TestFiles directory
- Extracts and displays building information (storeys, spaces, doors)
- Shows how to use the extracted `IFCBuildingModel` data

This application links against the main IfcSpacialImport library and includes its header files.

**Usage:**
```bash
IfcSpacialImport_HotelExtractor.exe [path_to_ifc_file]
```

Default file: `..\..\TestFiles\hotel-v12.ifc`

## Building

### Build both applications (default):
```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64
cmake --build build --config Release
```

### Build only the main DLL (no apps):
```bash
cmake -S . -B build -G "Visual Studio 15 2017" -A x64 -DBUILD_APPS=OFF
cmake --build build --config Release
```

## Running

After successful build:

```bash
# Simple loader test
build\bin\apps\Release\IfcSpacialImport_TestApp.exe

# Hotel extractor (full extraction)
build\bin\apps\Release\IfcSpacialImport_HotelExtractor.exe

# With custom IFC file
build\bin\apps\Release\IfcSpacialImport_HotelExtractor.exe "path\to\your\file.ifc"
```

## Output Example

```
================================================================================
IfcSpacialImport - Hotel IFC Extractor
================================================================================

Configuration:
  Input File: ..\..\TestFiles\hotel-v12.ifc

------------------------------------------------------------
[*] Step 1: Verifying IFC File
  File found: ..\..\TestFiles\hotel-v12.ifc
  File size: 4.32 MB

[*] Step 2: Loading IfcSpacialImport.dll
  Successfully loaded: ..\..\build\bin\Release\IfcSpacialImport.dll
  Module handle: 00007FF8A5430000

[*] Step 3: Locating FullIfcImporter Function
  Function address: 00007FF8A5437B20
  Function ready to call

[*] Step 4: Extracting IFC Data
  Calling FullIfcImporter...
  Extraction completed successfully!

[*] Step 5: Extracted Building Information
  Total Storeys: 5

  Storey 1:
    Name: Ground Floor
    GUID: 3rtAAyE5P2h8pKDWxSkkhw
    Elevation: 0.00 m
    Spaces: 12
      - Main Lobby
      - Reception Area
      - Restaurant
      ... and 9 more

  [More storeys...]

================================================================================
Extraction Summary
================================================================================

Successfully extracted information from: ..\..\TestFiles\hotel-v12.ifc
Building model populated with 5 storey(s)

Next Steps:
  1. Modify this app to export data to JSON/CSV/other format
  2. Process geometric data from ExtractGeom()
  3. Integrate with your BIM workflow

Test application completed successfully!
```

## Integration Guide

To integrate the IfcSpacialImport DLL in your own application:

### C++ Integration (compile-time linking)

```cpp
#include "ExodusIFCClasses.h"

// Function declaration
extern "C" __declspec(dllimport) bool FullIfcImporter(const char* filename, IFCBuildingModel &MyBuilding);

int main() {
    IFCBuildingModel buildingModel;
    bool success = FullIfcImporter("file.ifc", buildingModel);
    
    if (success) {
        auto storeys = buildingModel.GetStoreys();
        // Process extracted data...
    }
    return 0;
}
```

### Dynamic Loading (runtime)

```cpp
#include <windows.h>

typedef bool(__cdecl* FullIfcImporterFunc)(const char*, IFCBuildingModel&);

HMODULE dll = LoadLibraryA("IfcSpacialImport.dll");
FullIfcImporterFunc importer = (FullIfcImporterFunc)GetProcAddress(dll, "FullIfcImporter");

IFCBuildingModel model;
bool result = importer("file.ifc", model);

FreeLibrary(dll);
```

## Troubleshooting

### DLL not found
- Ensure `IfcSpacialImport.dll` is built: `cmake --build build --config Release`
- Add the DLL path to PATH environment variable
- Or place DLL in the same directory as the executable

### Function not found
- Verify the DLL exports `FullIfcImporter`
- Check that the DLL was built correctly (no link errors)

### IFC file parsing fails
- Verify the IFC file is valid and not corrupted
- Check that the file uses a supported IFC schema (IFC 4.3 by default)
- Ensure IfcOpenShell libraries are available and properly built

## See Also

- Main README: `../README.md`
- Unit Tests: `../tests/README.md`
- CMakeLists.txt for app configuration: `CMakeLists.txt`
