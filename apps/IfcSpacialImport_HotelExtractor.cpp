// IfcSpacialImport_HotelExtractor.cpp
// Integration test application that loads hotel-v12.ifc and extracts building information
// This version includes proper IFCBuildingModel usage

#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <memory>

// Include the required headers
#include "../ExodusIFCClasses.h"

// Function pointer type for FullIfcImporter
typedef bool(__cdecl* FullIfcImporterFunc)(const char*, IFCBuildingModel&);

// Helper functions
void PrintHeader(const std::string& title)
{
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(80, '=') << "\n\n";
}

void PrintSection(const std::string& section)
{
    std::cout << "\n[*] " << section << "\n";
    std::cout << std::string(60, '-') << "\n";
}

double GetFileSizeMB(const std::string& filepath)
{
    try {
        return std::filesystem::file_size(filepath) / (1024.0 * 1024.0);
    }
    catch (...) {
        return 0.0;
    }
}

std::string GetDLLPath(const std::string& build_type = "Release")
{
    std::string paths[] = {
        "IfcSpacialImport.dll",
        "..\\build\\bin\\" + build_type + "\\IfcSpacialImport.dll",
        "..\\..\\build\\bin\\" + build_type + "\\IfcSpacialImport.dll",
        "x64\\" + build_type + "\\IfcSpacialImport.dll",
    };

    for (const auto& path : paths) {
        if (std::filesystem::exists(path)) {
            return path;
        }
    }
    return "";
}

