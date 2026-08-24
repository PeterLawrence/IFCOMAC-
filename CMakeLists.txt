cmake_minimum_required(VERSION 3.20)

project(IfcSpacialImport VERSION 1.0 LANGUAGES CXX)

# ============================================================================
# CMAKE CONFIGURATION
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# For Visual Studio, ensure proper architecture selection
if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# ============================================================================
# USER CONFIGURATION - ADJUST THESE PATHS TO YOUR ENVIRONMENT
# ============================================================================

# Base path to IfcOpenShell repository
set(IFCOPENSHELL_ROOT "E:/GitProjects/BIM-IFC/IfcOpenshell" CACHE PATH "Root directory of IfcOpenShell")

# Architecture - x64 (recommended) or Win32
set(IFCSPACIALIMPORT_ARCH "x64" CACHE STRING "Target architecture: x64 or Win32")
set_property(CACHE IFCSPACIALIMPORT_ARCH PROPERTY STRINGS x64 Win32)

# Toolset version - vs2017 or vs2022
set(IFCSPACIALIMPORT_TOOLSET "vs2017" CACHE STRING "Visual Studio toolset version")
set_property(CACHE IFCSPACIALIMPORT_TOOLSET PROPERTY STRINGS vs2017 vs2022)

# ============================================================================
# DERIVED PATHS
# ============================================================================

set(IFCOPENSHELL_SRC "${IFCOPENSHELL_ROOT}/src")
set(IFCOPENSHELL_BUILD "${IFCOPENSHELL_ROOT}/_build-${IFCSPACIALIMPORT_TOOLSET}-${IFCSPACIALIMPORT_ARCH}")
set(IFCOPENSHELL_DEPS "${IFCOPENSHELL_ROOT}/_deps")
set(IFCOPENSHELL_DEPS_INSTALLED "${IFCOPENSHELL_ROOT}/_deps-${IFCSPACIALIMPORT_TOOLSET}-${IFCSPACIALIMPORT_ARCH}-installed")

# ============================================================================
# PROJECT DEFINITION
# ============================================================================

add_library(IfcSpacialImport SHARED
    dllmain.cpp
    exodusifcutils.cpp
    extractgreom.cpp
    extractIFCData.cpp
    getpsets.cpp
    pch.cpp
)

# ============================================================================
# COMPILER SETTINGS
# ============================================================================

# Disable warnings from IfcOpenShell headers
if(MSVC)
    target_compile_options(IfcSpacialImport PRIVATE
        /W3
        /wd4018  # signed/unsigned mismatch
        /wd4267  # conversion from larger to smaller type
        /wd4250  # inherits via dominance
        /wd4984  # if constexpr is a C++17 language extension
        /wd4985  # declaration is not available in conformance mode
        /wd4307  # integral constant overflow
    )
endif()

