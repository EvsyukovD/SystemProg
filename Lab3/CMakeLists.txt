project(main)

include_directories(lib)
add_subdirectory(lib)
add_executable(fileUtil main.c)
target_link_libraries(fileUtil LIB)