include_directories(include)
FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB HEADERS include/*.h)

add_library(CryptLib ${SOURCES} ${HEADERS})
target_link_libraries(CryptLib  -ladvapi32)
