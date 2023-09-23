project(main)

set(C_STANDARD 17)
include_directories(lib)
add_subdirectory(lib)
add_executable(fileUtil main.c)
target_link_libraries(fileUtil LIB)