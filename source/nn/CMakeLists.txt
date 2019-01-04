cmake_minimum_required(VERSION 2.6)
project(nn)

set (PROJECT_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/main.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(nn
	${EXTERNAL_DEPS}
	${PROJECT_SRCS}
)

if (UNIX)
	target_link_libraries(nn 
		m 
		pthread
		blas
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(nn rt)
endif ()


target_link_libraries(nn 
	cgraphapi
)

install(TARGETS nn RUNTIME DESTINATION bin)
