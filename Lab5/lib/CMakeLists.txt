include_directories(include)
FILE(GLOB SOURCES "${CMAKE_SOURCE_DIR}/lib/src/*.c")
FILE(GLOB HEADERS "${CMAKE_SOURCE_DIR}/lib/include/*.h")
add_library(ThreadLIB ${SOURCES} ${HEADERS})