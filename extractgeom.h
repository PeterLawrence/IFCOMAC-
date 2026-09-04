/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "ifcgeom/ConversionSettings.h"
#include "ifcparse/IfcFile.h"
#include "ifcparse/IfcParse.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef IFCSPACIALIMPORT_API
#if defined(_WIN32)
#if defined(IFCSPACIALIMPORT_EXPORTS)
#define IFCSPACIALIMPORT_API __declspec(dllexport)
#else
#define IFCSPACIALIMPORT_API __declspec(dllimport)
#endif
#else
#define IFCSPACIALIMPORT_API
#endif
#endif

class IFCBuildingModel;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ExodusIFCGeomUtils
{
    int TestExtractGeom(IfcParse::IfcFile& file, ifcopenshell::geometry::Settings& settings);
    int ExtractGeom(IfcParse::IfcFile& file, ifcopenshell::geometry::Settings& settings, IFCBuildingModel &MyBuilding);
    IFCSPACIALIMPORT_API bool EXODUSIFCExtract(IfcParse::IfcFile &file, IFCBuildingModel &MyBuilding);
    IFCSPACIALIMPORT_API bool EXODUSIFCExtract(const char* filename, IFCBuildingModel &MyBuilding);
}