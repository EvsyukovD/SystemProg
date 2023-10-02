project(ProcessUtil)

set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_SOURCE_DIR}/lib/include)
add_subdirectory(lib)
add_executable(ProcessUtil main.c)
target_link_libraries(ProcessUtil util_lib)