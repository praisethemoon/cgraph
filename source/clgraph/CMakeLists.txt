cmake_minimum_required(VERSION 2.6)
project(CLGraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/minunit/
	${PROJECT_SOURCE_DIR}/vendor/remotery/
	${PROJECT_SOURCE_DIR}/vendor/profiler
	${PROJECT_SOURCE_DIR}/vendor/ocl/
)


set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/vendor/remotery/Remotery.c
	${PROJECT_SOURCE_DIR}/vendor/ocl/ocl.c
	${PROJECT_SOURCE_DIR}/main.c
	${PROJECT_SOURCE_DIR}/opencl_helper.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(CLGraph 
	${PROJECT_SRCS}
)

if (UNIX)
	target_link_libraries(CLGraph 
		m 
		pthread
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(CLGraph rt)
endif ()


SET(CF4OCL2_INCLUDE_SEARCH_PATHS
		/usr/include
		/usr/local/include
		/usr/local/include
		/usr/local/Cellar/cf4ocl/2.1.0/include
		)

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
FIND_LIBRARY(CF4OCL2_LIB NAMES cf4ocl2 PATHS ${CF4OCL2_LIB_SEARCH_PATHS})

# Library include directories
include_directories(${CF4OCL2_INCLUDE_DIR})


find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB REQUIRED glib-2.0)
target_include_directories(CLGraph PRIVATE ${GLIB_INCLUDE_DIRS})
target_link_libraries(CLGraph INTERFACE ${GLIB_LDFLAGS})

find_package(OPENCL)

include_directories(${OpenCL_INCLUDE_DIRS} ${CLBLAST_INCLUDE_DIR})


target_link_libraries(CLGraph 
	cgraphapi
	${OpenCL_LIBRARIES}
	${CF4OCL2_LIB}
)






install(TARGETS CLGraph RUNTIME DESTINATION bin)
