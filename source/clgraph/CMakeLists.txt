cmake_minimum_required(VERSION 2.6)
project(CLGraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/minunit/
	${PROJECT_SOURCE_DIR}/vendor/remotery/
)


set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/vendor/remotery/Remotery.c
	${PROJECT_SOURCE_DIR}/cg_unittests.c
	${PROJECT_SOURCE_DIR}/main.c
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


target_link_libraries(CLGraph 
	cgraphapi
	OpenCL 
	clblast
)



install(TARGETS CLGraph RUNTIME DESTINATION bin)
