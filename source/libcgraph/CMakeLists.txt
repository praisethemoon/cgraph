cmake_minimum_required(VERSION 2.6)
project(cgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/profiler
	${PROJECT_SOURCE_DIR}/vendor/container
	${PROJECT_SOURCE_DIR}/vendor/thread
	${PROJECT_SOURCE_DIR}/vendor/random
)


set( EXTERNAL_DEPS
	${PROJECT_SOURCE_DIR}/vendor/thread/tinycthread.c
	${PROJECT_SOURCE_DIR}/vendor/container/vec.c
	${PROJECT_SOURCE_DIR}/vendor/container/map.c
	${PROJECT_SOURCE_DIR}/vendor/random/random.c
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/source/cgraph.c
	${PROJECT_SOURCE_DIR}/source/cg_factory.c
	${PROJECT_SOURCE_DIR}/source/cg_constants.c
	${PROJECT_SOURCE_DIR}/source/cg_errors.c
	${PROJECT_SOURCE_DIR}/source/cg_diff.c
	${PROJECT_SOURCE_DIR}/source/cg_math.c
	${PROJECT_SOURCE_DIR}/source/cg_free.c
	${PROJECT_SOURCE_DIR}/source/cg_cpu.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(cgraph STATIC
	${EXTERNAL_DEPS}
	${PROJECT_SRCS}
)

if(COMMAND CG_USE_LIBCPUID)
target_link_libraries(cgraph
	cpuid
)

endif()

if (APPLE)
	include_directories(/usr/local/opt/openblas/include)
endif()

if (UNIX)
	target_link_libraries(cgraph 
		m 
		pthread
		blas
	) 
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(cgraph rt)
endif ()

if (WIN32)
	target_link_libraries(cgraph kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32 kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
endif ()

install(TARGETS cgraph
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
