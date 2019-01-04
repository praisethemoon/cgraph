cmake_minimum_required(VERSION 2.6)
project(CLDevices)

add_subdirectory(vendor/clew)

set (PROJECT_INCLUDE_DIR
	/usr/local/include #for blas
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/../c_api/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/vendor/clew/include/
)


set (PROJECT_SRCS
	${PROJECT_SOURCE_DIR}/cl_devices.c
	${PROJECT_SOURCE_DIR}/main.c
)

include_directories(${PROJECT_INCLUDE_DIR})

add_executable(CLDevices 
	${PROJECT_SRCS}
)
if (UNIX)
	target_link_libraries(CLDevices 
		m 
		pthread
	)
endif ()

if (UNIX AND NOT APPLE)
	target_link_libraries(CLDevices rt)
endif ()


target_link_libraries(CLDevices 
	cgraphapi
	clew
)



install(TARGETS CLDevices RUNTIME DESTINATION bin)
