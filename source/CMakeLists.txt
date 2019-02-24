cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug)
project(cgraph_projekt)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(CG_USE_LIBCPUID "Use CPUID to detect CPU Features" OFF)
option(CG_USE_OPENCL "OpenCL acceleration" OFF)

if(CG_USE_OPENCL)
    add_compile_definitions(CG_USE_OPENCL)
endif()

if(CG_USE_OPENCL)

    SET(CF4OCL2_LIB_SEARCH_PATHS
            /lib/
            /lib64/
            /usr/lib
            /usr/lib64
            /usr/local/lib
            /usr/local/lib64
            /usr/local/Cellar/cf4ocl/2.1.0/lib
            )

    FIND_PATH(CF4OCL2_INCLUDE_DIR NAMES cf4ocl2.h PATHS ${CF4OCL2_INCLUDE_SEARCH_PATHS})

    # Library include directories
    include_directories(${CF4OCL2_INCLUDE_DIR})

    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLIB REQUIRED glib-2.0)

    include_directories(${GLIB_INCLUDE_DIRS})
endif()

add_subdirectory(libcgraph)
add_subdirectory(c_api)
add_subdirectory(unittest)
add_subdirectory(lua_api)
add_subdirectory(cv)
add_subdirectory(ui)
#add_subdirectory(cldevices)
add_subdirectory(nn)

set_property(TARGET cgraph PROPERTY POSITION_INDEPENDENT_CODE ON)
