# tests/CMakeLists.txt
# This file is included if BUILD_TESTS is enabled

enable_testing()

# Add test executable
add_executable(IfcSpacialImportTests
    test_exodus_ifc_utils.cpp
)

# Link test executable with main library and Google Test
target_link_libraries(IfcSpacialImportTests PRIVATE
    IfcSpacialImport
    gtest
    gtest_main
)

# Include directories for test
target_include_directories(IfcSpacialImportTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${IFCOPENSHELL_SRC}"
)

# Set output directory for test executable
set_target_properties(IfcSpacialImportTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<CONFIG>"
)

# Add test to CTest
add_test(
    NAME IfcSpacialImportTests
    COMMAND IfcSpacialImportTests
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests/$<CONFIG>"
)

message(STATUS "Unit tests configured")
message(STATUS "Build tests: cmake --build . --target IfcSpacialImportTests")
message(STATUS "Run tests: cmake --build . --target RUN_TESTS")
