cmake_minimum_required(VERSION 2.6)
project(cg_unittest)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/minunit/
)


set (PROJECT_SRCS
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
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(cg_unittest rt)
endif ()


target_link_libraries(cg_unittest 
	cgraphapi
)



install(TARGETS cg_unittest RUNTIME DESTINATION bin)