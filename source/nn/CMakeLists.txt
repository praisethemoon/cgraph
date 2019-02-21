cmake_minimum_required(VERSION 2.6)
project(nn)

set (PROJECT_INCLUDE_DIR
	${PROJECT_SOURCE_DIR}/
	${PROJECT_SOURCE_DIR}/include/
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/progressbar
	${PROJECT_SOURCE_DIR}/vendor/csv
	${PROJECT_SOURCE_DIR}/vendor/profiler
)

set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/main.c
	#${PROJECT_SOURCE_DIR}/RBM.c

	# dependencies
	${PROJECT_SOURCE_DIR}/vendor/progressbar/progressbar.c
	${PROJECT_SOURCE_DIR}/vendor/progressbar/statusbar.c
	${PROJECT_SOURCE_DIR}/vendor/csv/csv.c
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


set(CURSES_NEED_NCURSES TRUE)
find_package(Curses)
include_directories(${CURSES_INCLUDE_DIRS})

target_link_libraries(nn 
	cgraphapi
	ncurses
)

install(TARGETS nn RUNTIME DESTINATION bin)
