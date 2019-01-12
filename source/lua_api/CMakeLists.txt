cmake_minimum_required(VERSION 2.6)
project(luacgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/vendor/luaarray
	${PROJECT_SOURCE_DIR}/../libcgraph/include/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/../libcgraph/vendor/container
	/usr/local/include/luajit-2.0
)


set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/lua_api.c
	${PROJECT_SOURCE_DIR}/vendor/luaarray/array.c
	${PROJECT_SOURCE_DIR}/vendor/luaarray/core.c
)

link_directories(
	/usr/local/lib
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(luacgraph SHARED
	${PROJECT_SRCS}
)

target_link_libraries(luacgraph 
	cgraphapi
	lua5.1
)

install(TARGETS luacgraph DESTINATION lib)
