# apps/CMakeLists.txt
# Test applications for IfcSpacialImport

# Test App 1: Simple DLL Loader (standalone, no library dependencies)
add_executable(IfcSpacialImport_TestApp
    IfcSpacialImport_TestApp.cpp
)

set_target_properties(IfcSpacialImport_TestApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/apps/$<CONFIG>"
)

# Compile options for Windows
if(MSVC)
    target_compile_options(IfcSpacialImport_TestApp PRIVATE /W3 /sdl)
endif()

# Test App 2: Hotel Extractor (uses IfcSpacialImport DLL and classes)
add_executable(IfcSpacialImport_HotelExtractor
    IfcSpacialImport_HotelExtractor.cpp
)

# Link with main library
target_link_libraries(IfcSpacialImport_HotelExtractor PRIVATE
    IfcSpacialImport
)

# Include directories
target_include_directories(IfcSpacialImport_HotelExtractor PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${IFCOPENSHELL_SRC}"
)

set_target_properties(IfcSpacialImport_HotelExtractor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/apps/$<CONFIG>"
)

# Compile options for Windows
if(MSVC)
    target_compile_options(IfcSpacialImport_HotelExtractor PRIVATE /W3 /sdl)
    # Link with Windows libraries
    target_link_libraries(IfcSpacialImport_HotelExtractor PRIVATE
        ws2_32.lib
        kernel32.lib
    )
endif()

message(STATUS "Test applications configured")
message(STATUS "  - IfcSpacialImport_TestApp: Simple DLL loader demo")
message(STATUS "  - IfcSpacialImport_HotelExtractor: Hotel IFC extraction")
message(STATUS "Build: cmake --build . --target IfcSpacialImport_HotelExtractor")
message(STATUS "Run: build\\bin\\apps\\Release\\IfcSpacialImport_HotelExtractor.exe")
