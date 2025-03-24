/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define EXOIfc4x3 1
#define EXOIfc4x3_add1 2

#define ExoIFCSchema EXOIfc4x3

#ifndef ExoIFCSchema
    #error please define ExoIFCSchema
#elif (ExoIFCSchema==EXOIfc4x3_add1)
    #define IFCSchema  Ifc4x3_add1
    #include "../ifcparse/Ifc4x3_add1.h"
#elif (ExoIFCSchema==EXOIfc4x3)
    #define IFCSchema  Ifc4x3
	#include "../ifcparse/Ifc4x3.h"
#else
   #error please ExoIFCSchema unkown
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ExodusIFCUtils
{
    IFCSchema::IfcProduct* GetParent(IFCSchema::IfcProduct* aProduct);
    std::shared_ptr<IFCStoreyModel> CheckStorey(IFCSchema::IfcProduct* anElement, const IFCBuildingModel* MyBuildingm, std::shared_ptr<IFCStoreyModel> currentStory);
    std::string GetStoreyGUID(IFCSchema::IfcProduct* anElement);
    std::shared_ptr<IFCStoreyModel> GetIfcElementStorey(const IFCBuildingModel* MyBuilding, IFCSchema::IfcProduct* anElement);

    template <typename anIFCType>  void GetBaseData(std::shared_ptr<IFCBaseData> a_base_ifc, const anIFCType* an_element)
    {
        if (an_element->Name().has_value())
        {
            std::string aName = an_element->Name().get();
            a_base_ifc->SetName(aName);
        }
        if (an_element->LongName().has_value())
        {
            a_base_ifc->SetLongName(an_element->LongName().get());
        }
    }
}