cmake_minimum_required(VERSION 2.6)
project(cgraphui)

#set( EXTERNAL_DEPS
#        ${PROJECT_SOURCE_DIR}/vendor/
#        )

set (PROJECT_SRCS
        #${PROJECT_SOURCE_DIR}/source/app.c
        ${PROJECT_SOURCE_DIR}/source/cg_plot.c
)



set( PROJECT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/vendor/kplot
        ${PROJECT_SOURCE_DIR}/vendor/nuklear
	${PROJECT_SOURCE_DIR}/../libcgraph/include/
	${PROJECT_SOURCE_DIR}/../libcgraph/include/c_api
	${PROJECT_SOURCE_DIR}/../libcgraph/vendor/container
        )

set(EXTERNAL_DEPS
        ${PROJECT_SOURCE_DIR}/vendor/kplot/array.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/border.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/bucket.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/buffer.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/colours.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/grid.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/hist.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/kdata.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/kplot.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/label.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/margin.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/mean.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/plotctx.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/reallocarray.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/stddev.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/tic.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/vector.c
        ${PROJECT_SOURCE_DIR}/vendor/kplot/draw.c
)

include_directories(${PROJECT_INCLUDE_DIR})


find_package(Cairo)
find_package(GLFW)
find_package(GLEW)

include_directories(${CAIRO_INCLUDE_DIRS} ${GLFW_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

add_library(cgraphui
        ${EXTERNAL_DEPS}
        ${PROJECT_SRCS}
)


target_link_libraries(cgraphui
        cgraph
        ${CAIRO_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARY}
        ${OPENGL_LIBRARIES}
)

