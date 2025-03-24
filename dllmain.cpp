// dllmain.cpp : Defines the entry point for the DLL application.


#include <string>
#include <iostream>
#include <fstream>

#include <Precision.hxx>

// Disable warnings coming from IfcOpenShell
#pragma warning(disable : 4018 4267 4250 4984 4985 4307)

#if USE_VLD
#include <vld.h>
#endif


#include "../src/ifcgeom/ConversionSettings.h"
#include "../src/ifcgeom/Converter.h"
#include "../src/ifcparse/IfcFile.h"
#include "../src/ifcparse/IfcSchema.h"

#include <map>
#include <string>
#include <variant>
#include "../src/examples/ExodusIFCClasses.h"
#include "../src/examples/extractgeom.h"

#include "pch.h"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport)  bool FullIfcImporter(const char* filename, IFCBuildingModel &MyBuilding)
{
#ifdef _DEBUG
    char text_buffer[200];
    snprintf(text_buffer, 200, "Loading %s\n", filename);
    OutputDebugStringA(text_buffer);
#endif
    IfcParse::IfcFile file(filename);
    if (!file.good()) {
        std::cout << "Unable to parse " << filename <<  " file" << std::endl;
#ifdef _DEBUG
        snprintf(text_buffer, 200, "Unable to parse ");
        OutputDebugStringA(text_buffer);
#endif
        return false;
    }
    const IfcParse::schema_definition* aSchema = file.schema();
    if (aSchema)
    {
        ifcopenshell::geometry::Settings settings;
        settings.get<ifcopenshell::geometry::settings::UseWorldCoords>().value = true;
        settings.get<ifcopenshell::geometry::settings::UseElementHierarchy>().value = true;
        //settings.get<ifcopenshell::geometry::settings::IteratorOutput>().value = ifcopenshell::geometry::settings::NATIVE;

        const std::string aSchemaString = aSchema->name();
        std::cout << aSchemaString << std::endl;
        
        ExodusIFCGeomUtils::EXODUSIFCExtract(file, MyBuilding);
        ExodusIFCGeomUtils::ExtractGeom(file, settings, MyBuilding);
        MyBuilding.UpdateAssociations();
        return true;
    }

    return false;
}
