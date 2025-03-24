/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <iomanip>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "exodusIFCClasses.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IFCBuildingModel::GeometryRequired(std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        if (aStory->GetWall(guid))
        {
            return true;
        }
        if (aStory->GetColumn(guid))
        {
            return true;
        }
        if (aStory->GetSpace(guid))
        {
            return true;
        }
        if (aStory->GetDoor(guid))
        {
            return true;
        }
        if (aStory->HasStairPart(guid))
        {
            return true;
        }
        if (aStory->GetEscalator(guid))
        {
            return true;
        }
        if (aStory->GetMovingWalkway(guid))
        {
            return true;
        }
        if (aStory->GetElevator(guid))
        {
            return true;
        }
        if (aStory->GetRamp(guid))
        {
            return true;
        }
        if (aStory->HasRampPart(guid))
        {
            return true;
        }
        if (aStory->GetSign(guid))
        {
            return true;
        }
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<IFCStoreyModel> IFCBuildingModel::GetStorey(std::shared_ptr<IFCRepresentation> anObject) const
{
    for (auto aStory : m_BuildingStories)
    {
        if (aStory->HasEntry(anObject))
        {
            return aStory;
        }
    }
    return (nullptr);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<IFCDoorModel> IFCBuildingModel::GetDoor(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCDoorModel> aDoor = aStory->GetDoor(guid);
        if (aDoor != nullptr)
        {
            return aDoor;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCStairFlightModel> IFCBuildingModel::GetStairFlight(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCStairFlightModel> aStairFlight = aStory->GetStairFlight(guid);
        if (aStairFlight != nullptr)
        {
            return aStairFlight;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//! \brief returns landings for both stairs and ramps
//
std::shared_ptr <IFCLandingModel> IFCBuildingModel::GetLanding(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCLandingModel> aLanding = aStory->GetLanding(guid);
        if (aLanding != nullptr)
        {
            return aLanding;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCTransportElement> IFCBuildingModel::GetTransportElement(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCEscalatorModel> anEscalator = aStory->GetEscalator(guid);
        if (anEscalator != nullptr)
        {
            return anEscalator;
        }
    
        std::shared_ptr<IFCMovingWalkwayModel> aMovingWalkway = aStory->GetMovingWalkway(guid);
        if (aMovingWalkway != nullptr)
        {
            return aMovingWalkway;
        }

        std::shared_ptr<IFCElevatorModel> anElevator = aStory->GetElevator(guid);
        if (anElevator != nullptr)
        {
            return anElevator;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCElevatorModel> IFCBuildingModel::GetElevator(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCElevatorModel> anElevator = aStory->GetElevator(guid);
        if (anElevator != nullptr)
        {
            return anElevator;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCSpaceModel> IFCBuildingModel::GetSpace(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCSpaceModel> aSpace = aStory->GetSpace(guid);
        if (aSpace != nullptr)
        {
            return aSpace;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCRampModel> IFCBuildingModel::GetRamp(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCRampModel> aRamp = aStory->GetRamp(guid);
        if (aRamp != nullptr)
        {
            return aRamp;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCRampFlightModel> IFCBuildingModel::GetRampFlight(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCRampFlightModel> aRampFlight = aStory->GetRampFlight(guid);
        if (aRampFlight != nullptr)
        {
            return aRampFlight;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCSignModel> IFCBuildingModel::GetSign(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCSignModel> aSign = aStory->GetSign(guid);
        if (aSign != nullptr)
        {
            return aSign;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCWallModel> IFCBuildingModel::GetWall(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCWallModel> aWall = aStory->GetWall(guid);
        if (aWall != nullptr)
        {
            return aWall;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr <IFCColumnModel> IFCBuildingModel::GetColumn(const std::string &guid) const
{
    for (auto aStory : m_BuildingStories)
    {
        std::shared_ptr<IFCColumnModel> aColumn = aStory->GetColumn(guid);
        if (aColumn != nullptr)
        {
            return aColumn;
        }
    }
    return nullptr;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IFCBuildingModel::UpdateAssociations()
{
    for (auto aStory : m_BuildingStories)
    {
        aStory->UpdateAssociations();
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
double IFCTriangulation::GetLowerHeight() const
{
    double aMinZ = 0;
    if (m_Verts.size() > 0)
    {
        aMinZ = m_Verts.at(2);
        if (m_Verts.size() > 3)
        {
            for (size_t i = 5; i < m_Verts.size(); i += 3)
            {
                aMinZ = std::min(aMinZ, m_Verts.at(i));
            }
        }
    }
    return aMinZ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
double IFCTriangulation::GetUpperHeight() const
{
    double aMaxZ = 0;
    if (m_Verts.size() > 2)
    {
        aMaxZ = m_Verts.at(2);
        if (m_Verts.size() > 3)
        {
            for (size_t i = 5; i < m_Verts.size(); i += 3)
            {
                aMaxZ = std::max(aMaxZ, m_Verts.at(i));
            }
        }
    }
    return aMaxZ;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
double IFCTriangulation::GetHeight() const
{
    return (GetUpperHeight() - GetLowerHeight());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool  IFCTriangulation::GetPathData(std::vector<IFCPoint> &aPath, double &aWidth) const
{
    if (m_Faces.size() > 0)
    {
        double theTol = 0.01;

        struct ObjLevelData
        {
            double m_LevelHeight = -1;
            std::vector<int> m_lFace;
            ObjLevelData(double aHeight) : m_LevelHeight(aHeight) { }
            void AddFace(int v0, int v1, int v2)
            {
                m_lFace.push_back(v0);
                m_lFace.push_back(v1);
                m_lFace.push_back(v2);
            }

            IFCPoint GetCentre(const std::vector<double> &theVerts) const
            {
                size_t iFace;
                double x = 0.0;
                double y = 0.0;
                double z = 0.0;
                if (m_lFace.size() > 0)
                {
                    for (iFace = 0; iFace < m_lFace.size(); ++iFace)
                    {
                        int v0 = m_lFace.at(iFace);     // not don't need to multiplty by 3 as before
                        //int v1 = m_lFace.at(iFace + 1); // since that was performed when these points 
                        //int v2 = m_lFace.at(iFace + 2); // were added to this class
                        x += theVerts.at(v0 + xp);
                        y += theVerts.at(v0 + yp);
                        z += theVerts.at(v0 + zp);
                    }
                    x = x / double(m_lFace.size());
                    y = y / double(m_lFace.size());
                    z = z / double(m_lFace.size());
                }
                return IFCPoint(x, y, z);
            }
        };

        std::vector<ObjLevelData> m_Levels;
        size_t iFace = 0;
        for (iFace = 0; iFace < m_Faces.size(); iFace += 3)
        {
            int v0 = m_Faces.at(iFace) * 3;
            int v1 = m_Faces.at(iFace + 1) * 3;
            int v2 = m_Faces.at(iFace + 2) * 3;
            if (abs(m_Verts.at(v0 + zp) - m_Verts.at(v1 + zp)) < theTol &&
                abs(m_Verts.at(v0 + zp) - m_Verts.at(v2 + zp)) < theTol &&
                abs(m_Verts.at(v1 + zp) - m_Verts.at(v2 + zp)) < theTol)
            {
                double faceHeight = m_Verts.at(v0 + zp);
                bool AddpendLevel = true;
                if (m_Levels.size() > 0)
                {
                    for (auto &aLevel : m_Levels)
                    {
                        if (abs(aLevel.m_LevelHeight - faceHeight) < theTol)
                        {
                            aLevel.AddFace(v0, v1, v2);
                            AddpendLevel = false;
                            break;
                        }
                    }
                }
                if (AddpendLevel)
                {
                    // add_step_level
                    m_Levels.emplace_back(ObjLevelData(faceHeight));
                    m_Levels.back().AddFace(v0, v1, v2);
                }
            }
        }
        std::sort(std::begin(m_Levels), std::end(m_Levels), [](ObjLevelData &a, ObjLevelData & b) {return a.m_LevelHeight > b.m_LevelHeight; });

        for (auto &aLevel : m_Levels)
        {
            IFCPoint aPoint = aLevel.GetCentre(m_Verts);
            aPath.push_back(aPoint);
        }
        if (aPath.size() > 1)
        {
            IFCPoint Ps = aPath.at(0);
            IFCPoint Pe = aPath.at(1);
            Pe.subtract(Ps);
            auto &aStepData = m_Levels.at(1);
            double max_d = -1.0;
            double min_d = 1.0;
            for (auto aPoint : aStepData.m_lFace)
            {
                double aX = m_Verts.at(aPoint + xp);
                double aY = m_Verts.at(aPoint + yp);
                //double aZ = m_Verts.at(aPoint + zp);
                IFCPoint aP = IFCPoint(aX, aY, 0.0);
                aP.subtract(Ps);
                // calculate maximum distance from path line(p1, p2)
                // -ve or +ve depends on which side of the line the point is on
                double area = double(aP.CrossProduct2D(Pe));
                double base = sqrt(Pe.NormSqrt2D());
                double distance = area / base;
                max_d = std::max(distance, max_d);
                min_d = std::min(distance, min_d);
            }
            // width is the max distance minus the max - ve(i.e.min) distance
            aWidth = max_d - min_d;
        }
    }
    return (aPath.size() > 0);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
double IFCTriangulation::GetFloorArea(double Elev, double aTol)
{
    double LimitMin = Elev - aTol;
    double LimitMax = Elev + aTol;

    double area = 0.0;
    if (m_Faces.size() > 0)
    {
        size_t iFace = 0;
        for (iFace = 0; iFace < m_Faces.size(); iFace += 3)
        {
            int v0 = m_Faces.at(iFace) * 3;
            int v1 = m_Faces.at(iFace + 1) * 3;
            int v2 = m_Faces.at(iFace + 2) * 3;

            if (m_Verts.at(v0 + zp) > LimitMin && m_Verts.at(v0 + zp) < LimitMax &&
                m_Verts.at(v1 + zp) > LimitMin && m_Verts.at(v1 + zp) < LimitMax &&
                m_Verts.at(v2 + zp) > LimitMin && m_Verts.at(v2 + zp) < LimitMax)
            {
                area += abs(0.5*(m_Verts.at(v0 + xp) * (m_Verts.at(v1 + yp) - m_Verts.at(v2 + yp)) +
                    m_Verts.at(v1 + xp) * (m_Verts.at(v2 + yp) - m_Verts.at(v0 + yp)) +
                    m_Verts.at(v2 + xp) * (m_Verts.at(v0 + yp) - m_Verts.at(v1 + yp))));
            }
        }
    }
    return area;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IFCTriangulation::GenerateEdges(face_list &Faces, edge_list &Boundary) const
{
    // Convert to a list of edges
    edge_list Edges;
    for (auto itri : Faces)
    {
        for (int i1 = 0; i1 < 3; ++i1)
        {
            int i2 = (i1 + 1) % 3;
            Edges.push_back({ itri[i1], itri[i2] });
        }
    }

    // Find edges which are defined twice, i.e. defined in two triangles
    size_t edge_count = Edges.size();
    if (edge_count > 0)
    {
        std::vector<size_t> removelist;
        for (size_t ib = 0; ib < (edge_count - 1); ++ib)
        {
            if (std::find(removelist.begin(), removelist.end(), ib) == removelist.end()) // not in
            {
                for (size_t jb = ib + 1; jb < edge_count; ++jb)
                {
                    if (std::find(removelist.begin(), removelist.end(), jb) == removelist.end()) // not in
                    {
                        if ((Edges.at(ib)[0] == Edges.at(jb)[0] && Edges.at(ib)[1] == Edges.at(jb)[1]) ||
                            (Edges.at(ib)[0] == Edges.at(jb)[1] && Edges.at(ib)[1] == Edges.at(jb)[0]))
                        {
                            removelist.push_back(ib);
                            removelist.push_back(jb);
                            break;
                        }

                    }
                }
            }
        }
        // the edges which are not defined twice must be on the boundary
        for (size_t ib = 0; ib < edge_count; ++ib)
        {
            if (std::find(removelist.begin(), removelist.end(), ib) == removelist.end()) // not in
            {
                Boundary.push_back(Edges.at(ib));
            }
        }
    }
    return Boundary.size() > 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//! \brief generates a list of edges of the shape in 2D
//! \param[in] Elev : the level to cut the wall at, absolute value about ground
//! \param[in] aTol : the tolerance used to identify faces at the elev
//! \param[out] EdgePoints : list of edge points
//! \note Elev should be at base of the object (i.e. IfcSpace or IfcWall) to identify triangles on its base
bool IFCTriangulation::GenerateBoundary(double Elev, double aTol, point_list &EdgePoints) const
{
    face_list Faces;
    double LimitMin = Elev - aTol;
    double LimitMax = Elev + aTol;

    int i[3] = { 1,2,3 };

    if (m_Faces.size() > 0)
    {
        size_t iFace = 0;
        for (iFace = 0; iFace < m_Faces.size(); iFace += 3)
        {
            int v0 = m_Faces.at(iFace) * 3;
            int v1 = m_Faces.at(iFace + 1) * 3;
            int v2 = m_Faces.at(iFace + 2) * 3;

            if (m_Verts.at(v0 + zp) > LimitMin && m_Verts.at(v0 + zp) < LimitMax &&
                m_Verts.at(v1 + zp) > LimitMin && m_Verts.at(v1 + zp) < LimitMax &&
                m_Verts.at(v2 + zp) > LimitMin && m_Verts.at(v2 + zp) < LimitMax)
            {
                Faces.push_back({ v0, v1, v2 });
            }
        }
    }
    edge_list Boundary;
    if (GenerateEdges(Faces, Boundary))
    {
        for (auto edge : Boundary)
        {
            EdgePoints.push_back({ m_Verts.at(edge[0] + xp),m_Verts.at(edge[0] + yp), m_Verts.at(edge[0] + zp), m_Verts.at(edge[1] + xp),m_Verts.at(edge[1] + yp), m_Verts.at(edge[1] + zp) });
        }
        return (EdgePoints.size() > 0);
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//! \brief find the distance of a given point to the nearest face on the triangulation
//
double IFCTriangulation::CentreDistOfNearestFaceToPoint(IFCPoint &aPoint) const
{
    double aDist = -1.0;

    if (m_Faces.size() > 0)
    {
        size_t iFace = 0;
        for (iFace = 0; iFace < m_Faces.size(); iFace += 3)
        {
            int v0 = m_Faces.at(iFace) * 3;
            int v1 = m_Faces.at(iFace + 1) * 3;
            int v2 = m_Faces.at(iFace + 2) * 3;

            float MidX = (m_Verts.at(v0 + xp) + m_Verts.at(v1 + xp) + m_Verts.at(v2 + xp)) / 3.0;
            float MidY = (m_Verts.at(v0 + yp) + m_Verts.at(v1 + yp) + m_Verts.at(v2 + yp)) / 3.0;
            float MidZ = (m_Verts.at(v0 + zp) + m_Verts.at(v1 + zp) + m_Verts.at(v2 + zp)) / 3.0;

            double aPointDist = aPoint.PointDistSq(MidX, MidY, MidZ);
            if (aDist < 0.0 || aPointDist < aDist)
            {
                aDist = aPointDist;
            }
        }
        if (aDist > 0.0)
        {
            aDist = sqrt(aDist);
        }
    }
    return aDist;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IFCStoreyModel::UpdateAssociations()
{
    for (auto IfcSpace : m_BuildingSpaces)
    {
        IfcSpace->UpdateAssociations(IfcSpace);
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IFCSpaceModel::UpdateAssociations(std::shared_ptr<IFCSpaceModel> aIFCSpace)
{
    size_t DoorCount = NumberOfDoors();
    for (size_t iDoor = 0; iDoor < DoorCount; ++iDoor)
    {
        auto pDoor = GetDoor(iDoor);
        pDoor->AddSpace(aIFCSpace);
    }
    return true;
}

