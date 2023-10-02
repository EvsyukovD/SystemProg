project(ProcessUtil)

include_directories(${CMAKE_SOURCE_DIR}/lib/include)
add_subdirectory(lib)

add_executable(ProcessUtil main.c)
target_link_libraries(ProcessUtil util_lib)
#target_link_libraries(ProcessUtil -lpsapi)