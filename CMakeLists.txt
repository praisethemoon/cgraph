cmake_minimum_required(VERSION 2.6)
project(cgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include
	${PROJECT_SOURCE_DIR}/source/include
	${PROJECT_SOURCE_DIR}/source/vendor/memory
	${PROJECT_SOURCE_DIR}/source/vendor/profiler
	${PROJECT_SOURCE_DIR}/source/vendor/lua
	${PROJECT_SOURCE_DIR}/source/vendor/container
	${PROJECT_SOURCE_DIR}/source/vendor/thread
	${PROJECT_SOURCE_DIR}/source/vendor/luaarray
)


set( EXTERNAL_DEPS
	${PROJECT_SOURCE_DIR}/source/vendor/memory/memory.c
	${PROJECT_SOURCE_DIR}/source/vendor/thread/tinycthread.c
	${PROJECT_SOURCE_DIR}/source/vendor/container/vec.c
	${PROJECT_SOURCE_DIR}/source/vendor/container/map.c
	${PROJECT_SOURCE_DIR}/source/vendor/luaarray/array.c
	${PROJECT_SOURCE_DIR}/source/vendor/luaarray/core.c
	${PROJECT_SOURCE_DIR}/source/vendor/container/map.c
)

set(LUA_SRCS 
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lzio.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lvm.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lutf8lib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lundump.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ltablib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ltm.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lstrlib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ltable.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lstring.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lstate.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lparser.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/loslib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lopcodes.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/loadlib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lobject.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lmem.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lmathlib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/llex.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/linit.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/liolib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lgc.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lfunc.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ldump.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ldo.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ldblib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/ldebug.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lctype.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lcorolib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lbitlib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lcode.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lbaselib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lauxlib.c
	${PROJECT_SOURCE_DIR}/source/vendor/lua/lapi.c
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/source/cgraph.c
	${PROJECT_SOURCE_DIR}/source/cg_factory.c
	${PROJECT_SOURCE_DIR}/source/cg_constants.c
	${PROJECT_SOURCE_DIR}/source/cg_errors.c
	${PROJECT_SOURCE_DIR}/source/lua_api/lua_api.c
	${PROJECT_SOURCE_DIR}/source/main.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(cgraph SHARED
	${EXTERNAL_DEPS}
	${LUA_SRCS}
	${PROJECT_SRCS}
)

#add_executable(cgraph 
#	${EXTERNAL_DEPS}
#	${LUA_SRCS}
#	${PROJECT_SRCS}
#)

if (UNIX)
	target_link_libraries(cgraph 
		m 
		pthread
		lapack
		blas
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(cgraph rt)
endif ()

if (WIN32)
	target_link_libraries(cgraph kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32 kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
endif ()

#install(TARGETS cgraph RUNTIME DESTINATION bin)
install(TARGETS cgraph DESTINATION bin)
