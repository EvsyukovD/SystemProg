include_directories(include)
FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB HEADERS include/*.h)

add_library(FileSelectLib ${SOURCES} ${HEADERS})
target_link_libraries(FileSelectLib  -lcomdlg32)
