#=============================================================================#
# Project setup.
#=============================================================================#
project(bsoid)

cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0054 NEW)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# We do not support Apple, so disable it from the get-go.
#if (APPLE)
#    message(FATAL_ERROR "Bsoid only supports Windows and Linux.")
#endif()

# Any compile-time options go here.
option(BSOID_BUILD_DOCS "Build Bsoid documentation" ON)
option(BSOID_GUI "Enable GUI for polygonizer" ON)

# Set the version data.
set(BSOID_VERSION_MAJOR "0")
set(BSOID_VERSION_MINOR "0")
set(BSOID_VERSION_PATCH "0")
set(BSOID_VERSION_EXTRA "")
set(BSOID_VERSION "${BSOID_VERSION_MAJOR}.${BSOID_VERSION_MINOR}")
set(BSOID_VERSION_FULL 
    "${BSOID_VERSION}.${BSOID_VERSION_PATCH}${BSOID_VERSION_EXTRA}")

#=============================================================================#
# Directory Variables.
#=============================================================================#
set(BSOID_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(BSOID_INCLUDE_ROOT "${BSOID_SOURCE_DIR}/include")
set(BSOID_SOURCE_ROOT "${BSOID_SOURCE_DIR}/source")
set(BSOID_SHADER_ROOT "${BSOID_SOURCE_DIR}/shader")
set(BSOID_DOCS_ROOT "${BSOID_SOURCE_DIR}/docs")
set(BSOID_CONFIG_ROOT "${BSOID_SOURCE_DIR}/config")
set(BSOID_ATLAS_ROOT "${BSOID_SOURCE_DIR}/lib/atlas")

#=============================================================================#
# Compilation settings.
#=============================================================================#
include("${BSOID_CONFIG_ROOT}/Compiler.cmake")
include("${BSOID_CONFIG_ROOT}/Externals.cmake")
#include("${BSOID_CONFIG_ROOT}/FindTBB.cmake")

#=============================================================================#
# Generate files.
#=============================================================================#
if (BSOID_BUILD_DOCS)
    configure_file("${BSOID_DOCS_ROOT}/Doxyfile.in"
        "${BSOID_BINARY_DIR}/docs/Doxyfile" @ONLY)
endif()

#=============================================================================#
# Subdirectories.
#=============================================================================#
add_subdirectory("${BSOID_ATLAS_ROOT}")

add_subdirectory("${BSOID_INCLUDE_ROOT}/bsoid")
add_subdirectory("${BSOID_SOURCE_ROOT}")
add_subdirectory("${BSOID_SHADER_ROOT}")

if (DOXYGEN_FOUND AND BSOID_BUILD_DOCS)
    add_subdirectory("${BSOID_DOCS_ROOT}")
endif()

#=============================================================================#
# Include directories.
#=============================================================================#
include_directories(
    ${ATLAS_INCLUDE_DIRS}
    ${BSOID_INCLUDE_ROOT}
    ${BSOID_SHADER_ROOT}
    )

#list(APPEND CMAKE_MODULE_PATH "${BSOID_CONFIG_ROOT}/FindTBB.cmake")

#find_package(TBB COMPONENTS tbbmalloc tbbmalloc_proxy tbb_preview)

#=============================================================================#
# Executable.
#=============================================================================#
include("${BSOID_CONFIG_ROOT}/SourceGroups.cmake")
add_executable(bsoid ${BSOID_SOURCE_LIST} ${BSOID_INCLUDE_LIST}
    ${BSOID_SHADER_LIST})

# Only link with TBB if we are using a parallel build.
#if (BSOID_PARALLEL)
#    target_link_libraries(bsoid tbb ${ATLAS_LIBRARIES})
#else()
    target_link_libraries(bsoid ${ATLAS_LIBRARIES})
#endif()

set_target_properties(bsoid PROPERTIES FOLDER "bsoid")
