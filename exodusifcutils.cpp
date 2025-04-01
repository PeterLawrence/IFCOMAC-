/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Disable warnings coming from IfcOpenShell
#pragma warning(disable : 4018 4267 4250 4984 4985 4307)
#include <string>
#include <vector>
#include <memory>
#include "ExodusIFCClasses.h"
#include "exodusifcutils.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ExodusIFCUtils
{
    IFCSchema::IfcProduct* GetParent(IFCSchema::IfcProduct* aProduct)
    {
        if (aProduct)
        {
            IFCSchema::IfcOpeningElement* anOpening = aProduct->as<IFCSchema::IfcOpeningElement>(false);
            if (anOpening)
            {
                IFCSchema::IfcRelVoidsElement::list::ptr ElementList = anOpening->VoidsElements();
                if (ElementList)
                {
                    for (auto a_elements_it = ElementList->begin(); a_elements_it != ElementList->end(); ++a_elements_it)
                    {
                        auto anElement1 = *a_elements_it;
                        if (anElement1)
                        {
                            return anElement1->RelatingBuildingElement();
                        }
                    }
                }
            }
            IFCSchema::IfcElement* anElement1 = aProduct->as<IFCSchema::IfcElement>(false);
            if (anElement1)
            {
                IFCSchema::IfcRelFillsElement::list::ptr FillVoids = anElement1->FillsVoids();
                if (FillVoids)
                {
                    for (auto a_elements_it = FillVoids->begin(); a_elements_it != FillVoids->end(); ++a_elements_it)
                    {
                        auto anElement2 = *a_elements_it;
                        if (anElement2)
                        {
                            return anElement2->RelatingOpeningElement();
                        }
                    }
                }
                IFCSchema::IfcRelContainedInSpatialStructure::list::ptr contains = anElement1->ContainedInStructure();
                if (contains)
                {
                    for (auto a_elements_it = contains->begin(); a_elements_it != contains->end(); ++a_elements_it)
                    {
                        auto anElement3 = *a_elements_it;
                        if (anElement3->RelatingStructure())
                        {
                            IFCSchema::IfcProduct* pProduct = anElement3->RelatingStructure()->as<IFCSchema::IfcProduct>(false);
                            if (pProduct)
                            {
                                return pProduct;
                            }
                        }
                    }
                }
            }
            IFCSchema::IfcObjectDefinition* IfcObjectDefinition = aProduct->as<IFCSchema::IfcObjectDefinition>(false);
            if (IfcObjectDefinition)
            {
                IFCSchema::IfcRelAggregates::list::ptr ObjectDefinitions = IfcObjectDefinition->Decomposes();
                if (ObjectDefinitions)
                {
                    for (auto a_elements_it = ObjectDefinitions->begin(); a_elements_it != ObjectDefinitions->end(); ++a_elements_it)
                    {
                        auto anElement4 = *a_elements_it;
                        if (anElement4->RelatingObject())
                        {
                            IFCSchema::IfcProduct* pProduct = anElement4->RelatingObject()->as<IFCSchema::IfcProduct>(false);
                            if (pProduct)
                            {
                                return pProduct;
                            }
                        }
                    }
                }
            }
        }
        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<IFCStoreyModel> CheckStorey(IFCSchema::IfcProduct* anElement,const IFCBuildingModel* MyBuilding, std::shared_ptr<IFCStoreyModel> currentStory)
    {
        IFCSchema::IfcProduct* aRelatingSpace = GetParent(anElement);
        while (aRelatingSpace)
        {
            if (aRelatingSpace)
            {
                IFCSchema::IfcBuildingStorey* aStory = aRelatingSpace->as<IFCSchema::IfcBuildingStorey>(false);
                if (aStory)
                {
                    return (MyBuilding->GetStorey(aStory->GlobalId()));
                }
                aRelatingSpace = GetParent(aRelatingSpace);
            }
        }
        return currentStory;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string GetStoreyGUID(IFCSchema::IfcProduct* anElement)
    {
        IFCSchema::IfcProduct* aRelatingSpace = GetParent(anElement);
        while (aRelatingSpace)
        {
            if (aRelatingSpace)
            {
                IFCSchema::IfcBuildingStorey* aStory = aRelatingSpace->as<IFCSchema::IfcBuildingStorey>(false);
                if (aStory)
                {
                    return (aStory->GlobalId());
                }
                aRelatingSpace = GetParent(aRelatingSpace);
            }
        }
        return "";
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<IFCStoreyModel> GetIfcElementStorey(const IFCBuildingModel* MyBuilding, IFCSchema::IfcProduct* anElement)
    {
        std::string storeyGUID = GetStoreyGUID(anElement);
        if (storeyGUID.length() > 0)
        {
            return (MyBuilding->GetStorey(storeyGUID));
        }
        return nullptr;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <Eigen/Dense>
using namespace Eigen;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix4d extract_transformation_matrix(IFCSchema::IfcObjectPlacement *placement) {
    // Identity matrix as default
    Matrix4d matrix = Matrix4d::Identity();


    if (placement==nullptr) {
        return matrix;
    }
    IFCSchema::IfcLocalPlacement* local_placement = placement->as<IFCSchema::IfcLocalPlacement>(false);
    if (local_placement == nullptr) {
        return matrix;
    }

    auto relative_placement = local_placement->RelativePlacement();
    IFCSchema::IfcAxis2Placement3D* Axis2Placement3D = relative_placement->as<IFCSchema::IfcAxis2Placement3D>(false);
    if (Axis2Placement3D)
    {
        // Extract location and directions
        IFCSchema::IfcPoint* location = Axis2Placement3D->Location();
        if (location)
        {
            IFCSchema::IfcCartesianPoint* CartesianLocation = location->as<IFCSchema::IfcCartesianPoint>(false);
            if (CartesianLocation)
            {
                auto TheLocation = CartesianLocation->Coordinates();
                Vector3d location_vec(TheLocation.at(0), TheLocation.at(1), TheLocation.at(2));

                auto axis = std::vector<double>{ 0.0, 0.0, 1.0 };
                if (Axis2Placement3D->Axis())
                {
                    axis = Axis2Placement3D->Axis()->DirectionRatios();
                }
                Vector3d z_axis(axis[0], axis[1], axis[2]);

                auto ref_direction_values = std::vector<double>{ 1.0, 0.0, 0.0 };;
                auto ref_direction = Axis2Placement3D->RefDirection();
                if (ref_direction)
                {
                    ref_direction_values = ref_direction->DirectionRatios();
                }
                Vector3d x_axis(ref_direction_values[0], ref_direction_values[1], ref_direction_values[2]);

                Vector3d y_axis = z_axis.cross(x_axis);

                // Build the transformation matrix
                matrix.block<3, 3>(0, 0) << x_axis, y_axis, z_axis;
                matrix.block<3, 1>(0, 3) = location_vec;
            }
        }
    }
    else
    {
        IFCSchema::IfcAxis2Placement2D* Axis2Placement2D = relative_placement->as<IFCSchema::IfcAxis2Placement2D>(false);
        if (Axis2Placement2D)
        {
            // For 2D placement
            IFCSchema::IfcPoint* location = Axis2Placement2D->Location();
            if (location)
            {
                IFCSchema::IfcCartesianPoint* CartesianLocation = location->as<IFCSchema::IfcCartesianPoint>(false);
                if (CartesianLocation)
                {
                    auto TheLocation = CartesianLocation->Coordinates();
                    Vector2d location_vec(TheLocation[0], TheLocation[1]);

                    auto ref_direction_value = std::vector<double>{ 1.0, 0.0 };
                    if (Axis2Placement2D->RefDirection())
                    {
                        ref_direction_value = Axis2Placement2D->RefDirection()->DirectionRatios();
                    }
                    Vector2d x_axis(ref_direction_value[0], ref_direction_value[1]);
                    Vector2d y_axis(-x_axis[1], x_axis[0]); // Perpendicular in 2D
                    matrix.block<2, 2>(0, 0) << x_axis, y_axis;
                    matrix.block<2, 1>(0, 3) = location_vec;
                }
            }
        }
    }
    return matrix;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
Matrix4d compute_global_transform(IFCSchema::IfcObjectPlacement * placement) {
    Matrix4d matrix = extract_transformation_matrix(placement);
    if (placement->PlacementRelTo())
    {
        Matrix4d parent_matrix = compute_global_transform(placement->PlacementRelTo());
        return parent_matrix * matrix; // Combine transformations
    }
    return matrix;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_code(IFCSchema::IfcDoor * anElement)
{
    if (anElement->Name().is_initialized())
    {
        std::cout << "Element ID: " << anElement->id() << " Name: " << anElement->Name().get().c_str() << std::endl;
    }
    else
    {
        std::cout << "Element ID: " << anElement->id() << std::endl;
    }

    auto width = anElement->OverallWidth();
    auto height = anElement->OverallHeight();
    //auto length = anElement->OverallLength();

    if (width.has_value()) std::cout << "Width: " << width.get() << std::endl;
    if (height.has_value()) std::cout << "Height: " << height.get() << std::endl;
    //if (length.has_value()) std::cout << "Length: " << length.get() << std::endl;

    if (anElement->ObjectPlacement())
    {
        auto global_matrix = compute_global_transform(anElement->ObjectPlacement());
        std::cout << "Global Transformation Matrix:\n" << global_matrix << std::endl;
    }
    else
    {
        std::cout << "No placement information available." << std::endl;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void test_code(IFCSchema::IfcTransportElement * anElement)
{
    if (anElement->Name().is_initialized())
    {
        std::cout << "Element ID: " << anElement->id() << " Name: " << anElement->Name().get().c_str() << std::endl;
    }
    else
    {
        std::cout << "Element ID: " << anElement->id() << std::endl;
    }

    if (anElement->ObjectPlacement())
    {
        auto global_matrix = compute_global_transform(anElement->ObjectPlacement());
        std::cout << "Global Transformation Matrix:\n" << global_matrix << std::endl;
    }
    else
    {
        std::cout << "No placement information available." << std::endl;
    }
}