int main(int argc, char* argv[])
{
    PrintHeader("IfcSpacialImport - Hotel IFC Extractor");

    // Configuration - try multiple paths
    std::string ifc_candidates[] = {
        "hotel-v12.ifc",                          // Current directory
        "../../TestFiles/hotel-v12.ifc",          // From build/bin/apps/Release/
        "../TestFiles/hotel-v12.ifc",             // From build/bin/
        "TestFiles/hotel-v12.ifc",                // From root
        "../TestFiles/hotel-v12.ifc"              // Fallback
    };

    std::string ifc_filepath;
    if (argc > 1) {
        ifc_filepath = argv[1];
    } else {
        // Find first existing path
        for (const auto& candidate : ifc_candidates) {
            if (std::filesystem::exists(candidate)) {
                ifc_filepath = candidate;
                break;
            }
        }
    }

    std::cout << "Configuration:\n";
    std::cout << "  Input File: " << ifc_filepath << "\n";

    // Step 1: Verify IFC file exists
    PrintSection("Step 1: Verifying IFC File");

    if (!std::filesystem::exists(ifc_filepath)) {
        std::cerr << "  ERROR: File not found: " << ifc_filepath << "\n\n";
        return 1;
    }

    double file_size = GetFileSizeMB(ifc_filepath);
    std::cout << "  File found: " << ifc_filepath << "\n";
    std::cout << "  File size: " << std::fixed << std::setprecision(2) << file_size << " MB\n";

    // Step 2: Load DLL
    PrintSection("Step 2: Loading IfcSpacialImport.dll");

    std::string dll_path = GetDLLPath("Release");
    if (dll_path.empty()) {
        dll_path = GetDLLPath("Debug");
    }

    if (dll_path.empty()) {
        std::cerr << "  ERROR: Could not find IfcSpacialImport.dll\n";
        std::cerr << "  Make sure the DLL is built. Try:\n";
        std::cerr << "    cmake --build build --config Release\n\n";
        return 1;
    }

    HMODULE hModule = LoadLibraryA(dll_path.c_str());
    if (!hModule) {
        std::cerr << "  ERROR: Failed to load DLL: " << dll_path << "\n";
        std::cerr << "  Error code: " << GetLastError() << "\n\n";
        return 1;
    }

    std::cout << "  Successfully loaded: " << dll_path << "\n";
    std::cout << "  Module handle: " << std::hex << hModule << std::dec << "\n";

    // Step 3: Locate function
    PrintSection("Step 3: Locating FullIfcImporter Function");

    FullIfcImporterFunc FullIfcImporter = 
        (FullIfcImporterFunc)GetProcAddress(hModule, "FullIfcImporter");

    if (!FullIfcImporter) {
        std::cerr << "  ERROR: Could not find FullIfcImporter function\n";
        std::cerr << "  The DLL may not be correctly built or exported\n\n";
        FreeLibrary(hModule);
        return 1;
    }

    std::cout << "  Function address: " << std::hex << (void*)FullIfcImporter << std::dec << "\n";
    std::cout << "  Function ready to call\n";

    // Step 4: Extract IFC data
    PrintSection("Step 4: Extracting IFC Data");

    IFCBuildingModel buildingModel;

    try {
        std::cout << "  Calling FullIfcImporter...\n";
        bool result = FullIfcImporter(ifc_filepath.c_str(), buildingModel);

        if (!result) {
            std::cerr << "  ERROR: FullIfcImporter returned false\n";
            std::cerr << "  IFC file may be invalid or unsupported\n\n";
            FreeLibrary(hModule);
            return 1;
        }

        std::cout << "  Extraction completed successfully!\n";
    }
    catch (const std::exception& e) {
        std::cerr << "  ERROR: Exception during extraction: " << e.what() << "\n\n";
        FreeLibrary(hModule);
        return 1;
    }
    catch (...) {
        std::cerr << "  ERROR: Unknown exception during extraction\n\n";
        FreeLibrary(hModule);
        return 1;
    }

    // Step 5: Display extracted data
    PrintSection("Step 5: Extracted Building Information");

    try {
        size_t num_stories = buildingModel.NumberOfStories();
        std::cout << "  Total Storeys: " << num_stories << "\n\n";

        // Display storey details
        for (size_t i = 0; i < num_stories; ++i) {
            auto storey = buildingModel.GetStorey(i);
            if (!storey) continue;

            std::cout << "  Storey " << (i + 1) << ":\n";
            std::cout << "    Name: " << storey->GetName() << "\n";
            std::cout << "    GUID: " << storey->GetGUID() << "\n";
            std::cout << "    Elevation: " << std::fixed << std::setprecision(2) 
                      << storey->GetElevationOfFFLRelative() << " m\n";
            
            // Display spaces count if available
            try {
                std::cout << "    Spaces: (see property sets)\n";
            }
            catch (...) {
                // Spaces may not be available
                std::cout << "    Spaces: (not extracted)\n";
            }

            std::cout << "\n";
        }

        // Display summary
        PrintSection("Building Summary");
        std::cout << "  Building Bounding Box:\n";
        auto& min_pt = buildingModel.GetMinCoordinate();
        auto& max_pt = buildingModel.GetMaxCoordinate();
        std::cout << "    Min: (" << min_pt.Get(0) << ", " << min_pt.Get(1) << ", " << min_pt.Get(2) << ")\n";
        std::cout << "    Max: (" << max_pt.Get(0) << ", " << max_pt.Get(1) << ", " << max_pt.Get(2) << ")\n";
    }
    catch (const std::exception& e) {
        std::cerr << "  ERROR while displaying results: " << e.what() << "\n";
    }

    // Summary
    PrintHeader("Extraction Summary");
    std::cout << "Successfully extracted information from: " << ifc_filepath << "\n";
    std::cout << "Building model populated with " << buildingModel.NumberOfStories() << " storey(s)\n";
    std::cout << "\nNext Steps:\n";
    std::cout << "  1. Modify this app to export data to JSON/CSV/other format\n";
    std::cout << "  2. Process geometric data from ExtractGeom()\n";
    std::cout << "  3. Integrate with your BIM workflow\n";

    // Cleanup
    FreeLibrary(hModule);
    std::cout << "\nTest application completed successfully!\n\n";

    return 0;
}

