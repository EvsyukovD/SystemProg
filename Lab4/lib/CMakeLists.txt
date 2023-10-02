include_directories(${CMAKE_SOURCE_DIR}/lib/include)
set(SOURCES_DIR ${CMAKE_SOURCE_DIR}/lib/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/include)
add_library(util_lib STATIC ${SOURCES_DIR}/util.c
                            ${SOURCES_DIR}/common.c
                            ${INCLUDE_DIR}/util.h)