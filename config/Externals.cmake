#=============================================================================#
# External dependencies.
#=============================================================================#
if (BSOID_BUILD_DOCS)
    # Skip the graph stuff from Doxygen.
    set(DOXYGE_SKIP_DOT TRUE)
    find_package(Doxygen)
endif()

list(APPEND CMAKE_MODULE_PATH ${BSOID_CONFIG_ROOT})
if (BSOID_PARALLEL)
    find_package(TBB REQUIRED)
endif()
