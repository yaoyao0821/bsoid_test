#=============================================================================#
# Compiler configuration file.
#=============================================================================#
# First identify the compiler that we are using.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(BSOID_COMPILER_CLANG 1)
    message(STATUS "Using LLVM Clang...")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(BSOID_COMPILER_INTEL 1)
    message(STATUS "Using Intel...")
elseif(MSVC)
    if (MSVC_VERSION VERSION_GREATER 1800)
        set(BSOID_COMPILER_MSVC 1)
        message(STATUS "Using MSVC ${MSVC_VERSION}")
    else()
        message(FATAL_ERROR "Athena requires at least Visual Studio 12 to run.")
    endif()
else()
    set(BSOID_COMPILER_GNU 1)
    message(STATUS "Using GNU...")
endif()

# Before we get into the platform-specific flags, enable the parallel macro if
# this is a parallel build.
if (BSOID_PARALLEL)
    add_definitions(-DBSOID_PARALLEL)
endif()

# Now set the compiler flags, notice that Windows requires a different syntax
# for flags than Linux does, so lets handle that one first.
if (WIN32)
    if (BSOID_COMPILER_MSVC)
        add_definitions(
            -wd4201     # nonstandard extension used: nameless struct/union.
            -D_CRT_SECURE_NO_WARNINGS
            )

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /std:c++17")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    elseif(BSOID_COMPILER_INTEL)
        add_definitions(
            -wd4201
            -wd3280
            -wd3346
            -wd1897
            -wd1879
            -D_CRT_SECURE_NO_WARNINGS
            )

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Qstd=c++17")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    endif()
else()
    # TODO: Any additional flags for Clang/GCC/Intel go here.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++14")
endif()