# C++17 standard
set_target_properties(IfcSpacialImport PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# ============================================================================
# PREPROCESSOR DEFINITIONS
# ============================================================================

target_compile_definitions(IfcSpacialImport PRIVATE
    IFCSPACIALIMPORT_EXPORTS
    _WINDOWS
    _USRDLL
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================

target_include_directories(IfcSpacialImport PRIVATE
    # Current project
    "${CMAKE_CURRENT_SOURCE_DIR}"
    
    # IfcOpenShell
    "${IFCOPENSHELL_SRC}"
    
    # OCCT (OpenCASCADE)
    "${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-7.7.2/inc"
    
    # OpenCOLLADA
    "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/include/opencollada/COLLADABaseUtils"
    "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/include/opencollada/COLLADAStreamWriter"
    "${IFCOPENSHELL_DEPS}/OpenCOLLADA/Externals/LibXML/include"
    
    # Boost
    "${IFCOPENSHELL_DEPS}/boost_1_79_0"
    
    # Other dependencies
    "${IFCOPENSHELL_DEPS_INSTALLED}/json"
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/include"
    "${IFCOPENSHELL_DEPS_INSTALLED}/Eigen"
    "${IFCOPENSHELL_DEPS_INSTALLED}/cgal/include"
    "${IFCOPENSHELL_DEPS_INSTALLED}/mpir"
    "${IFCOPENSHELL_DEPS_INSTALLED}/mpfr"
)

# ============================================================================
# LINK DIRECTORIES
# ============================================================================

# Add both Debug and Release build output directories from IfcOpenShell
link_directories(
    "${IFCOPENSHELL_BUILD}/Debug"
    "${IFCOPENSHELL_BUILD}/Release"
    "${IFCOPENSHELL_DEPS}/boost_1_79_0/stage/${IFCSPACIALIMPORT_TOOLSET}-${IFCSPACIALIMPORT_ARCH}/lib"
)

# ============================================================================
# LIBRARY LINKING
# ============================================================================

# IfcOpenShell Core Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    # IfcOpenShell Parse & Geom
    IfcParse.lib
    IfcGeom.lib
    Serializers.lib
    geometry_serializer.lib
)

# IfcOpenShell Geometry Mapping (all IFC versions)
target_link_libraries(IfcSpacialImport PRIVATE
    geometry_mapping_ifc2x3.lib
    geometry_mapping_ifc4.lib
    geometry_mapping_ifc4x1.lib
    geometry_mapping_ifc4x2.lib
    geometry_mapping_ifc4x3.lib
    geometry_mapping_ifc4x3_tc1.lib
    geometry_mapping_ifc4x3_add1.lib
    geometry_mapping_ifc4x3_add2.lib
)

# IfcOpenShell Serializers (all versions)
target_link_libraries(IfcSpacialImport PRIVATE
    Serializers_ifc2x3.lib
    Serializers_ifc4.lib
    Serializers_ifc4x1.lib
    Serializers_ifc4x2.lib
    Serializers_ifc4x3.lib
    Serializers_ifc4x3_tc1.lib
    Serializers_ifc4x3_add1.lib
    Serializers_ifc4x3_add2.lib
)

# IfcOpenShell Geometry Serializers (all versions)
target_link_libraries(IfcSpacialImport PRIVATE
    geometry_serializer_ifc2x3.lib
    geometry_serializer_ifc4.lib
    geometry_serializer_ifc4x1.lib
    geometry_serializer_ifc4x2.lib
    geometry_serializer_ifc4x3.lib
    geometry_serializer_ifc4x3_tc1.lib
    geometry_serializer_ifc4x3_add1.lib
    geometry_serializer_ifc4x3_add2.lib
)

# IfcOpenShell Geometry Kernels
target_link_libraries(IfcSpacialImport PRIVATE
    geometry_kernel_opencascade.lib
    geometry_kernel_cgal.lib
    geometry_kernel_cgal_simple.lib
    cityjson_converter.lib
)

# OCCT (OpenCASCADE) Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    # Debug versions use 'd' suffix
    $<IF:$<CONFIG:Debug>,
        TKerneld.lib TKMathd.lib TKBRepd.lib TKGeomBased.lib TKGeomAlgod.lib TKG3dd.lib TKG2dd.lib
        TKShHealingd.lib TKTopAlgod.lib TKMeshd.lib TKPrimd.lib TKBoold.lib TKBOd.lib TKFilletd.lib
        TKXSBased.lib TKOffsetd.lib TKHLRd.lib TKBind.lib TKIGESd.lib TKSTEPBased.lib TKSTEPAttrd.lib
        TKSTEP209d.lib TKSTEPd.lib,
        TKernel.lib TKMath.lib TKBRep.lib TKGeomBase.lib TKGeomAlgo.lib TKG3d.lib TKG2d.lib
        TKShHealing.lib TKTopAlgo.lib TKMesh.lib TKPrim.lib TKBool.lib TKBO.lib TKFillet.lib
        TKXSBase.lib TKOffset.lib TKHLR.lib TKBin.lib TKIGES.lib TKSTEPBase.lib TKSTEPAttr.lib
        TKSTEP209.lib TKSTEP.lib
    >
)

# Boost Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    $<IF:$<CONFIG:Debug>,
        libboost_system-vc141-mt-gd-x64-1_79.lib
        libboost_program_options-vc141-mt-gd-x64-1_79.lib
        libboost_regex-vc141-mt-gd-x64-1_79.lib
        libboost_thread-vc141-mt-gd-x64-1_79.lib
        libboost_date_time-vc141-mt-gd-x64-1_79.lib
        libboost_chrono-vc141-mt-gd-x64-1_79.lib
        libboost_atomic-vc141-mt-gd-x64-1_79.lib,
        libboost_system-vc141-mt-x64-1_79.lib
        libboost_program_options-vc141-mt-x64-1_79.lib
        libboost_regex-vc141-mt-x64-1_79.lib
        libboost_thread-vc141-mt-x64-1_79.lib
        libboost_date_time-vc141-mt-x64-1_79.lib
        libboost_chrono-vc141-mt-x64-1_79.lib
        libboost_atomic-vc141-mt-x64-1_79.lib
    >
)

# HDF5 Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/lib/libhdf5_cpp.lib"
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/lib/libhdf5.lib"
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/lib/libzlib.lib"
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/lib/libsz.lib"
    "${IFCOPENSHELL_DEPS_INSTALLED}/HDF5-1.8.22-win64/lib/libaec.lib"
)

