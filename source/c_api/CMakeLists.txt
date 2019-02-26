cmake_minimum_required(VERSION 2.6)
project(cgraphapi)

set (PROJECT_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/../libcgraph/vendor/container
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/cg_api.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(cgraphapi STATIC
	${EXTERNAL_DEPS}
	${PROJECT_SRCS}
)

if (UNIX)
	target_link_libraries(cgraphapi 
		m 
		pthread
		blas
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(cgraphapi rt)
endif ()




target_link_libraries(cgraphapi 
	cgraph
	cgraphcv
)

install(TARGETS cgraphapi
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
