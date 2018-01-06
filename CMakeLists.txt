cmake_minimum_required(VERSION 2.6)
project(ccgraph)

set (PROJECT_INCLUDE_DIR
	/usr/local/include
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/vendor/memory
	${PROJECT_SOURCE_DIR}/vendor/profiler
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/main.c
	${PROJECT_SOURCE_DIR}/ccgraph.c
	${PROJECT_SOURCE_DIR}/vendor/memory/memory.c
)

include_directories(${PROJECT_INCLUDE_DIR})
add_executable(ccgraph ${PROJECT_SRCS})

target_link_libraries(ccgraph
	m
	lapack
	blas
)

install(TARGETS ccgraph RUNTIME DESTINATION bin)
