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


if(CG_USE_OPENCL)
	set(MATH_OPS
			${PROJECT_SOURCE_DIR}/source/cg_clops.c
			${PROJECT_SOURCE_DIR}/source/cg_opencl.c
		)

else()
	set(MATH_OPS
			${PROJECT_SOURCE_DIR}/source/cg_cpuops.c
        )
endif()

include_directories(${PROJECT_INCLUDE_DIR})


add_library(cgraph STATIC
	${EXTERNAL_DEPS}
	${PROJECT_SRCS}
	${MATH_OPS}
)

# mac os will keep crying, since they deprecated opencl
add_definitions(-DCL_SILENCE_DEPRECATION)

if(CG_USE_LIBCPUID)

	add_definitions(-DCG_USE_LIBCPUID=1)
	SET(LIBCPUID_INCLUDE_SEARCH_PATHS
			/usr/include
			/usr/local/include
			/usr/include/libcpuid
			/usr/local/include/libcpuid
			)

	SET(LIBCPUID_LIB_SEARCH_PATHS
			/lib/
			/lib64/
			/usr/lib
			/usr/lib64
			/usr/local/lib
			/usr/local/lib64
			)

	FIND_PATH(LIBCPUID_INCLUDE_DIR NAMES libcpuid.h PATHS ${LIBCPUID_INCLUDE_SEARCH_PATHS})
	FIND_LIBRARY(LIBCPUID_LIB NAMES cpuid PATHS ${LIBCPUID_LIB_SEARCH_PATHS})

	message('Found libcpuid at `${LIBCPUID_INCLUDE_DIR}` `${LIBCPUID_LIB}`')

	include_directories(${LIBCPUID_INCLUDE_DIR})


	target_link_libraries(cgraph
		${LIBCPUID_LIB}
	)
endif()


if(CG_USE_OPENCL)

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
	include_directories(${CF4OCL2_INCLUDE_DIR}

            /usr/local/include)
		message('Found CF4OCL2 Dir')
        message(${CF4OCL2_INCLUDE_DIR})


	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLIB REQUIRED glib-2.0)

	include_directories(${GLIB_INCLUDE_DIRS})
	target_link_libraries(cgraph INTERFACE ${GLIB_LDFLAGS})

	find_package(OPENCL)
	include_directories(${OpenCL_INCLUDE_DIRS} ${CLBLAST_INCLUDE_DIR})


	SET(CLBLAST_INCLUDE_SEARCH_PATHS
			/usr/include
			/usr/include/clblast
			/usr/local/include
			/usr/local/include/clblast
			/usr/local/include
			)

	SET(CLBLAST_LIB_SEARCH_PATHS
			/lib/
			/lib64/
			/usr/lib
			/usr/lib/clblast
			/usr/lib64
			/usr/local/lib
			/usr/local/lib64
			)

	FIND_PATH(CLBLAST_INCLUDE_DIR NAMES clblast.h PATHS ${CLBLAST_INCLUDE_SEARCH_PATHS})
	FIND_LIBRARY(CLBLAST_LIB NAMES clblast PATHS ${CLBLAST_LIB_SEARCH_PATHS})

	include_directories(CLBLAST_INCLUDE_DIR)
	target_link_libraries(cgraph
		${OpenCL_LIBRARIES}
		${CF4OCL2_LIB}
		${CLBLAST_LIB}
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