# OpenCOLLADA Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    $<IF:$<CONFIG:Debug>,
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/GeneratedSaxParserd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/MathMLSolverd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADABaseUtilsd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADAFrameworkd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADASaxFrameworkLoaderd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADAStreamWriterd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/UTFd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/bufferd.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/ftoad.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/pcred.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/xmld.lib",
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/GeneratedSaxParser.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/MathMLSolver.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADABaseUtils.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADAFramework.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADASaxFrameworkLoader.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/OpenCOLLADAStreamWriter.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/UTF.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/buffer.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/ftoa.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/pcre.lib"
        "${IFCOPENSHELL_DEPS_INSTALLED}/OpenCOLLADA/lib/opencollada/xml.lib"
    >
)

# Arbitrary Precision Math (MPFR, MPIR)
target_link_libraries(IfcSpacialImport PRIVATE
    "${IFCOPENSHELL_DEPS_INSTALLED}/mpfr/mpfr.lib"
    "${IFCOPENSHELL_DEPS_INSTALLED}/mpir/mpir.lib"
)

# OCCT path for linker
link_directories("${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-7.7.2/win64/lib")

# Add explicit LIBPATH for IfcOpenShell build outputs so link.exe finds the .lib files
if(EXISTS "${IFCOPENSHELL_BUILD}/Debug")
    target_link_options(IfcSpacialImport PRIVATE
        $<$<CONFIG:Debug>:/LIBPATH:${IFCOPENSHELL_BUILD}/Debug>
        $<$<CONFIG:Release>:/LIBPATH:${IFCOPENSHELL_BUILD}/Release>
        /LIBPATH:${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-7.7.2/win64/lib
        /LIBPATH:${IFCOPENSHELL_DEPS}/boost_1_79_0/stage/${IFCSPACIALIMPORT_TOOLSET}-${IFCSPACIALIMPORT_ARCH}/lib
    )
else()
    # Fallback to generic build directory
    target_link_options(IfcSpacialImport PRIVATE
        $<$<CONFIG:Debug>:/LIBPATH:${IFCOPENSHELL_BUILD}>
        $<$<CONFIG:Release>:/LIBPATH:${IFCOPENSHELL_BUILD}>
        /LIBPATH:${IFCOPENSHELL_DEPS_INSTALLED}/opencascade-7.7.2/win64/lib
        /LIBPATH:${IFCOPENSHELL_DEPS}/boost_1_79_0/stage/${IFCSPACIALIMPORT_TOOLSET}-${IFCSPACIALIMPORT_ARCH}/lib
    )
endif()

# Windows System Libraries
target_link_libraries(IfcSpacialImport PRIVATE
    ws2_32.lib
    bcrypt.lib
    kernel32.lib
    user32.lib
    gdi32.lib
    winspool.lib
    shell32.lib
    ole32.lib
    oleaut32.lib
    uuid.lib
    comdlg32.lib
    advapi32.lib
)

# ============================================================================
# OUTPUT CONFIGURATION
# ============================================================================

# Set output directory
set_target_properties(IfcSpacialImport PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
)

# DLL Export configuration
set_target_properties(IfcSpacialImport PROPERTIES
    WINDOWS_EXPORT_ALL_SYMBOLS OFF
    PREFIX ""
    SUFFIX ".dll"
)

# ============================================================================
# MSVC-SPECIFIC CONFIGURATION
# ============================================================================

if(MSVC)
    # Enable SDL checks
    target_compile_options(IfcSpacialImport PRIVATE /sdl)
    
    # Conformance mode
    target_compile_options(IfcSpacialImport PRIVATE /permissive-)
    
    # Per-configuration compile and link options (generator-expressions)
    target_compile_options(IfcSpacialImport PRIVATE
        $<$<CONFIG:Debug>:/Od>  # No optimization in Debug
        $<$<CONFIG:Release>:/O2>  # Maximum optimization in Release
    )

    target_link_options(IfcSpacialImport PRIVATE
        $<$<CONFIG:Debug>:/DEBUG>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
    )

    # Subsystem and security options
    target_link_options(IfcSpacialImport PRIVATE /SUBSYSTEM:WINDOWS)
    target_link_options(IfcSpacialImport PRIVATE /NXCOMPAT)

    # Quick-and-dirty: allow multiple definitions to force link completion
    # WARNING: This can hide real symbol conflicts. Use only for short-term debugging.
    target_link_options(IfcSpacialImport PRIVATE /FORCE:MULTIPLE)
endif()

# ============================================================================
# SUMMARY
# ============================================================================

message(STATUS "IfcSpacialImport CMake Configuration")
message(STATUS "  IfcOpenShell Root: ${IFCOPENSHELL_ROOT}")
message(STATUS "  Build Path: ${IFCOPENSHELL_BUILD}")
message(STATUS "  Architecture: ${IFCSPACIALIMPORT_ARCH}")
message(STATUS "  Toolset: ${IFCSPACIALIMPORT_TOOLSET}")
message(STATUS "  Output: ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/IfcSpacialImport.dll")
