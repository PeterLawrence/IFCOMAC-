# Toolchain file for Visual Studio 2017 x64
# Use with CMake configure presets or -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-vs2017-x64.cmake

# Provide a default toolset identifier for MSVC (v141) used by VS2017 builds
set(CMAKE_GENERATOR_TOOLSET "v141" CACHE STRING "MSVC toolset (Visual Studio 2017 -> v141)")

# Default IfcOpenShell root (override with -DIFCOPENSHELL_ROOT)
if(NOT DEFINED IFCOPENSHELL_ROOT)
  set(IFCOPENSHELL_ROOT "${CMAKE_SOURCE_DIR}/../IfcOpenshell" CACHE PATH "Root directory of IfcOpenShell")
endif()

# Default project cache variables
if(NOT DEFINED IFCSPACIALIMPORT_ARCH)
  set(IFCSPACIALIMPORT_ARCH "x64" CACHE STRING "Target architecture: x64 or Win32")
endif()

if(NOT DEFINED IFCSPACIALIMPORT_TOOLSET)
  set(IFCSPACIALIMPORT_TOOLSET "vs2017" CACHE STRING "Visual Studio toolset version")
endif()

# Make sure generator platform is set for multi-config generators
set(CMAKE_GENERATOR_PLATFORM "x64" CACHE STRING "Platform to generate for (x64)")
