cmake_minimum_required(VERSION 2.6)
set(CMAKE_BUILD_TYPE Debug)
project(cgraph_projekt)

add_subdirectory(libcgraph)
add_subdirectory(c_api)
add_subdirectory(unittest)
add_subdirectory(lua_api)
add_subdirectory(clgraph)
add_subdirectory(cldevices)
add_subdirectory(nn)

set_property(TARGET cgraph PROPERTY POSITION_INDEPENDENT_CODE ON)
