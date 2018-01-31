cmake_minimum_required(VERSION 2.6)
project(luacgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/vendor/lua
	${PROJECT_SOURCE_DIR}/vendor/luaarray
	${PROJECT_SOURCE_DIR}/../libcgraph/include/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/../libcgraph/vendor/container
)

set(LUA_SRCS 
	${PROJECT_SOURCE_DIR}/vendor/lua/lzio.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lvm.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lutf8lib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lundump.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ltablib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ltm.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lstrlib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ltable.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lstring.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lstate.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lparser.c
	${PROJECT_SOURCE_DIR}/vendor/lua/loslib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lopcodes.c
	${PROJECT_SOURCE_DIR}/vendor/lua/loadlib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lobject.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lmem.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lmathlib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/llex.c
	${PROJECT_SOURCE_DIR}/vendor/lua/linit.c
	${PROJECT_SOURCE_DIR}/vendor/lua/liolib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lgc.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lfunc.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ldump.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ldo.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ldblib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/ldebug.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lctype.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lcorolib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lbitlib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lcode.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lbaselib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lauxlib.c
	${PROJECT_SOURCE_DIR}/vendor/lua/lapi.c
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/lua_api.c
	
	${PROJECT_SOURCE_DIR}/vendor/luaarray/array.c
	${PROJECT_SOURCE_DIR}/vendor/luaarray/core.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(luacgraph SHARED
	${LUA_SRCS}
	${PROJECT_SRCS}
)

target_link_libraries(luacgraph 
	cgraphapi
)

install(TARGETS luacgraph DESTINATION lib)
