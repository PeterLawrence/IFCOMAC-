/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <algorithm>
#include <vector>
#include <array>
#include <tuple>
#include <limits>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCSpaceModel;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCPoint
{
private:
    double m_p[3] = { 0.0, 0.0, 0.0 };
public:
    IFCPoint() {}
    IFCPoint(double x, double y, double z) { m_p[0] = x; m_p[1] = y; m_p[2] = z; }
    double Get(size_t i) const { return m_p[i]; }

    void Set(double x, double y, double z) { m_p[0] = x; m_p[1] = y; m_p[2] = z; }
    void SetToMax()
    {
        // set to max +ve number
        m_p[0] = std::numeric_limits<double>::max();
        m_p[1] = std::numeric_limits<double>::max();
        m_p[2] = std::numeric_limits<double>::max();
    }
    void SetToMin()
    {
        // set to max -ve number
        m_p[0] = -std::numeric_limits<double>::max(); 
        m_p[1] = -std::numeric_limits<double>::max();
        m_p[2] = -std::numeric_limits<double>::max();
    }
    void SetToMax(double x, double y, double z)
    {
        m_p[0] = std::max(x, m_p[0]); m_p[1] = std::max(y, m_p[1]); m_p[2] = std::max(z, m_p[2]);
    }
    void SetToMin(double x, double y, double z)
    {
        m_p[0] = std::min(x, m_p[0]); m_p[1] = std::min(y, m_p[1]); m_p[2] = std::min(z, m_p[2]);
    }
    void add(std::array<double, 6> point_list)
    {
        m_p[0] += point_list[0];
        m_p[1] += point_list[1];
        m_p[2] += point_list[2];
        m_p[0] += point_list[3];
        m_p[1] += point_list[4];
        m_p[2] += point_list[5];
    }
    void add(const IFCPoint &aPoint)
    {
        m_p[0] += aPoint.m_p[0];
        m_p[1] += aPoint.m_p[1];
        m_p[2] += aPoint.m_p[2];
    }
    void subtract(const IFCPoint &aPoint)
    {
        m_p[0] -= aPoint.m_p[0];
        m_p[1] -= aPoint.m_p[1];
        m_p[2] -= aPoint.m_p[2];
    }
    void div(const double aVal)
    {
        m_p[0] = m_p[0] / aVal;
        m_p[1] = m_p[1] / aVal;
        m_p[2] = m_p[2] / aVal;
    }

    double CrossProduct2D(const IFCPoint &p) const { return (m_p[0] * p.m_p[1] - m_p[1] * p.m_p[0]); }
    double DotProduct2D(const IFCPoint &p) const { return (m_p[0] * p.m_p[0] + m_p[1] * p.m_p[1]); }
    double NormSqrt2D() const { return (m_p[0] * m_p[0] + m_p[1] * m_p[1]); }
    double PointDistSq(double aX, double aY, double aZ)const  { return ((aX - m_p[0])*(aX - m_p[0]) + (aY - m_p[1])*(aY - m_p[1]) + (aZ - m_p[1])*(aZ - m_p[1]));     }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCTriangulation
{
public:
    typedef std::vector<std::array<int, 3>> face_list;     //!< Data structure for a face represented as [v0,v1,v2]
    typedef std::vector<std::array<int, 2>> edge_list;     //!< Data structure for an edge represented as [p0,p1]
    typedef std::vector<std::array<double, 6>> point_list; //!< Data structure for an edge represented as [x0,y0,z0,x1,y1,z1]
private:
    std::vector<double> m_Verts; // flat data structure of vertices (x0,y0,z0,x1,y1,z1,...)
    std::vector<int> m_Faces;    // flat data structure of facre (f0_v1,f0_v2,f0_v3,f1_v1,f1_v2,f1_v3,...)

    // relative locations of x,y,z in m_Verts
    static const int xp = 0;
    static const int yp = 1;
    static const int zp = 2;
public:
    IFCTriangulation() { }
    IFCTriangulation(std::vector<double> &theVerts, std::vector<int> &theFaces) : m_Verts(theVerts), m_Faces(theFaces) { }
    void SetTriangulation(std::vector<double> &theVerts, std::vector<int> &theFaces)
    {
        m_Verts = theVerts;
        m_Faces = theFaces;
    }
    void SetTriangulation(const std::vector<double> &theVerts, const std::vector<int> &theFaces)
    {
        m_Verts = theVerts;
        m_Faces = theFaces;
    }

    double GetLowerHeight() const;
    double GetUpperHeight() const;
    double GetHeight() const;
    bool GetPathData(std::vector<IFCPoint> &aPath, double &aWidth) const;

    double GetFloorArea(double Elev, double aTol);
    bool GenerateEdges(face_list &Faces, edge_list &Boundary) const;
    bool GenerateBoundary(double Elev, double aTol, point_list &EdgePoints) const;
    double CentreDistOfNearestFaceToPoint(IFCPoint &aPoint) const;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCBaseData
{
public:
    std::string m_GUID;
    std::string m_Name;
    std::string m_LongName;
public:
    IFCBaseData() { }
    IFCBaseData(std::string aGUID) : m_GUID(aGUID) {}
    std::string GetGUID() const { return (m_GUID); }
    void SetGUID(const std::string &aGUID) { m_GUID = aGUID; }
    bool GUIDMatch(const std::string &aGUID)
    {
        return (m_GUID.compare(aGUID) == 0 ? true : false);
    }

    std::string GetName() const { return (m_Name); }
    void SetName(const std::string &aName) { m_Name = aName; }

    std::string GetLongName() const { return (m_LongName); }
    void SetLongName(const std::string &LongName) { m_LongName = LongName; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCRepresentation : public IFCBaseData
{
private:
    IFCTriangulation m_Triangulation;
    IFCTriangulation::point_list m_BoundaryLines;
public:
    IFCRepresentation() : IFCBaseData() { }
    IFCRepresentation(std::string aGUID) : IFCBaseData(aGUID) {}

    void SetTriangulation(std::vector<double> &theVerts, std::vector<int> &theFaces)
    {
        m_Triangulation.SetTriangulation(theVerts, theFaces);
    }
    void SetTriangulation(const std::vector<double> &theVerts, const std::vector<int> &theFaces)
    {
        m_Triangulation.SetTriangulation(theVerts, theFaces);
    }
    double GetLowerHeight() const
    {
        return (m_Triangulation.GetLowerHeight());
    }
    double GetUpperHeight() const
    {
        return (m_Triangulation.GetUpperHeight());
    }
    double GetFloorArea(double Elev, double aTol)
    {
        return (m_Triangulation.GetFloorArea(Elev,aTol));
    }
    bool GenerateBoundary(double Elev, double aTol) {
        return (m_Triangulation.GenerateBoundary(Elev, aTol, m_BoundaryLines));
    }
    IFCPoint GetCentre() const
    {
        IFCPoint Centre(0.0, 0.0, 0.0);
        if (m_BoundaryLines.size() > 0)
        {
            for (auto Point : m_BoundaryLines)
            {
                Centre.add(Point);
            }
            Centre.div(double(m_BoundaryLines.size() * 2));
        }
        return (Centre);
    }

    double GetHeight() const { return m_Triangulation.GetHeight(); }
    bool GetPathData(std::vector<IFCPoint> &aPath, double &aWidth) const { return m_Triangulation.GetPathData(aPath, aWidth); }

    IFCTriangulation::point_list& GetBoundaryLines() { return m_BoundaryLines; }

    double CentreDistOfNearestFaceToPoint(IFCPoint &aPoint) const {
        return (m_Triangulation.CentreDistOfNearestFaceToPoint(aPoint));
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCLandingModel : public IFCRepresentation
{
public:
    IFCLandingModel() : IFCRepresentation() { }
    IFCLandingModel(std::string aGUID) : IFCRepresentation(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCRampFlightModel : public IFCRepresentation
{
public:
    IFCRampFlightModel() : IFCRepresentation() { }
    IFCRampFlightModel(std::string aGUID) : IFCRepresentation(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCRampModel : public IFCRepresentation
{
private:
    std::vector<std::shared_ptr<IFCRampFlightModel>> m_RampFlights;
    std::vector<std::shared_ptr<IFCLandingModel>> m_Landings;
    double m_Slop = -1.0; // degrees
public:
    IFCRampModel() : IFCRepresentation() { }
    IFCRampModel(std::string aGUID) : IFCRepresentation(aGUID) { }

    double GetSlop() const{ return m_Slop; }
    void SetSlop(double aValue) { m_Slop = aValue; }

    size_t NumberOfRampFlights() const { return m_RampFlights.size(); }
    void AddRampFlight(std::shared_ptr<IFCRampFlightModel> aStairFlight)
    {
        m_RampFlights.push_back(aStairFlight);
    }
    std::shared_ptr<IFCRampFlightModel> GetRampFlight(size_t anIndex) const
    {
        return (m_RampFlights.at(anIndex));
    }
    std::shared_ptr<IFCRampFlightModel> GetRampFlight(const std::string &aGUID) const
    {
        auto is_the_one_rampflight = [aGUID](std::shared_ptr<IFCRampFlightModel> aFlight) { return aFlight->GUIDMatch(aGUID); };

        auto it = std::find_if(m_RampFlights.begin(), m_RampFlights.end(), is_the_one_rampflight);
        if (it != m_RampFlights.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfLandings() const { return m_Landings.size(); }
    void AddLanding(std::shared_ptr<IFCLandingModel> aLanding)
    {
        m_Landings.push_back(aLanding);
    }
    std::shared_ptr<IFCLandingModel> GetLanding(size_t anIndex) const
    {
        return (m_Landings.at(anIndex));
    }
    std::shared_ptr<IFCLandingModel> GetLanding(const std::string &aGUID) const
    {
        auto is_the_one_landing = [aGUID](std::shared_ptr<IFCLandingModel> aFlight) { return aFlight->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Landings.begin(), m_Landings.end(), is_the_one_landing);
        if (it != m_Landings.end())
        {
            return *it;
        }
        return nullptr;
    }

    bool HasRampPart(const std::string &aGUID) const
    {
        if (GetRampFlight(aGUID))
        {
            return true;
        }
        if (GetLanding(aGUID))
        {
            return true;
        }
        return false;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCSignModel : public IFCRepresentation
{
private:
    std::string m_Title;
    double m_LetterHeight = -1.0;
    double m_HeightFromGround = -1.0;
    std::string m_Usage;

    double m_Height = -1.0;
    double m_Width = -1.0;
public:
    IFCSignModel() : IFCRepresentation() { }
    IFCSignModel(std::string aGUID) : IFCRepresentation(aGUID) { }

    std::string GetTitle() const { return (m_Title); }
    void SetTitle(const std::string &aTitle) { m_Title = aTitle; }
    double GetLetterHeight() const { return m_LetterHeight; }
    void SetLetterHeight(double aValue) { m_LetterHeight = aValue; }
    double GetHeightFromGround() const { return m_HeightFromGround; }
    void SetHeightFromGround(double aValue) { m_HeightFromGround = aValue; }
    std::string GeUsage() const { return (m_Usage); }
    void SetUsage(const std::string &aUsage) { m_Usage = aUsage; }
    double GetHeight() const { return m_Height; }
    void SetHeight(double aValue) { m_Height = aValue; }
    double GetWidth() const { return m_Width; }
    void SetWidth(double aValue) { m_Width = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCWallModel : public IFCRepresentation
{
public:
    IFCWallModel() : IFCRepresentation() { }
    IFCWallModel(std::string aGUID) : IFCRepresentation(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCColumnModel : public IFCRepresentation
{
public:
    IFCColumnModel() : IFCRepresentation() { }
    IFCColumnModel(std::string aGUID) : IFCRepresentation(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCStairModelData
{
private:
    float m_RiserHeight = -1.0f;
    int m_NumberOfRisers = -1;
    float m_TreadLength = -1.0f;
    float m_OverallWidth = -1.0f;
    int m_NumberOfTreads = -1;
    float m_NosingLength = -1.0f;
    float m_EffectiveWidth = -1.0f;
    float m_RunHeight = -1.0f;
public:
    float GetRiserHeight() const { return (m_RiserHeight); }
    void SetRiserHeight(float aValue) { m_RiserHeight = aValue; }

    int GetNumberOfRisers() const { return (m_NumberOfRisers); }
    void SetNumberOfRisers(int aValue) { m_NumberOfRisers = aValue; }

    float GetTreadLength() const { return (m_TreadLength); }
    void SetTreadLength(float aValue) { m_TreadLength = aValue; }

    int GetNumberOfTreads() const { return (m_NumberOfTreads); }
    void SetNumberOfTreads(int aValue) { m_NumberOfTreads = aValue; }

    float GetOverallWidth() const { return (m_OverallWidth); }
    void SetOverallWidth(float aValue) { m_OverallWidth = aValue; }
    
    float GetNosingLength() const { return (m_NosingLength); }
    void SetNosingLength(float aValue) { m_NosingLength = aValue; }

    float GetEffectiveWidth() const { return (m_EffectiveWidth); }
    void SetEffectiveWidth(float aValue) { m_EffectiveWidth = aValue; }

    float GetRunHeight() const { return (m_RunHeight); }
    void SetRunHeight(float aValue) { m_RunHeight = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCStairFlightModel : public IFCStairModelData, public IFCRepresentation
{
public:
    IFCStairFlightModel() : IFCRepresentation() { }
    IFCStairFlightModel(std::string aGUID) : IFCRepresentation(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCStairModel : public IFCStairModelData, public IFCRepresentation
{
private:
    std::vector<std::shared_ptr<IFCStairFlightModel>> m_StairFlights;
    std::vector<std::shared_ptr<IFCLandingModel>> m_Landings;
public:
    IFCStairModel() : IFCRepresentation() { }
    IFCStairModel(std::string aGUID) : IFCRepresentation(aGUID) { }

    size_t NumberOfStairFlights() const { return m_StairFlights.size(); }
    void AddStairFlight(std::shared_ptr<IFCStairFlightModel> aStairFlight)
    {
        m_StairFlights.push_back(aStairFlight);
    }
    std::shared_ptr<IFCStairFlightModel> GetStairFlight(size_t anIndex) const
    {
        return (m_StairFlights.at(anIndex));
    }
    std::shared_ptr<IFCStairFlightModel> GetStairFlight(const std::string &aGUID) const
    {
        auto is_the_one_stairflight = [aGUID](std::shared_ptr<IFCStairFlightModel> aFlight) { return aFlight->GUIDMatch(aGUID); };

        auto it = std::find_if(m_StairFlights.begin(), m_StairFlights.end(), is_the_one_stairflight);
        if (it != m_StairFlights.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfLandings() const { return m_Landings.size(); }
    void AddLanding(std::shared_ptr<IFCLandingModel> aLanding)
    {
        m_Landings.push_back(aLanding);
    }
    std::shared_ptr<IFCLandingModel> GetLanding(size_t anIndex) const
    {
        return (m_Landings.at(anIndex));
    }
    std::shared_ptr<IFCLandingModel> GetLanding(const std::string &aGUID) const
    {
        auto is_the_one_landing = [aGUID](std::shared_ptr<IFCLandingModel> aFlight) { return aFlight->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Landings.begin(), m_Landings.end(), is_the_one_landing);
        if (it != m_Landings.end())
        {
            return *it;
        }
        return nullptr;
    }

    bool HasStairPart(const std::string &aGUID) const
    {
        if (GetStairFlight(aGUID))
        {
            return true;
        }
        if (GetLanding(aGUID))
        {
            return true;
        }
        return false;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//! \brief the spaces which reference an ifc elment
//
class IFCSpaceReference 
{
private:
    std::vector<std::shared_ptr<IFCSpaceModel>> m_BuildingSpaces; // list of IfcSpaces which reference this space
public:
    void ClearSpaceReferences() { m_BuildingSpaces.clear(); }
    size_t NumberOfSpaces() const { return(m_BuildingSpaces.size()); }
    void AddSpace(std::shared_ptr<IFCSpaceModel> aSpace)
    {
        m_BuildingSpaces.push_back(aSpace);
    }
    std::shared_ptr<IFCSpaceModel> GetSpace(size_t anIndex) const
    {
        return (m_BuildingSpaces.at(anIndex));
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCDoorModel : public IFCSpaceReference, public IFCRepresentation
{
private:
    bool m_isExternal = false;
    float m_EffectiveWidth = 0.0f;
    float m_OverallWidth = 0.0f;
public:
    IFCDoorModel() : IFCRepresentation() { }
    IFCDoorModel(std::string aGUID) : IFCRepresentation(aGUID) { }

    bool isExternal() const { return (m_isExternal); }
    void SetExternal(bool aStatus) { m_isExternal = aStatus; }
    float GetEffectiveWidth() const { return (m_EffectiveWidth); }
    void SetEffectiveWidth(float aValue) { m_EffectiveWidth = aValue; }
    float GetOverallWidth() const { return (m_OverallWidth); }
    void SetOverallWidth(float aValue) { m_OverallWidth = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCTransportElement : public IFCRepresentation
{
private:
    float m_LoadingArea = -1.0f;
    float m_MaximumConstantSpeed = -1.0f;
public:
    IFCTransportElement() : IFCRepresentation() { }
    IFCTransportElement(std::string aGUID) : IFCRepresentation(aGUID) { }

    float GetLoadingArea() const { return (m_LoadingArea); }
    void SetLoadingArea(float aValue) { m_LoadingArea = aValue; }
    float GetMaximumConstantSpeed() const { return (m_MaximumConstantSpeed); }
    void SetMaximumConstantSpeed(float aValue) { m_MaximumConstantSpeed = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCConveyorModel : public IFCTransportElement
{
private:
    float m_TreadLength = -1.0f;
    float m_RunLength = -1.0f;
    float m_Length = -1.0f;
    float m_ClearWidth = -1.0f;

public:
    IFCConveyorModel() : IFCTransportElement() { }
    IFCConveyorModel(std::string aGUID) : IFCTransportElement(aGUID) { }

    float GetTreadLength() const { return (m_TreadLength); }
    void SetTreadLength(float aValue) { m_TreadLength = aValue; }
    float GetRunLength() const { return (m_RunLength); }
    void SetRunLength(float aValue) { m_RunLength = aValue; }
    float GetLength() const { return (m_Length); }
    void SetLength(float aValue) { m_Length = aValue; }
    float GetClearWidth() const { return (m_ClearWidth); }
    void SetClearWidth(float aValue) { m_ClearWidth = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCEscalatorModel : public IFCConveyorModel
{
private:
    float m_RiserHeight = -1.0f;
    float m_RunHeight = -1.0f;
public:
    IFCEscalatorModel() : IFCConveyorModel() { }
    IFCEscalatorModel(std::string aGUID) : IFCConveyorModel(aGUID) { }

    float GetRiserHeight() const { return (m_RiserHeight); }
    void SetRiserHeight(float aValue) { m_RiserHeight = aValue; }
    float GetRunHeight() const { return (m_RunHeight); }
    void SetRunHeight(float aValue) { m_RunHeight = aValue; }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCMovingWalkwayModel : public IFCConveyorModel
{
public:
    IFCMovingWalkwayModel() : IFCConveyorModel() { }
    IFCMovingWalkwayModel(std::string aGUID) : IFCConveyorModel(aGUID) { }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCElevatorModel : public IFCSpaceReference, public IFCTransportElement
{
private:
    int m_Capacity = 0;
    float m_ClearDepth = -1.0f;
    float m_ClearWidth = -1.0f;
    float m_MotorDelayTime = -1.0f;
    float m_AccelerationRate = -1.0f;
    float m_DecelerationRate = -1.0f;
    float m_AdvancedDoorOpeningTime = -1.0f;
    float m_DoorOpeningTime = -1.0f;
    float m_DoorClosingTime = -1.0f;
    float m_DoorDwellTime = -1.0f;
    float m_LevellingDelayTime = -1.0f;
    float m_JerkRate = -1.0f;
    
public:
    IFCElevatorModel() : IFCTransportElement() { }
    IFCElevatorModel(std::string aGUID) : IFCTransportElement(aGUID) { }

    int GetCapacity() const { return (m_Capacity); }
    void SetCapacity(int aValue) { m_Capacity = aValue; }
    float ClearDepth() const { return (m_ClearDepth); }
    void ClearDepth(float aValue) { m_ClearDepth = aValue; }
    float GetClearWidth() const { return (m_ClearWidth); }
    void SetClearWidth(float aValue) { m_ClearWidth = aValue; }
    float GetMotorDelayTime() const { return (m_MotorDelayTime); }
    void SetMotorDelayTime(float aValue) {  m_MotorDelayTime = aValue; }
    float GetAccelerationRate() const { return (m_AccelerationRate); }
    void SetAccelerationRate(float aValue) {  m_AccelerationRate = aValue; }
    float GetDecelerationRate() const { return (m_DecelerationRate); }
    void SetDecelerationRate(float aValue) {  m_DecelerationRate = aValue; }
    float GetAdvancedDoorOpeningTime() const { return (m_AdvancedDoorOpeningTime); }
    void SetAdvancedDoorOpeningTime(float aValue) {  m_AdvancedDoorOpeningTime = aValue; }
    float GetDoorOpeningTime() const { return (m_DoorOpeningTime); }
    void SetDoorOpeningTime(float aValue) {  m_DoorOpeningTime = aValue; }
    float GetDoorClosingTime() const { return (m_DoorClosingTime); }
    void SetDoorClosingTime(float aValue) {  m_DoorClosingTime = aValue; }
    float GetDoorDwellTime() const { return (m_DoorDwellTime); }
    void SetDoorDwellTime(float aValue) {  m_DoorDwellTime = aValue; }
    float GetLevellingDelayTime() const { return (m_LevellingDelayTime); }
    void SetLevellingDelayTime(float aValue) {  m_LevellingDelayTime= aValue; }
    float GetJerkRate() const { return ( m_JerkRate); }
    void SetJerkRate(float aValue) {  m_JerkRate=  aValue; }
    
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCBuildingRelationships
{
private:
    std::vector<std::shared_ptr<IFCDoorModel>> m_BuildingDoors;
    std::vector<std::shared_ptr<IFCStairModel>> m_Stairs;
    std::vector<std::shared_ptr<IFCEscalatorModel>> m_Escalators;
    std::vector<std::shared_ptr<IFCMovingWalkwayModel>> m_MovingWalkways;
    std::vector<std::shared_ptr<IFCElevatorModel>> m_Elevators;
    std::vector<std::shared_ptr<IFCRampModel>> m_Ramps;
    std::vector<std::shared_ptr<IFCSignModel>> m_Signs;

private:
    template<typename aType>
    bool FindRepresentation(const std::vector<std::shared_ptr<aType>> &aVector, std::shared_ptr<IFCRepresentation> anObject) const
    {
        auto is_the_one = [anObject](std::shared_ptr<IFCRepresentation> aRep) { return (aRep.get() == anObject.get()); };

        auto it = std::find_if(aVector.begin(), aVector.end(), is_the_one);
        if (it != aVector.end())
        {
            return true;
        }
        return false;
    }
public:
    bool HasEntry(std::shared_ptr<IFCRepresentation> anObject) const
    {
        if (FindRepresentation<IFCDoorModel>(m_BuildingDoors, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCStairModel>(m_Stairs, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCEscalatorModel>(m_Escalators, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCMovingWalkwayModel>(m_MovingWalkways, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCElevatorModel>(m_Elevators, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCRampModel>(m_Ramps, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCSignModel>(m_Signs, anObject))
        {
            return true;
        }
        return false;
    }
    size_t NumberOfDoors() const { return(m_BuildingDoors.size()); }
    void AddDoor(std::shared_ptr<IFCDoorModel> aDoor)
    {
        m_BuildingDoors.push_back(aDoor);
    }
    std::shared_ptr<IFCDoorModel> GetDoor(size_t anIndex) const
    {
        return (m_BuildingDoors.at(anIndex));
    }
    std::shared_ptr<IFCDoorModel> GetDoor(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCDoorModel> aDoor) { return aDoor->GUIDMatch(aGUID); };

        auto it = std::find_if(m_BuildingDoors.begin(), m_BuildingDoors.end(), is_the_one);
        if (it != m_BuildingDoors.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfStairs() const { return m_Stairs.size(); }
    void AddStair(std::shared_ptr<IFCStairModel> aStair)
    {
        m_Stairs.push_back(aStair);
    }
    std::shared_ptr<IFCStairModel> GetStair(size_t anIndex) const
    {
        return (m_Stairs.at(anIndex));
    }
    bool HasStairPart(const std::string &aGUID) const
    {
        for (auto aStair : m_Stairs)
        {
            if (aStair->HasStairPart(aGUID))
            {
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<IFCStairFlightModel> GetStairFlight(const std::string &guid) const
    {
        for (auto aStair : m_Stairs)
        {
            std::shared_ptr<IFCStairFlightModel> aStairFlight = aStair->GetStairFlight(guid);
            if (aStairFlight!=nullptr)
            {
                return aStairFlight;
            }
        }
        return nullptr;
    }
    //! \brief returns landings for both stairs and ramps
    std::shared_ptr<IFCLandingModel> GetLanding(const std::string &guid) const
    {
        for (auto aStair : m_Stairs)
        {
            std::shared_ptr<IFCLandingModel> aLanding = aStair->GetLanding(guid);
            if (aLanding != nullptr)
            {
                return aLanding;
            }
        }
        for (auto aRamp : m_Ramps)
        {
            std::shared_ptr<IFCLandingModel> aLanding = aRamp->GetLanding(guid);
            if (aLanding != nullptr)
            {
                return aLanding;
            }
        }
        return nullptr;
    }
    
    size_t NumberOfEscalators() const { return m_Escalators.size(); }
    void AddEscalator(std::shared_ptr<IFCEscalatorModel> anEscalator)
    {
        m_Escalators.push_back(anEscalator);
    }
    std::shared_ptr<IFCEscalatorModel> GetEscalator(size_t anIndex) const
    {
        return (m_Escalators.at(anIndex));
    }
    std::shared_ptr<IFCEscalatorModel> GetEscalator(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCEscalatorModel> anEscalator) { return anEscalator->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Escalators.begin(), m_Escalators.end(), is_the_one);
        if (it != m_Escalators.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfMovingWalkways() const { return m_MovingWalkways.size(); }
    void AddMovingWalkway(std::shared_ptr<IFCMovingWalkwayModel> anEscalator)
    {
        m_MovingWalkways.push_back(anEscalator);
    }
    std::shared_ptr<IFCMovingWalkwayModel> GetMovingWalkway(size_t anIndex) const
    {
        return (m_MovingWalkways.at(anIndex));
    }
    std::shared_ptr<IFCMovingWalkwayModel> GetMovingWalkway(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCMovingWalkwayModel> aMovingWalkway) { return aMovingWalkway->GUIDMatch(aGUID); };

        auto it = std::find_if(m_MovingWalkways.begin(), m_MovingWalkways.end(), is_the_one);
        if (it != m_MovingWalkways.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfElevators() const { return m_Elevators.size(); }
    void AddElevator(std::shared_ptr<IFCElevatorModel> anEscalator)
    {
        m_Elevators.push_back(anEscalator);
    }
    std::shared_ptr<IFCElevatorModel> GetElevator(size_t anIndex) const
    {
        return (m_Elevators.at(anIndex));
    }
    std::shared_ptr<IFCElevatorModel> GetElevator(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCElevatorModel> aElevator) { return aElevator->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Elevators.begin(), m_Elevators.end(), is_the_one);
        if (it != m_Elevators.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfRamps() const { return m_Ramps.size(); }
    void AddRamp(std::shared_ptr<IFCRampModel> aRamp)
    {
        m_Ramps.push_back(aRamp);
    }
    std::shared_ptr<IFCRampModel> GetRamp(size_t anIndex) const
    {
        return (m_Ramps.at(anIndex));
    }
    std::shared_ptr<IFCRampModel> GetRamp(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCRampModel> aRamp) { return aRamp->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Ramps.begin(), m_Ramps.end(), is_the_one);
        if (it != m_Ramps.end())
        {
            return *it;
        }
        return nullptr;
    }
    bool HasRampPart(const std::string &aGUID) const
    {
        for (auto aRamp : m_Ramps)
        {
            if (aRamp->HasRampPart(aGUID))
            {
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<IFCRampFlightModel> GetRampFlight(const std::string &guid) const
    {
        for (auto aRamp : m_Ramps)
        {
            std::shared_ptr<IFCRampFlightModel> aRampFlight = aRamp->GetRampFlight(guid);
            if (aRampFlight != nullptr)
            {
                return aRampFlight;
            }
        }
        return nullptr;
    }

    size_t NumberOfSigns() const { return m_Signs.size(); }
    void AddSign(std::shared_ptr<IFCSignModel> aSign)
    {
        m_Signs.push_back(aSign);
    }
    std::shared_ptr<IFCSignModel> GetSign(size_t anIndex) const
    {
        return (m_Signs.at(anIndex));
    }
    std::shared_ptr<IFCSignModel> GetSign(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCSignModel> aSign) { return aSign->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Signs.begin(), m_Signs.end(), is_the_one);
        if (it != m_Signs.end())
        {
            return *it;
        }
        return nullptr;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BoundaryEdges
{
private:
    std::vector<std::shared_ptr<IFCWallModel>> m_Walls;
    std::vector<std::shared_ptr<IFCColumnModel>> m_Collumns;
    template<typename aType>
    bool FindRepresentation(const std::vector<std::shared_ptr<aType>> &aVector, std::shared_ptr<IFCRepresentation> anObject) const
    {
        auto is_the_one = [anObject](std::shared_ptr<IFCRepresentation> aRep) { return (aRep.get() == anObject.get()); };

        auto it = std::find_if(aVector.begin(), aVector.end(), is_the_one);
        if (it != aVector.end())
        {
            return true;
        }
        return false;
    }
public:
    size_t NumberOfWalls() const { return(m_Walls.size()); }
    void AddWall(std::shared_ptr<IFCWallModel> aWall)
    {
        m_Walls.push_back(aWall);
    }
    std::shared_ptr<IFCWallModel> GetWall(size_t anIndex) const
    {
        return (m_Walls.at(anIndex));
    }
    std::shared_ptr<IFCWallModel> GetWall(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCWallModel> aWall) { return aWall->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Walls.begin(), m_Walls.end(), is_the_one);
        if (it != m_Walls.end())
        {
            return *it;
        }
        return nullptr;
    }

    size_t NumberOfColumns() const { return(m_Collumns.size()); }
    void AddColumn(std::shared_ptr<IFCColumnModel> aCollumn)
    {
        m_Collumns.push_back(aCollumn);
    }
    std::shared_ptr<IFCColumnModel> GetColumn(size_t anIndex) const
    {
        return (m_Collumns.at(anIndex));
    }
    std::shared_ptr<IFCColumnModel> GetColumn(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCColumnModel> aCollumn) { return aCollumn->GUIDMatch(aGUID); };

        auto it = std::find_if(m_Collumns.begin(), m_Collumns.end(), is_the_one);
        if (it != m_Collumns.end())
        {
            return *it;
        }
        return nullptr;
    }
    bool HasEntry(std::shared_ptr<IFCRepresentation> anObject) const
    {
        if (FindRepresentation<IFCWallModel>(m_Walls, anObject))
        {
            return true;
        }
        if (FindRepresentation<IFCColumnModel>(m_Collumns, anObject))
        {
            return true;
        }
        return false;
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCSpaceModel : std::enable_shared_from_this<IFCSpaceModel>, public IFCRepresentation, public IFCBuildingRelationships, public BoundaryEdges
{
public:
    IFCSpaceModel() : IFCRepresentation() { }
    IFCSpaceModel(std::string aGUID) : IFCRepresentation(aGUID) { }

    bool UpdateAssociations(std::shared_ptr<IFCSpaceModel> aIFCSpace);
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCStoreyModel : public IFCBaseData, public IFCBuildingRelationships, public BoundaryEdges
{
public:
    std::vector<std::shared_ptr<IFCSpaceModel>> m_BuildingSpaces;
    float m_ElevationOfFFLRelative = 0.0f; //!< Elevation of the top surface of the finished floor level given in elevation above the local zero height.
public:
    IFCStoreyModel() : IFCBaseData() { }
    IFCStoreyModel(std::string aGUID) : IFCBaseData(aGUID) { }

    bool UpdateAssociations();

    float GetElevationOfFFLRelative() const { return (m_ElevationOfFFLRelative); }
    void SetElevationOfFFLRelative(float aElevationOfFFLRelative) { m_ElevationOfFFLRelative = aElevationOfFFLRelative; }

    size_t NumberOfSpaces() const { return(m_BuildingSpaces.size()); }
    void AddSpace(std::shared_ptr<IFCSpaceModel> aSpace)
    {
        m_BuildingSpaces.push_back(aSpace);
    }
    std::shared_ptr<IFCSpaceModel> GetSpace(size_t anIndex) const
    {
        return (m_BuildingSpaces.at(anIndex));
    }
    std::shared_ptr<IFCSpaceModel> GetSpace(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCSpaceModel> aSpace) { return aSpace->GUIDMatch(aGUID); };

        auto it = std::find_if(m_BuildingSpaces.begin(), m_BuildingSpaces.end(), is_the_one);
        if (it != m_BuildingSpaces.end())
        {
            return *it;
        }
        return nullptr;
    }
    bool HasEntry(std::shared_ptr<IFCRepresentation> anObject) const
    {
        auto is_the_one = [anObject](std::shared_ptr<IFCRepresentation> aRep) { return (aRep.get() == anObject.get()); };

        auto it = std::find_if(m_BuildingSpaces.begin(), m_BuildingSpaces.end(), is_the_one);
        if (it != m_BuildingSpaces.end())
        {
            return true;
        }
        if (BoundaryEdges::HasEntry(anObject))
        {
            return true;
        }
        return (IFCBuildingRelationships::HasEntry(anObject));
    }
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IFCBuildingModel
{
private:
	std::vector<std::shared_ptr<IFCStoreyModel>> m_BuildingStories;
    IFCPoint m_MinCoordinate, m_MaxCoordinate;
public:
    IFCPoint &GetMinCoordinate()  { return m_MinCoordinate; }
    IFCPoint &GetMaxCoordinate()  { return m_MaxCoordinate; }
    void SetMinCoordinate(IFCPoint aVal) { m_MinCoordinate = aVal; }
    void SetMaxCoordinate(IFCPoint aVal) { m_MaxCoordinate = aVal; }

    bool UpdateAssociations();

    bool GeometryRequired(std::string &guid) const;
    std::shared_ptr<IFCStoreyModel> GetStorey(std::shared_ptr<IFCRepresentation> anObject) const;

	size_t NumberOfStories() const { return(m_BuildingStories.size()); }
    void AddStorey(std::shared_ptr<IFCStoreyModel> aStorey)
    {
        m_BuildingStories.push_back(aStorey);
    }
    std::shared_ptr<IFCStoreyModel> GetStorey(size_t anIndex) const
    {
        return (m_BuildingStories.at(anIndex));
    }
    std::shared_ptr<IFCStoreyModel> GetStorey(const std::string &aGUID) const
    {
        auto is_the_one = [aGUID](std::shared_ptr<IFCStoreyModel> aStorey) { return aStorey->GUIDMatch(aGUID); };

        auto it = std::find_if(m_BuildingStories.begin(), m_BuildingStories.end(), is_the_one);
        if (it != m_BuildingStories.end())
        {
            return *it;
        }
        return nullptr;
    }

    std::shared_ptr<IFCDoorModel> GetDoor(const std::string &guid) const;
    std::shared_ptr <IFCStairFlightModel> GetStairFlight(const std::string &guid) const;
    std::shared_ptr <IFCLandingModel> GetLanding(const std::string &guid) const;
    std::shared_ptr <IFCTransportElement> GetTransportElement(const std::string &guid) const;
    std::shared_ptr <IFCElevatorModel> GetElevator(const std::string &guid) const;
    std::shared_ptr <IFCRampModel> GetRamp(const std::string &guid) const;
    std::shared_ptr <IFCRampFlightModel> GetRampFlight(const std::string &guid) const;
    std::shared_ptr <IFCSignModel> GetSign(const std::string &guid) const;

    std::shared_ptr <IFCSpaceModel> GetSpace(const std::string &guid) const;

    std::shared_ptr <IFCWallModel> GetWall(const std::string &guid) const;
    std::shared_ptr <IFCColumnModel> GetColumn(const std::string &guid) const;
};