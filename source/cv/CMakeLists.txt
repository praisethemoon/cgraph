cmake_minimum_required(VERSION 2.6)
project(cgraphcv)


#set( EXTERNAL_DEPS
#        ${PROJECT_SOURCE_DIR}/vendor/
#        )

set (PROJECT_SRCS
        ${PROJECT_SOURCE_DIR}/source/cg-color.c
        ${PROJECT_SOURCE_DIR}/source/cg-filter.c
        ${PROJECT_SOURCE_DIR}/source/cg-image.c
        )


set(PROJECT_INCLUDE_DIR
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

include_directories(${PROJECT_INCLUDE_DIR})

add_library(cgraphcv STATIC
        #${EXTERNAL_DEPS}
        ${PROJECT_SRCS}
    )