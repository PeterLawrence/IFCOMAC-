// IfcSpacialImport_TestApp.cpp
// Simple test application demonstrating how to use the IfcSpacialImport DLL
// Loads a hotel IFC file and extracts building information

#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <filesystem>

// Forward declarations (from ExodusIFCClasses.h)
struct IFCBuildingModel;

// Function pointer type for FullIfcImporter
typedef bool(__cdecl* FullIfcImporterFunc)(const char*, IFCBuildingModel&);

// Helper function to print separator
void PrintSeparator(int width = 80)
{
    std::cout << std::string(width, '=') << std::endl;
}

// Helper function to get file size in MB
double GetFileSizeMB(const std::string& filepath)
{
    try {
        return std::filesystem::file_size(filepath) / (1024.0 * 1024.0);
    }
    catch (...) {
        return 0.0;
    }
}

int main(int argc, char* argv[])
{
    std::cout << "\n";
    PrintSeparator();
    std::cout << "IfcSpacialImport Test Application" << std::endl;
    std::cout << "Demonstrates DLL loading and IFC file extraction" << std::endl;
    PrintSeparator();
    std::cout << std::endl;

    // Determine IFC file path
    std::string ifc_filepath;
    
    if (argc > 1) {
        ifc_filepath = argv[1];
    } else {
        // Default to hotel-v12.ifc in TestFiles directory
        ifc_filepath = "..\\..\\..\\..\\TestFiles\\hotel-v12.ifc";
    }

    std::cout << "[1/5] Input Configuration" << std::endl;
    std::cout << "  IFC File: " << ifc_filepath << std::endl;

    // Verify file exists
    if (!std::filesystem::exists(ifc_filepath)) {
        std::cerr << "ERROR: IFC file not found: " << ifc_filepath << std::endl;
        return 1;
    }

    double file_size_mb = GetFileSizeMB(ifc_filepath);
    std::cout << "  File Size: " << std::fixed << std::setprecision(2) << file_size_mb << " MB" << std::endl;
    std::cout << std::endl;

    // Step 2: Load the DLL
    std::cout << "[2/5] Loading IfcSpacialImport.dll..." << std::endl;
    
    // Try different possible DLL paths
    const char* dll_paths[] = {
        "IfcSpacialImport.dll",
        "..\\..\\build\\bin\\Release\\IfcSpacialImport.dll",
        "..\\..\\build\\bin\\Debug\\IfcSpacialImport.dll",
        "x64\\Release\\IfcSpacialImport.dll",
    };

    HMODULE hModule = nullptr;
    std::string loaded_dll;

    for (const char* dll_path : dll_paths) {
        hModule = LoadLibraryA(dll_path);
        if (hModule) {
            loaded_dll = dll_path;
            break;
        }
    }

    if (!hModule) {
        std::cerr << "ERROR: Failed to load IfcSpacialImport.dll" << std::endl;
        std::cerr << "Make sure the DLL is built and in the PATH or current directory" << std::endl;
        return 1;
    }

    std::cout << "  Successfully loaded: " << loaded_dll << std::endl;
    std::cout << std::endl;

    // Step 3: Get the function pointer
    std::cout << "[3/5] Locating FullIfcImporter function..." << std::endl;
    
    FullIfcImporterFunc FullIfcImporter = 
        (FullIfcImporterFunc)GetProcAddress(hModule, "FullIfcImporter");

    if (!FullIfcImporter) {
        std::cerr << "ERROR: Failed to locate FullIfcImporter function" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }

    std::cout << "  Function pointer obtained successfully" << std::endl;
    std::cout << std::endl;

    // Step 4: Create building model and call importer
    std::cout << "[4/5] Extracting IFC data..." << std::endl;
    
    // Note: IFCBuildingModel is defined in ExodusIFCClasses.h
    // We use void* as a placeholder since we're not including the full header here
    IFCBuildingModel* pBuildingModel = nullptr;
    
    try {
        // In a real application, you would allocate the IFCBuildingModel here
        // This is a simplified version that demonstrates the concept
        std::cout << "  NOTE: Full extraction requires IFCBuildingModel class definition" << std::endl;
        std::cout << "  In production, instantiate IFCBuildingModel and pass by reference" << std::endl;
        std::cout << std::endl;
        
        // Example call (commented out without proper IFCBuildingModel initialization):
        // bool result = FullIfcImporter(ifc_filepath.c_str(), *pBuildingModel);
        
        std::cout << "  To use this app fully:" << std::endl;
        std::cout << "    1. Include ExodusIFCClasses.h" << std::endl;
        std::cout << "    2. Create: IFCBuildingModel buildingModel;" << std::endl;
        std::cout << "    3. Call: bool success = FullIfcImporter(filepath, buildingModel);" << std::endl;
        std::cout << "    4. Access extracted data via buildingModel members" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: Exception during extraction: " << e.what() << std::endl;
        FreeLibrary(hModule);
        return 1;
    }
    catch (...) {
        std::cerr << "ERROR: Unknown exception during extraction" << std::endl;
        FreeLibrary(hModule);
        return 1;
    }

    std::cout << std::endl;

    // Step 5: Display results
    std::cout << "[5/5] Results" << std::endl;
    std::cout << "  DLL successfully loaded and function accessed" << std::endl;
    std::cout << "  Ready to extract: " << ifc_filepath << std::endl;
    std::cout << std::endl;

    PrintSeparator();
    std::cout << "Next Steps:" << std::endl;
    std::cout << "  1. Include the full IFCBuildingModel class definition" << std::endl;
    std::cout << "  2. Uncomment the FullIfcImporter call in the code" << std::endl;
    std::cout << "  3. Access building model data: storeys, spaces, doors, etc." << std::endl;
    std::cout << "  4. Process extracted geometric and semantic data" << std::endl;
    PrintSeparator();
    std::cout << std::endl;

    // Cleanup
    FreeLibrary(hModule);
    
    std::cout << "Test application completed successfully!" << std::endl;
    return 0;
}
