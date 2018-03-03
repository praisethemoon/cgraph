cmake_minimum_required(VERSION 2.6)
project(cgraph_projekt)

add_subdirectory(libcgraph)
add_subdirectory(c_api)
add_subdirectory(unittest)
add_subdirectory(lua_api)

set_property(TARGET cgraph PROPERTY POSITION_INDEPENDENT_CODE ON)
