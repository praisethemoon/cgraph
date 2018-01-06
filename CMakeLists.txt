cmake_minimum_required(VERSION 2.6)
project(ccgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/vendor/memory
	${PROJECT_SOURCE_DIR}/vendor/profiler
	${PROJECT_SOURCE_DIR}/vendor/lua
	${PROJECT_SOURCE_DIR}/vendor/container
	${PROJECT_SOURCE_DIR}/vendor/thread
)


set( EXTERNAL_DEPS
	${PROJECT_SOURCE_DIR}/vendor/memory/memory.c
	${PROJECT_SOURCE_DIR}/vendor/thread/tinycthread.c
	${PROJECT_SOURCE_DIR}/vendor/container/vec.c
	${PROJECT_SOURCE_DIR}/vendor/container/map.c
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
	${PROJECT_SOURCE_DIR}/main.c
	${PROJECT_SOURCE_DIR}/ccgraph.c
	${PROJECT_SOURCE_DIR}/cg_factory.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(ccgraph 
	${EXTERNAL_DEPS}
	${LUA_SRCS}
	${PROJECT_SRCS}
)

if (UNIX)
	target_link_libraries(ccgraph 
		m 
		pthread
		lapack
		blas
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(ccgraph rt)
endif ()

if (WIN32)
	target_link_libraries(ccgraph kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32 kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
endif ()

install(TARGETS ccgraph RUNTIME DESTINATION bin)
