
include_directories(${CMAKE_SOURCE_DIR}/lib/include)

add_library(
    LIB ${CMAKE_SOURCE_DIR}/lib/src/winapifiles.c 
${CMAKE_SOURCE_DIR}/lib/include/util.h 
${CMAKE_SOURCE_DIR}/lib/src/fmap.c 
${CMAKE_SOURCE_DIR}/lib/src/commonfuncs.c)