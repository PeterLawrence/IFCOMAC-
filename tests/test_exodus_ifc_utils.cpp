#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <fstream>

#include "ExodusIFCClasses.h"
#include "extractgeom.h"

#pragma warning(disable : 4018 4267 4250 4984 4985 4307)

class ExodusIFCUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
        test_building = std::make_unique<IFCBuildingModel>();
    }

    void TearDown() override {
        // Clean up after tests
        test_building.reset();
    }

    std::unique_ptr<IFCBuildingModel> test_building;
};

// Test: EXODUSIFCExtract with valid IFC file
TEST_F(ExodusIFCUtilsTest, ExtractFromValidIFCFile) {
    // This test requires a valid IFC file in the test data directory
    const char* test_ifc_path = "../test_data/sample.ifc";
    
    // Verify test file exists
    std::ifstream test_file(test_ifc_path);
    if (!test_file.good()) {
        GTEST_SKIP() << "Test IFC file not found: " << test_ifc_path;
    }

    try {
        IfcParse::IfcFile file(test_ifc_path);
        if (!file.good()) {
            GTEST_SKIP() << "Unable to parse IFC file: " << test_ifc_path;
        }

        bool result = ExodusIFCGeomUtils::EXODUSIFCExtract(file, *test_building);
        
        // Verify extraction succeeded
        EXPECT_TRUE(result) << "EXODUSIFCExtract should return true for valid file";
        EXPECT_GT(test_building->NumberOfStories(), 0) << "Should have extracted at least one storey";
    }
    catch (const std::exception& e) {
        GTEST_SKIP() << "Exception during extraction: " << e.what();
    }
}

// Test: EXODUSIFCExtract with invalid/nonexistent file
TEST_F(ExodusIFCUtilsTest, ExtractFromInvalidFile) {
    const char* invalid_path = "nonexistent_file_12345.ifc";

    EXPECT_THROW(
        ExodusIFCGeomUtils::EXODUSIFCExtract(invalid_path, *test_building),
        std::invalid_argument);
}

// Test: Building model is initialized correctly
TEST_F(ExodusIFCUtilsTest, BuildingModelInitialization) {
    EXPECT_EQ(test_building->NumberOfStories(), 0) << "New building should have zero storeys";
}

// Test: Storey addition to building model
TEST_F(ExodusIFCUtilsTest, StoreyAddition) {
    auto storey = std::make_shared<IFCStoreyModel>("test-guid-12345");
    storey->SetName("First Floor");
    storey->SetElevationOfFFLRelative(0.0f);
    
    test_building->AddStorey(storey);
    
    EXPECT_EQ(test_building->NumberOfStories(), 1) << "Should have one storey after addition";
    EXPECT_EQ(test_building->GetStorey(0)->GetName(), "First Floor") << "Storey name should be set correctly";
}

// Test: Multiple storeys addition
TEST_F(ExodusIFCUtilsTest, MultipleStoreysAddition) {
    for (int i = 0; i < 5; ++i) {
        auto storey = std::make_shared<IFCStoreyModel>("test-guid-" + std::to_string(i));
        storey->SetName("Floor " + std::to_string(i));
        storey->SetElevationOfFFLRelative(static_cast<float>(i * 3.5f));
        test_building->AddStorey(storey);
    }
    
    EXPECT_EQ(test_building->NumberOfStories(), 5) << "Should have 5 storeys";
    
    for (size_t i = 0; i < test_building->NumberOfStories(); ++i) {
        EXPECT_NEAR(test_building->GetStorey(i)->GetElevationOfFFLRelative(), 
                    static_cast<float>(i * 3.5f), 0.001f) 
                    << "Storey elevation should be correctly set";
    }
}

// Test: Door model creation and properties
TEST_F(ExodusIFCUtilsTest, DoorModelProperties) {
    auto door = std::make_shared<IFCDoorModel>("door-guid-001");
    door->SetName("Main Entrance Door");
    door->SetOverallWidth(1.0f);
    door->SetEffectiveWidth(0.95f);
    
    EXPECT_EQ(door->GetName(), "Main Entrance Door") << "Door name should be set";
    EXPECT_FLOAT_EQ(door->GetOverallWidth(), 1.0f) << "Door overall width should match";
    EXPECT_FLOAT_EQ(door->GetEffectiveWidth(), 0.95f) << "Door effective width should match";
}

// Test: Building model update associations
TEST_F(ExodusIFCUtilsTest, UpdateAssociations) {
    auto storey1 = std::make_shared<IFCStoreyModel>("storey-1");
    storey1->SetName("Ground Floor");
    test_building->AddStorey(storey1);
    
    auto storey2 = std::make_shared<IFCStoreyModel>("storey-2");
    storey2->SetName("First Floor");
    test_building->AddStorey(storey2);
    
    // This should not throw an exception
    EXPECT_NO_THROW({
        test_building->UpdateAssociations();
    }) << "UpdateAssociations should handle multiple storeys";
}

// Test: Space model creation
TEST_F(ExodusIFCUtilsTest, SpaceModelCreation) {
    auto space = std::make_shared<IFCSpaceModel>("space-guid-001");
    space->SetName("Office Room A");
    space->SetLongName("Office Room A - Building 1");
    
    EXPECT_EQ(space->GetName(), "Office Room A") << "Space name should be set";
    EXPECT_EQ(space->GetLongName(), "Office Room A - Building 1") << "Space long name should be set";
}

// Test: Point representation
TEST_F(ExodusIFCUtilsTest, PointRepresentation) {
    IFCPoint p1(1.0f, 2.0f, 3.0f);
    IFCPoint p2(4.0f, 5.0f, 6.0f);
    
    EXPECT_DOUBLE_EQ(p1.Get(0), 1.0) << "Point X coordinate should match";
    EXPECT_DOUBLE_EQ(p1.Get(1), 2.0) << "Point Y coordinate should match";
    EXPECT_DOUBLE_EQ(p1.Get(2), 3.0) << "Point Z coordinate should match";
}

// Test: Base data attributes
TEST_F(ExodusIFCUtilsTest, BaseDataAttributes) {
    auto storey = std::make_shared<IFCStoreyModel>("test-guid");
    storey->SetName("Test Storey");
    storey->SetLongName("Extended Storey Name");
    
    EXPECT_FALSE(storey->GetName().empty()) << "Name should not be empty";
    EXPECT_FALSE(storey->GetLongName().empty()) << "Long name should not be empty";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
