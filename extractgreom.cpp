/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Disable warnings coming from IfcOpenShell
#pragma warning(disable : 4018 4267 4250 4984 4985 4307 4702)
#include "../ifcgeom/IfcGeomRepresentation.h"
#include "../ifcgeom/GeometrySerializer.h"
#include "../ifcgeom/Iterator.h"
#include "../ifcgeom/AbstractKernel.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "extractgeom.h"
#include "ExodusIFCClasses.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "exodusifcutils.h"
#include "getpset.h"
#include <Windows.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ExodusIFCGeomUtils::TestExtractGeom(IfcParse::IfcFile& file, ifcopenshell::geometry::Settings& settings)
{
    IfcGeom::filter_t f = [](IfcUtil::IfcBaseEntity* aEntry) {
        try {
        std::string guid =aEntry->get("GlobalId");
        if (guid.compare("3rtAAyE5P2h8pKDWxSkkhw") == 0)
        {
            return true;
        }
        }
        catch (IfcParse::IfcException &)
        {

        }
        return false;
    };
    std::cout << "opencascade \n";

    std::vector<IfcGeom::filter_t> aFilterList;
    aFilterList.push_back(f);
    IfcGeom::Iterator context_iterator("opencascade", settings, &file, aFilterList, 1);

    context_iterator.initialize();
    int num_created = 0;
    for (;; ++num_created) {
        bool has_more = true;
        if (num_created) {
            has_more = context_iterator.next();
        }
        IfcGeom::BRepElement* geom_object = nullptr;
        if (has_more) {
            geom_object = context_iterator.get_native();
        }
        if (!geom_object) {
            break;
        }
        if (geom_object->guid().compare("3rtAAyE5P2h8pKDWxSkkhw") == 0) {
            std::cout << " context_iterator name:" << geom_object->name() << " " << geom_object->guid() << std::endl;
            std::cout << " context " <<  geom_object->context() << std::endl;
            std::cout << " type " << geom_object->type() << std::endl;
            //BRepElement* anElement = geom_object->get_native();
            auto GeomObject = context_iterator.get();
            const IfcGeom::TriangulationElement* o = static_cast<const IfcGeom::TriangulationElement*>(GeomObject);
            if (o) {
                std::cout << " has meshs\n";
                const IfcGeom::Representation::Triangulation& mesh = o->geometry();
                int vc = 0;
                for (typename std::vector<double>::const_iterator it = mesh.verts().begin(); it != mesh.verts().end();) {
                    const double& x = *(it++);
                    const double& y = *(it++);
                    const double& z = *(it++);
                    std::cout << " vert " << vc << " " << x << " " << " " << y << " " << z << "\n";
                    ++vc;
                }
                int fc = 0;
                for (typename std::vector<int>::const_iterator it = mesh.faces().begin(); it != mesh.faces().end();) {
                    const int& f0 = *(it++);
                    const int& f1 = *(it++);
                    const int& f2 = *(it++);
                    std::cout << " face " << fc << " " << f0 << " " << " " << f1 << " " << f2 << "\n";
                    ++fc;
                }
                return 0;
            }
        }
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ExodusIFCGeomUtils::ExtractGeom(IfcParse::IfcFile& file, ifcopenshell::geometry::Settings& settings, IFCBuildingModel &MyBuilding)
{
    std::cout << "Getting Geometry\n";
    IfcGeom::filter_t f = [MyBuilding](IfcUtil::IfcBaseEntity* aEntry) {
        try {
            std::string guid = aEntry->get("GlobalId");
            return MyBuilding.GeometryRequired(guid);
        }
        catch (IfcParse::IfcException &)
        {

        }
        return false;
    };
    std::cout << "opencascade \n";

    std::vector<IfcGeom::filter_t> aFilterList;
    aFilterList.push_back(f);
    IfcGeom::Iterator context_iterator("opencascade", settings, &file, aFilterList, 1);
    //IfcGeom::Iterator context_iterator("cgal-simple", settings, &file, aFilterList, 1);
    //IfcGeom::Iterator context_iterator("cgal", settings, &file, aFilterList, 1);
    if (context_iterator.initialize() == false)
    {
        return false;
    }
    int num_created = 0;
    IFCPoint MaxPoint, MinPoint;
    MaxPoint.SetToMin(); // Max is set to min
    MinPoint.SetToMax(); // Min is set to max

    for (;; ++num_created) {
        bool has_more = true;
        if (num_created)
        {
            // this next command can be very slow, need to identify what's going on here
            has_more = context_iterator.next();
        }
        IfcGeom::BRepElement* geom_object = nullptr;
        if (has_more) {
            geom_object = context_iterator.get_native();
        }
        if (!geom_object) {
            break;
        }
        auto GeomObject = context_iterator.get();
        const IfcGeom::TriangulationElement* o = static_cast<const IfcGeom::TriangulationElement*>(GeomObject);
        if (o)
        {
            //std::cout << "Have triagulation for " << geom_object->guid() << " " << geom_object->name() <<  std::endl;
#ifdef _DEBUG
            char text_buffer[200];
            snprintf(text_buffer,200, "Have triagulation for %s %s\n", geom_object->guid().c_str(), geom_object->name().c_str()); 
            OutputDebugStringA(text_buffer);
#endif
            const IfcGeom::Representation::Triangulation& mesh = o->geometry();
            if (geom_object->type().compare("IfcDoor") == 0)
            {
                std::shared_ptr<IFCDoorModel> aDoor = MyBuilding.GetDoor(geom_object->guid());
                if (aDoor)
                {
                    aDoor->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aDoor);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        aDoor->GenerateBoundary(double(Elev), 0.1);
                    }
                }
            }
            else if (geom_object->type().compare("IfcStairFlight") == 0)
            {
                std::shared_ptr<IFCStairFlightModel> aStairFlight = MyBuilding.GetStairFlight(geom_object->guid());
                if (aStairFlight)
                {
                    aStairFlight->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aStairFlight);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        aStairFlight->GenerateBoundary(double(Elev), 0.1);
                    }
                }
            }
            else if (geom_object->type().compare("IfcSlab") == 0)
            {
                std::shared_ptr<IFCLandingModel> aLanding = MyBuilding.GetLanding(geom_object->guid());
                if (aLanding) // either a stair or ramp landing
                {
                    aLanding->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aLanding);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        aLanding->GenerateBoundary(double(Elev), 0.1);
                    }
                }
            }
            else if (geom_object->type().compare("IfcWall") == 0)
            {
                std::shared_ptr<IFCWallModel> aWall = MyBuilding.GetWall(geom_object->guid());
                if (aWall)
                {
                    aWall->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aWall);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        if (aWall->GenerateBoundary(double(Elev), 0.1))
                        {
                            IFCTriangulation::point_list aBoundary = aWall->GetBoundaryLines();
                            for (auto points : aBoundary)
                            {
                                MaxPoint.SetToMax(points[0], points[1], points[2]);
                                MinPoint.SetToMin(points[0], points[1], points[2]);
                                MaxPoint.SetToMax(points[3], points[4], points[5]);
                                MinPoint.SetToMin(points[3], points[4], points[5]);
                            }
                        }
                    }
                }
            }
            else if (geom_object->type().compare("IfcColumn") == 0)
            {
                std::shared_ptr<IFCColumnModel> aColumn = MyBuilding.GetColumn(geom_object->guid());
                if (aColumn)
                {
                    aColumn->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aColumn);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();

                        if (aColumn->GenerateBoundary(double(Elev), 0.1))
                        {
                            IFCTriangulation::point_list aBoundary = aColumn->GetBoundaryLines();
                            for (auto points : aBoundary)
                            {
                                MaxPoint.SetToMax(points[0], points[1], points[2]);
                                MinPoint.SetToMin(points[0], points[1], points[2]);
                                MaxPoint.SetToMax(points[3], points[4], points[5]);
                                MinPoint.SetToMin(points[3], points[4], points[5]);
                            }
                        }
                    }
                }
            }
            else if (geom_object->type().compare("IfcTransportElement") == 0)
            {
                std::shared_ptr<IFCTransportElement> aTransportElement = MyBuilding.GetTransportElement(geom_object->guid());
                if (aTransportElement)
                {
                    aTransportElement->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aTransportElement);
                    if (aStory)
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        aTransportElement->GenerateBoundary(double(Elev), 0.1);
                    }
                }
            }
            else if (geom_object->type().compare("IfcRamp") == 0)
            {
                // Note IfcRamp can have their own shape representation 
                // of the each part of the landing has it's own shape representation, such as IfcRampFlight
                std::shared_ptr<IFCRampModel> aRampElement = MyBuilding.GetRamp(geom_object->guid());
                if (aRampElement)
                {
                    aRampElement->SetTriangulation(mesh.verts(), mesh.faces());
                }
            }
            else if (geom_object->type().compare("IfcRampFlight") == 0)
            {
                std::shared_ptr<IFCRampFlightModel> aStairFlight = MyBuilding.GetRampFlight(geom_object->guid());
                if (aStairFlight)
                {
                    aStairFlight->SetTriangulation(mesh.verts(), mesh.faces());
                }
            }
            else if (geom_object->type().compare("IfcSign") == 0)
            {
                std::shared_ptr<IFCSignModel> aSignElement = MyBuilding.GetSign(geom_object->guid());
                if (aSignElement)
                {
                    aSignElement->SetTriangulation(mesh.verts(), mesh.faces());
                }
            }
            else if (geom_object->type().compare("IfcSpace") == 0)
            {
                std::shared_ptr<IFCSpaceModel> aSpace = MyBuilding.GetSpace(geom_object->guid());
                if (aSpace)
                {
                    aSpace->SetTriangulation(mesh.verts(), mesh.faces());
                    std::shared_ptr<IFCStoreyModel> aStory = MyBuilding.GetStorey(aSpace);
                    if (aStory == nullptr)
                    {
                        std::cout << "Upper " << aSpace->GetUpperHeight() << " Lower " << aSpace->GetLowerHeight() << " Elev " << "Unkown\n";
                    }
                    else
                    {
                        float Elev = aStory->GetElevationOfFFLRelative();
                        std::cout << "Upper " << aSpace->GetUpperHeight() << " Lower " << aSpace->GetLowerHeight() << " Elev " << aStory->GetElevationOfFFLRelative() << " Area " << aSpace->GetFloorArea(double(Elev), 0.1) << std::endl;

                        if (aSpace->GenerateBoundary(double(Elev), 0.1))
                        {
                            IFCTriangulation::point_list aBoundary = aSpace->GetBoundaryLines();
                            for (auto points : aBoundary)
                            {
                                MaxPoint.SetToMax(points[0], points[1], points[2]);
                                MinPoint.SetToMin(points[0], points[1], points[2]);
                                MaxPoint.SetToMax(points[3], points[4], points[5]);
                                MinPoint.SetToMin(points[3], points[4], points[5]);
                                //std::cout << points[0] << "," << points[1] << "," << points[2] << ",:," << points[3] << "," << points[4] << "," << points[5] << std::endl;
                            }
                        }
                    }
                }
            }
            else
            {
                std::cout << " Unknown type " << geom_object->type() << " context " << geom_object->context() << std::endl;
            }
        }
    }
    MyBuilding.SetMaxCoordinate(MaxPoint);
    MyBuilding.SetMinCoordinate(MinPoint);
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ExodusIFCGeomUtils
{
    float g_multi = 0.001f; // factor to convert to metres
    float toMetres(float aValue)
    {
        return (aValue*g_multi);
    }
    float toMetres(std::string &aPropValue)
    {
        float aValue = std::stof(aPropValue);
        return (aValue*g_multi);
    }
    float toSquareMetres(std::string &aPropValue)
    {
        float aValue = std::stof(aPropValue);
        return (aValue);
    }
    float toMetresSecond(std::string &aPropValue)
    {
        float aValue = std::stof(aPropValue);
        return (aValue);
    }
    float toMetresSecondSquared(std::string &aPropValue)
    {
        float aValue = std::stof(aPropValue);
        return (aValue);
    }
    void GetStairProperties(const IFCSchema::IfcObjectDefinition* a_stair_element, std::shared_ptr<IFCStairModelData> a_stair)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_stair_element, "Pset_StairFlightCommon", "RiserHeight", aPropValue))
        {
            std::cout << "Found " << "Pset_StairFlightCommon" << " " << "RiserHeight" << " " << aPropValue << std::endl;
            a_stair->SetRiserHeight(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_stair_element, "Pset_StairFlightCommon", "NumberOfRiser", aPropValue))
        {
            std::cout << "Found " << "Pset_StairFlightCommon" << " " << "NumberOfRiser" << " " << aPropValue << std::endl;
            a_stair->SetNumberOfRisers(std::stoi(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_stair_element, "Pset_StairFlightCommon", "TreadLength", aPropValue))
        {
            std::cout << "Found " << "Pset_StairFlightCommon" << " " << "TreadLength" << " " << aPropValue << std::endl;
            a_stair->SetTreadLength(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_stair_element, "Pset_StairFlightCommon", "NosingLength", aPropValue))
        {
            std::cout << "Found " << "Pset_StairFlightCommon" << " " << "NosingLength" << " " << aPropValue << std::endl;
            a_stair->SetNosingLength(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_stair_element, "Pset_StairFlightCommon", "NumberOfTreads", aPropValue))
        {
            std::cout << "Found " << "Pset_StairFlightCommon" << " " << "NumberOfTreads" << " " << aPropValue << std::endl;
            a_stair->SetNumberOfTreads(std::stoi(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_stair_element, "ePset_OMA_Stair", "EffectiveWidth", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_Stair" << " " << "EffectiveWidth" << " " << aPropValue << std::endl;
            a_stair->SetEffectiveWidth(toMetres(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //! \brief In older versions of IFC StairFlight has a number of properties defined on the element
    //!        not on the property set  Pset_StairFlightCommon
    //
    void GetStairflightOldProperties(const IFCSchema::IfcStairFlight* a_stairflight_element, std::shared_ptr<IFCStairFlightModel> a_stairflight)
    {
        // account for older IFC version saving of stairflight information
        if (a_stairflight_element->NumberOfRisers().has_value())
        {
            a_stairflight->SetNumberOfRisers(a_stairflight_element->NumberOfRisers().get());
        }
        if (a_stairflight_element->RiserHeight().has_value())
        {
            a_stairflight->SetRiserHeight(float(a_stairflight_element->RiserHeight().get()));
        }
        if (a_stairflight_element->NumberOfTreads().has_value())
        {
            a_stairflight->SetNumberOfTreads(a_stairflight_element->NumberOfTreads().get());
        }
        if (a_stairflight_element->TreadLength().has_value())
        {
            a_stairflight->SetTreadLength(float(a_stairflight_element->TreadLength().get()));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetTransportProperties(const IFCSchema::IfcObjectDefinition* an_transport_element, std::shared_ptr<IFCTransportElement> a_transport)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(an_transport_element, "Pset_ElementKinematics", "MaximumConstantSpeed", aPropValue))
        {
            std::cout << "Found " << "Pset_ElementKinematics" << " " << "MaximumConstantSpeed" << " " << aPropValue << std::endl;
            a_transport->SetMaximumConstantSpeed(toMetresSecond(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_transport_element, "ePset_OMA_TransportElementCommon", "LoadingArea", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "LoadingArea" << " " << aPropValue << std::endl;
            a_transport->SetLoadingArea(toSquareMetres(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetEscalatorProperties(const IFCSchema::IfcObjectDefinition* an_escaltor_element, std::shared_ptr<IFCEscalatorModel> an_escalator)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(an_escaltor_element, "ePset_OMA_TransportElementCommon", "RiserHeight", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "RiserHeight" << " " << aPropValue << std::endl;
            an_escalator->SetRiserHeight(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_escaltor_element, "ePset_OMA_TransportElementCommon", "RunHeight", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "RunHeight" << " " << aPropValue << std::endl;
            an_escalator->SetRunHeight(toMetres(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetEscalatorMovingWalkwayProperties(const IFCSchema::IfcObjectDefinition* a_Conveyor_element, std::shared_ptr<IFCConveyorModel> a_Conveyor)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_Conveyor_element, "ePset_OMA_TransportElementCommon", "TreadLength", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "TreadLength" << " " << aPropValue << std::endl;
            a_Conveyor->SetTreadLength(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_Conveyor_element, "ePset_OMA_TransportElementCommon", "RunLength", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "RunLength" << " " << aPropValue << std::endl;
            a_Conveyor->SetRunLength(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_Conveyor_element, "ePset_OMA_TransportElementCommon", "OverallLength", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "OverallLength" << " " << aPropValue << std::endl;
            a_Conveyor->SetLength(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_Conveyor_element, "ePset_OMA_TransportElementCommon", "ClearWidth", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "ClearWidth" << " " << aPropValue << std::endl;
            a_Conveyor->SetClearWidth(toMetres(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetElevatorProperties(const IFCSchema::IfcObjectDefinition* an_elevator_element, std::shared_ptr<IFCElevatorModel> an_elevator)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementCommon", "CapacityPeople", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementCommon" << " " << "CapacityPeople" << " " << aPropValue << std::endl;
            an_elevator->SetCapacity(std::stoi(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "Pset_TransportElementElevator", "ClearDepth", aPropValue))
        {
            std::cout << "Found " << "Pset_TransportElementElevator" << " " << "ClearDepth" << " " << aPropValue << std::endl;
            an_elevator->ClearDepth(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "Pset_TransportElementElevator", "ClearWidth", aPropValue))
        {
            std::cout << "Found " << "Pset_TransportElementElevator" << " " << "ClearWidth" << " " << aPropValue << std::endl;
            an_elevator->SetClearWidth(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator","MotorDelayTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "MotorDelayTime" << " " << aPropValue << std::endl;
            an_elevator->SetMotorDelayTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "AccelerationRate", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "AccelerationRate" << " " << aPropValue << std::endl;
            an_elevator->SetAccelerationRate(toMetresSecondSquared(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "DecelerationRate", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "DecelerationRate" << " " << aPropValue << std::endl;
            an_elevator->SetDecelerationRate(toMetresSecondSquared(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "AdvancedDoorOpeningTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "AdvancedDoorOpeningTime" << " " << aPropValue << std::endl;
            an_elevator->SetAdvancedDoorOpeningTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "DoorOpeningTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "DoorOpeningTime" << " " << aPropValue << std::endl;
            an_elevator->SetDoorOpeningTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "DoorClosingTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "DoorClosingTime" << " " << aPropValue << std::endl;
            an_elevator->SetDoorClosingTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "DoorDwellTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "DoorDwellTime" << " " << aPropValue << std::endl;
            an_elevator->SetDoorDwellTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "LevellingDelayTime", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "LevellingDelayTime" << " " << aPropValue << std::endl;
            an_elevator->SetLevellingDelayTime(std::stof(aPropValue));
        }
        if (get_psets_s<IFCSchema>(an_elevator_element, "ePset_OMA_TransportElementElevator", "JerkRate", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_TransportElementElevator" << " " << "JerkRate" << " " << aPropValue << std::endl;
            an_elevator->SetJerkRate(std::stof(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetRampProperties(const IFCSchema::IfcRamp* a_ramp_element, std::shared_ptr<IFCRampModel> a_Ramp)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_ramp_element, "Pset_RampCommon", "RequiredSlope", aPropValue))
        {
            a_Ramp->SetSlop(std::stof(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void GetSignProperties(const IFCSchema::IfcSign* a_sign_element, std::shared_ptr<IFCSignModel> a_Sign)
    {
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_sign_element, "ePset_OMA_Signage", "Title", aPropValue))
        {
            a_Sign->SetTitle(aPropValue);
        }
        if (get_psets_s<IFCSchema>(a_sign_element, "ePset_OMA_Signage", "LetterHeight", aPropValue))
        {
            a_Sign->SetLetterHeight(toSquareMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_sign_element, "ePset_OMA_Signage", "HeightFromTheGround", aPropValue))
        {
            a_Sign->SetHeightFromGround(toSquareMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_sign_element, "ePset_OMA_Signage", "Usage", aPropValue))
        {
            a_Sign->SetUsage(aPropValue);
        }
        if (get_psets_s<IFCSchema>(a_sign_element, "Qto_SignBaseQuantities", "Height", aPropValue))
        {
            a_Sign->SetWidth(toSquareMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_sign_element, "Qto_SignBaseQuantities", "Width", aPropValue))
        {
            a_Sign->SetHeight(toSquareMetres(aPropValue));
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool AddElevatorSpatialData(IFCSchema::IfcRelAssignsToGroup::list::ptr aGroup, IFCBuildingModel &MyBuilding)
    {
        if (aGroup != nullptr)
        {
            std::vector<std::shared_ptr<IFCSpaceModel>> FoundSpaces;
            std::shared_ptr<IFCElevatorModel> aElevatorModel;

            for (auto a_RelatingSpace_it = aGroup->begin(); a_RelatingSpace_it != aGroup->end(); ++a_RelatingSpace_it)
            {
                auto* a_RelatingSpace = *a_RelatingSpace_it;
                aggregate_of<IFCSchema::IfcObjectDefinition >::ptr aRelatedObjects = a_RelatingSpace->RelatedObjects();
                if (aRelatedObjects)
                {
                    for (auto a_RelatedObjects_it = aRelatedObjects->begin(); a_RelatedObjects_it != aRelatedObjects->end(); ++a_RelatedObjects_it)
                    {
                        auto aRelatedObject = *a_RelatedObjects_it;
                        IFCSchema::IfcSpace* aSpace = aRelatedObject->as<IFCSchema::IfcSpace>(false);
                        if (aSpace != nullptr)
                        {
                            std::shared_ptr<IFCSpaceModel> aSpaceModel = MyBuilding.GetSpace(aSpace->GlobalId());
                            if (aSpaceModel)
                            {
                                std::cout << "Found Lift IFC Space " << aSpace->Name() << std::endl;
                                FoundSpaces.push_back(aSpaceModel);
                            }
                            auto ItsContainedElements = aSpace->ContainsElements();
                            if (ItsContainedElements)
                            {
                                for (auto a_contanied_element_it = ItsContainedElements->begin(); a_contanied_element_it != ItsContainedElements->end(); ++a_contanied_element_it)
                                {
                                    auto a_contanied_element = *a_contanied_element_it;
                                    IFCSchema::IfcRelContainedInSpatialStructure* theRelatedElements = a_contanied_element->as<IFCSchema::IfcRelContainedInSpatialStructure>(false);
                                    if (theRelatedElements)
                                    {
                                        auto theElements = theRelatedElements->RelatedElements();
                                        for (auto a_element_it = theElements->begin(); a_element_it != theElements->end(); ++a_element_it)
                                        {
                                            auto a_element = *a_element_it;
                                            IFCSchema::IfcTransportElement* aTransportElement = a_element->as<IFCSchema::IfcTransportElement>(false);
                                            if (aTransportElement != nullptr)
                                            {
                                                aElevatorModel = MyBuilding.GetElevator(aTransportElement->GlobalId());
                                                std::cout << "Found Lift IFC Transport Element " << aTransportElement->Name() << std::endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            IFCSchema::IfcTransportElement* aTransportElement = aRelatedObject->as<IFCSchema::IfcTransportElement>(false);
                            if (aTransportElement != nullptr)
                            {
                                std::cout << "Found Lift IFC Transport Element " << aTransportElement->Name() << std::endl;
                            }
                            else
                            {
                                IFCSchema::IfcDoor* aDoor = aRelatedObject->as<IFCSchema::IfcDoor>(false);
                                if (aDoor != nullptr)
                                {
                                    std::cout << "Found Lift IFC Door " << aDoor->Name() << std::endl;
                                }
                            }
                        }
                    }
                }
            }
            if (aElevatorModel != nullptr)
            {
                for (auto aSpace : FoundSpaces)
                {
                    aElevatorModel->AddSpace(aSpace);
                    auto aStory = MyBuilding.GetStorey(aSpace);
                    if (aStory)
                    {
                        // add elevator if not already on that level
                        if (aStory->HasEntry(aElevatorModel) == false)
                        {
                            aStory->AddElevator(aElevatorModel);
                        }
                    }
                }
                return (true);
            }
        }
        return (false);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ExodusIFCGeomUtils::EXODUSIFCExtract(IfcParse::IfcFile &file, IFCBuildingModel &MyBuilding)
{
    IFCSchema::IfcBuildingStorey::list::ptr story_elements = file.instances_by_type<IFCSchema::IfcBuildingStorey>();
    for (auto a_storey_it = story_elements->begin(); a_storey_it != story_elements->end(); ++a_storey_it) {

        const auto* a_storey_element = *a_storey_it;

        std::shared_ptr<IFCStoreyModel> a_storey = std::make_shared<IFCStoreyModel>(a_storey_element->GlobalId());

        ExodusIFCUtils::GetBaseData<IFCSchema::IfcBuildingStorey>(a_storey, a_storey_element);
        MyBuilding.AddStorey(a_storey);

        //get_psets_s<IFCSchema>(a_storey_element);
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_storey_element, "Pset_BuildingStoreyCommon", "ElevationOfFFLRelative", aPropValue))
        {
            std::cout << "Found " << "Pset_BuildingStoreyCommon" << " " << "ElevationOfFFLRelative" << " " << aPropValue << std::endl;
            a_storey->SetElevationOfFFLRelative(toSquareMetres(aPropValue));
        }
        else
        {
            boost::optional< double > anElevation =  a_storey_element->Elevation();
            if (anElevation.is_initialized())
            {
                float aValue = float(anElevation.get());
                a_storey->SetElevationOfFFLRelative(toMetres(aValue));
            }
        }
    }
    IFCSchema::IfcDoor::list::ptr door_elements = file.instances_by_type<IFCSchema::IfcDoor>();
    for (auto a_door_elements_it = door_elements->begin(); a_door_elements_it != door_elements->end(); ++a_door_elements_it)
    {
        auto* a_door_element = *a_door_elements_it;

        std::shared_ptr<IFCDoorModel> a_door = std::make_shared<IFCDoorModel>(a_door_element->GlobalId());

        if (a_door_element->Name().has_value())
        {
            a_door->SetName(a_door_element->Name().get());
        }
        auto OverallWidth = a_door_element->OverallWidth();
        if (OverallWidth.is_initialized())
        {
            a_door->SetOverallWidth(float(OverallWidth.get()));
        }
        std::string aPropValue;
        if (get_psets_s<IFCSchema>(a_door_element, "ePset_OMA_Door", "EffectiveWidth", aPropValue))
        {
            std::cout << "Found " << "ePset_OMA_Door" << " " << "EffectiveWidth" << " " << aPropValue << std::endl;
            a_door->SetEffectiveWidth(toMetres(aPropValue));
        }
        if (get_psets_s<IFCSchema>(a_door_element, "Pset_DoorCommon", "IsExternal", aPropValue))
        {
            std::cout << "Found " << "Pset_DoorCommon" << " " << "IsExternal" << " " << aPropValue << std::endl;
            if (aPropValue.compare("true")==0)
            {
                a_door->SetExternal(true);
            }
        }
        std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_door_element);
        if (a_storey)
        {
            a_storey->AddDoor(a_door);
        }
    }

    // looking for 2nd level spaces (IfcRelSpaceBoundary2ndLevel)
    IFCSchema::IfcSpace::list::ptr space_elements = file.instances_by_type<IFCSchema::IfcSpace>();
    for (auto a_space_it = space_elements->begin(); a_space_it != space_elements->end(); ++a_space_it) {

        auto* a_space_element = *a_space_it;

        std::shared_ptr<IFCSpaceModel> a_space = std::make_shared<IFCSpaceModel>(a_space_element->GlobalId());
        ExodusIFCUtils::GetBaseData<IFCSchema::IfcSpace>(a_space, a_space_element);

        std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_space_element);

        if (a_storey)
        {
            a_storey->AddSpace(a_space);
        }

        IFCSchema::IfcRelSpaceBoundary::list::ptr boundary_elements = a_space_element->BoundedBy();
        if (boundary_elements)
        {
            for (auto a_boundary_elements_it = boundary_elements->begin(); a_boundary_elements_it != boundary_elements->end(); ++a_boundary_elements_it)
            {
                const IFCSchema::IfcRelSpaceBoundary * a_boundary_elements = *a_boundary_elements_it;
                auto aString = a_boundary_elements->Name(); // should be 2ndLevel
                if (aString.has_value())
                {
                    std::cout << "IfcRelSpaceBoundary " << aString.get() << std::endl;
                }
                try
                {
                    auto elem = a_boundary_elements->RelatedBuildingElement();

                    bool Identfied = false;

                    if (Identfied == false)
                    {
                        IFCSchema::IfcDoor *aDoor = elem->as<IFCSchema::IfcDoor>(false);
                        if (aDoor != nullptr)
                        {
                            Identfied = true;
                            std::cout << "The Space " << a_space->GetName() << " Door " << aDoor->Name().get() << std::endl;
                            if (a_storey)
                            {
                                std::shared_ptr<IFCStoreyModel> door_storey = ExodusIFCUtils::CheckStorey(aDoor, &MyBuilding, a_storey);
                                std::shared_ptr<IFCDoorModel> a_door = door_storey->GetDoor(aDoor->GlobalId());
                                if (a_door)
                                {
                                    a_space->AddDoor(a_door);
                                }
                                else
                                {
                                    std::cout << "Door not found on story\n";
                                }
                            }
                        }
                    }
                    if (Identfied == false)
                    {
                        IFCSchema::IfcWall *aWall = elem->as<IFCSchema::IfcWall>(false);
                        if (aWall)
                        {
                            Identfied = true;
                            if (a_storey)
                            {
                                std::shared_ptr<IFCStoreyModel> wall_storey = ExodusIFCUtils::CheckStorey(aWall, &MyBuilding, a_storey);
                                std::string anID = ExodusIFCUtils::GetStoreyGUID(aWall);
                                // Add wall since will need this to generate boundary of building/space
                                std::shared_ptr<IFCWallModel> aWallModel = wall_storey->GetWall(aWall->GlobalId());
                                if (aWallModel == nullptr)
                                {
                                    aWallModel = std::make_shared<IFCWallModel>(aWall->GlobalId());
                                    wall_storey->AddWall(aWallModel);
                                }
                                if (aWallModel)
                                {
                                    a_space->AddWall(aWallModel);
                                }
                            }
                        }
                    }
                    if (Identfied == false)
                    {
                        IFCSchema::IfcColumn *aColumn = elem->as<IFCSchema::IfcColumn>(false);
                        if (aColumn)
                        {
                            Identfied = true;
                            if (a_storey)
                            {
                                std::shared_ptr<IFCStoreyModel> column_storey = ExodusIFCUtils::CheckStorey(aColumn, &MyBuilding, a_storey);
                                // Add column since will need this to generate boundary of building/space
                                std::shared_ptr<IFCColumnModel> aColumnModel = a_storey->GetColumn(aColumn->GlobalId());
                                if (aColumnModel == nullptr)
                                {
                                    aColumnModel = std::make_shared<IFCColumnModel>(aColumn->GlobalId());
                                    column_storey->AddColumn(aColumnModel);
                                }
                                if (aColumnModel)
                                {
                                    a_space->AddColumn(aColumnModel);
                                }
                            }
                        }
                    }
                    if (Identfied == false)
                    {
                        IFCSchema::IfcVirtualElement *aVirtual = elem->as<IFCSchema::IfcVirtualElement>(false);
                        if (aVirtual)
                        {
                            Identfied = true;
                        }
                    }
                    if (Identfied == false)
                    {
                        IFCSchema::IfcSpace *aSpace = elem->as<IFCSchema::IfcSpace>(false);
                        if (aSpace)
                        {
                            Identfied = true;
                        }
                    }
                    if (Identfied == false)
                    {
                        if (elem->Name().is_initialized())
                        {
                            std::cout << "Object type:" << elem->Name().get() << std::endl;
                        }
                    }
                }
                catch (IfcParse::IfcException &/*anError*/)
                {
                    //std::cout << "Space Error " << a_space->GetName() << " " << anError.what() << std::endl;
                }
            }
        }
    }

    IFCSchema::IfcStair::list::ptr stair_elements = file.instances_by_type<IFCSchema::IfcStair>();
    for (auto a_stair_it = stair_elements->begin(); a_stair_it != stair_elements->end(); ++a_stair_it) {

        auto* a_stair_element = *a_stair_it;

        std::shared_ptr<IFCStairModel> a_stair = std::make_shared<IFCStairModel>(a_stair_element->GlobalId());
        GetStairProperties(a_stair_element, a_stair);

        std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_stair_element);
        if (a_storey)
        {
            a_storey->AddStair(a_stair);
        }

        IFCSchema::IfcRelAggregates::list::ptr stair_aggregates = a_stair_element->IsDecomposedBy();
        if (stair_aggregates)
        {
            for (auto a_stair_aggregates_it = stair_aggregates->begin(); a_stair_aggregates_it != stair_aggregates->end(); ++a_stair_aggregates_it)
            {
                const IFCSchema::IfcRelAggregates* a_stair_aggregatea = *a_stair_aggregates_it;
                auto stair_components = a_stair_aggregatea->RelatedObjects();
                if (stair_components)
                {
                    for (auto a_stair_component_it = stair_components->begin(); a_stair_component_it != stair_components->end(); ++a_stair_component_it)
                    {
                        auto a_stair_component = *a_stair_component_it;
                        IFCSchema::IfcStairFlight* a_stairflight_element = a_stair_component->as<IFCSchema::IfcStairFlight>(false);
                        if (a_stairflight_element)
                        {
                            std::shared_ptr<IFCStairFlightModel> a_stairflight = std::make_shared<IFCStairFlightModel>(a_stairflight_element->GlobalId());
                            GetStairProperties(a_stairflight_element, a_stairflight);
                            if (a_stairflight_element->Name().has_value())
                            {
                                a_stairflight->SetName(a_stairflight_element->Name().get());
                            }
                            a_stair->AddStairFlight(a_stairflight);

                            // account for older IFC version saving of stairflight information
                            GetStairflightOldProperties(a_stairflight_element, a_stairflight);
                        }
                        else
                        {
                            IFCSchema::IfcSlab* a_landing_element = a_stair_component->as<IFCSchema::IfcSlab>(false);
                            if (a_landing_element)
                            {
                                std::shared_ptr<IFCLandingModel> a_landing = std::make_shared<IFCLandingModel>(a_landing_element->GlobalId());
                                if (a_landing_element->Name().has_value())
                                {
                                    a_landing->SetName(a_landing_element->Name().get());
                                }
                                a_stair->AddLanding(a_landing);
                            }
                        }
                    }
                }
            }
        }
    }

    // Ramps
    IFCSchema::IfcRamp::list::ptr ramp_elements = file.template instances_by_type<IFCSchema::IfcRamp>();
    for (auto a_ramp_it = ramp_elements->begin(); a_ramp_it != ramp_elements->end(); ++a_ramp_it)
    {
        auto* a_ramp_element = *a_ramp_it;
        std::shared_ptr<IFCRampModel> a_ramp = std::make_shared<IFCRampModel>(a_ramp_element->GlobalId());

        if (a_ramp_element->Name().has_value())
        {
            a_ramp->SetName(a_ramp_element->Name().get());
        }
        std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_ramp_element);
        if (a_storey)
        {
            a_storey->AddRamp(a_ramp);
            GetRampProperties(a_ramp_element, a_ramp);
            IFCSchema::IfcRelAggregates::list::ptr ramp_aggregates = a_ramp_element->IsDecomposedBy();
            if (ramp_aggregates)
            {
                for (auto a_ramp_aggregates_it = ramp_aggregates->begin(); a_ramp_aggregates_it != ramp_aggregates->end(); ++a_ramp_aggregates_it)
                {
                    const IFCSchema::IfcRelAggregates* a_ramp_aggregatea = *a_ramp_aggregates_it;
                    auto ramp_components = a_ramp_aggregatea->RelatedObjects();
                    if (ramp_components)
                    {
                        for (auto a_ramp_component_it = ramp_components->begin(); a_ramp_component_it != ramp_components->end(); ++a_ramp_component_it)
                        {
                            auto a_ramp_component = *a_ramp_component_it;
                            IFCSchema::IfcRampFlight* a_rampflight_element = a_ramp_component->as<IFCSchema::IfcRampFlight>(false);
                            if (a_rampflight_element)
                            {
                                std::shared_ptr<IFCRampFlightModel> a_rampflight = std::make_shared<IFCRampFlightModel>(a_rampflight_element->GlobalId());
                                if (a_rampflight_element->Name().has_value())
                                {
                                    a_rampflight->SetName(a_rampflight_element->Name().get());
                                }
                                a_ramp->AddRampFlight(a_rampflight);
                            }
                            else
                            {
                                IFCSchema::IfcSlab* a_landing_element = a_ramp_component->as<IFCSchema::IfcSlab>(false);
                                if (a_landing_element)
                                {
                                    std::shared_ptr<IFCLandingModel> a_landing = std::make_shared<IFCLandingModel>(a_landing_element->GlobalId());
                                    if (a_landing_element->Name().has_value())
                                    {
                                        a_landing->SetName(a_landing_element->Name().get());
                                    }
                                    a_ramp->AddLanding(a_landing);
                                }
                            }
                        }
                    }
                }
            }
        }
    } 
    
    // Signs
    IFCSchema::IfcSign::list::ptr sign_elements = file.template instances_by_type<IFCSchema::IfcSign>();
    for (auto a_sign_it = sign_elements->begin(); a_sign_it != sign_elements->end(); ++a_sign_it)
    {
        auto* a_sign_element = *a_sign_it;
        std::shared_ptr<IFCSignModel> a_sign = std::make_shared<IFCSignModel>(a_sign_element->GlobalId());

        if (a_sign_element->Name().has_value())
        {
            a_sign->SetName(a_sign_element->Name().get());
        }
        std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_sign_element);
        if (a_storey)
        {
            a_storey->AddSign(a_sign);
            GetSignProperties(a_sign_element, a_sign);
        }
    }

    // Transport Elements : Escaltors, Moving walkways, elevators
    bool ElevatorsIdentfied = false;
    IFCSchema::IfcTransportElement::list::ptr transport_elements = file.template instances_by_type<IFCSchema::IfcTransportElement>();
    for (auto a_transport_it = transport_elements->begin(); a_transport_it != transport_elements->end(); ++a_transport_it)
    {
        auto* a_transport = *a_transport_it;
        boost::optional<IFCSchema::IfcTransportElementTypeEnum::Value> aType = a_transport->PredefinedType();

        switch (aType.get())
        {
        case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_ESCALATOR:
        case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_ELEVATOR:
        case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_MOVINGWALKWAY:
        {
            std::shared_ptr<IFCStoreyModel> a_storey = ExodusIFCUtils::GetIfcElementStorey(&MyBuilding, a_transport);
            if (a_storey)
            {
                switch (aType.get())
                {
                case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_ESCALATOR:
                {
                    std::shared_ptr<IFCEscalatorModel> an_escalator = std::make_shared<IFCEscalatorModel>(a_transport->GlobalId());
                    if (a_transport->Name().has_value())
                    {
                        an_escalator->SetName(a_transport->Name().get());
                    }
                    a_storey->AddEscalator(an_escalator);
                    GetTransportProperties(a_transport, an_escalator);
                    GetEscalatorProperties(a_transport, an_escalator);
                    GetEscalatorMovingWalkwayProperties(a_transport, an_escalator);
                    IFCSchema::IfcProduct *aParent = ExodusIFCUtils::GetParent(a_transport);
                    if (aParent)
                    {
                        IFCSchema::IfcSpace *aSpace = aParent->as<IFCSchema::IfcSpace>(false);
                        if (aSpace)
                        {
                            std::shared_ptr<IFCSpaceModel> a_space = a_storey->GetSpace(aSpace->GlobalId());
                            if (a_space)
                            {
                                a_space->AddEscalator(an_escalator);
                            }
                        }
                    }
                }
                break;
                case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_MOVINGWALKWAY:
                {
                    std::shared_ptr<IFCMovingWalkwayModel> a_movingwalkway = std::make_shared<IFCMovingWalkwayModel>(a_transport->GlobalId());
                    if (a_transport->Name().has_value())
                    {
                        a_movingwalkway->SetName(a_transport->Name().get());
                    }
                    a_storey->AddMovingWalkway(a_movingwalkway);
                    GetTransportProperties(a_transport, a_movingwalkway);
                    GetEscalatorMovingWalkwayProperties(a_transport, a_movingwalkway);
                    IFCSchema::IfcProduct *aParent = ExodusIFCUtils::GetParent(a_transport);
                    if (aParent)
                    {
                        IFCSchema::IfcSpace *aSpace = aParent->as<IFCSchema::IfcSpace>(false);
                        if (aSpace)
                        {
                            std::shared_ptr<IFCSpaceModel> a_space = a_storey->GetSpace(aSpace->GlobalId());
                            if (a_space)
                            {
                                a_space->AddMovingWalkway(a_movingwalkway);
                            }
                        }
                    }
                }
                break;
                case IFCSchema::IfcTransportElementTypeEnum::IfcTransportElementType_ELEVATOR:
                {
                    std::shared_ptr<IFCElevatorModel> a_elevator = std::make_shared<IFCElevatorModel>(a_transport->GlobalId());
                    ElevatorsIdentfied = true;
                    if (a_transport->Name().has_value())
                    {
                        a_elevator->SetName(a_transport->Name().get());
                    }
                    a_storey->AddElevator(a_elevator);
                    GetTransportProperties(a_transport, a_elevator);
                    GetElevatorProperties(a_transport, a_elevator);
                    IFCSchema::IfcProduct *aParent = ExodusIFCUtils::GetParent(a_transport);
                    if (aParent)
                    {
                        IFCSchema::IfcSpace *aSpace = aParent->as<IFCSchema::IfcSpace>(false);
                        if (aSpace)
                        {
                            std::shared_ptr<IFCSpaceModel> a_space = a_storey->GetSpace(aSpace->GlobalId());
                            if (a_space)
                            {
                                a_space->AddElevator(a_elevator);
                            }
                        }
                    }
                }
                break;
                }
            }
        }
        break;
        }
    }
    if (ElevatorsIdentfied)
    {
        IFCSchema::IfcZone::list::ptr zone_elements = file.template instances_by_type<IFCSchema::IfcZone>();
        for (auto a_zone_it = zone_elements->begin(); a_zone_it != zone_elements->end(); ++a_zone_it)
        {
            auto* a_zone_element = *a_zone_it;
            boost::optional< std::string > aType = a_zone_element->ObjectType();
            if (aType.is_initialized())
            {
                if (aType.get().compare("ElevatorShaft") == 0)
                {
                    IFCSchema::IfcRelAssignsToGroup::list::ptr aGroup = a_zone_element->IsGroupedBy();
                    if (aGroup != nullptr)
                    {
                        AddElevatorSpatialData(aGroup, MyBuilding);
                    }
                }
            }
        }
        IFCSchema::IfcBuiltSystem::list::ptr builtsystem_elements = file.template instances_by_type<IFCSchema::IfcBuiltSystem>();
        for (auto a_builtsystem_it = builtsystem_elements->begin(); a_builtsystem_it != builtsystem_elements->end(); ++a_builtsystem_it)
        {
            auto* a_builtsystem_element = *a_builtsystem_it;
            boost::optional<IFCSchema::IfcBuiltSystemTypeEnum::Value> aType = a_builtsystem_element->PredefinedType();
            if (aType.is_initialized())
            {
                if (aType.get()== IFCSchema::IfcBuiltSystemTypeEnum::IfcBuiltSystemType_TRANSPORT)
                {
                    IFCSchema::IfcRelAssignsToGroup::list::ptr aGroup = a_builtsystem_element->IsGroupedBy();
                    if (aGroup != nullptr)
                    {
                        AddElevatorSpatialData(aGroup, MyBuilding);
                    }
                }
            }
        }
        IFCSchema::IfcBuildingSystem::list::ptr buildingsystem_elements = file.template instances_by_type<IFCSchema::IfcBuildingSystem>();
        for (auto a_buildingsystem_it = buildingsystem_elements->begin(); a_buildingsystem_it != buildingsystem_elements->end(); ++a_buildingsystem_it)
        {
            auto* a_buildingsystem_element = *a_buildingsystem_it;
            boost::optional<IFCSchema::IfcBuildingSystemTypeEnum::Value> aType = a_buildingsystem_element->PredefinedType();
            if (aType.is_initialized())
            {
                if (aType.get() == IFCSchema::IfcBuildingSystemTypeEnum::IfcBuildingSystemType_TRANSPORT)
                {
                    IFCSchema::IfcRelAssignsToGroup::list::ptr aGroup = a_buildingsystem_element->IsGroupedBy();
                    if (aGroup != nullptr)
                    {
                        AddElevatorSpatialData(aGroup, MyBuilding);
                    }
                }
            }
        }
    }
    return true;
}

