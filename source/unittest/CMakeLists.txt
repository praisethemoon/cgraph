cmake_minimum_required(VERSION 2.6)
project(cg_unittest)

set (PROJECT_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/minunit/
	${PROJECT_SOURCE_DIR}/vendor/remotery/
	${PROJECT_SOURCE_DIR}/vendor/profiler/
)


set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/vendor/remotery/Remotery.c
	${PROJECT_SOURCE_DIR}/cg_unittests.c
	${PROJECT_SOURCE_DIR}/main.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(cg_unittest 
	${PROJECT_SRCS}
)
if (UNIX)
	target_link_libraries(cg_unittest 
		m 
		pthread
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(cg_unittest rt)
endif ()


target_link_libraries(cg_unittest 
	cgraphapi
)



install(TARGETS cg_unittest RUNTIME DESTINATION bin)
