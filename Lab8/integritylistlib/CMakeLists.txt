include_directories(include)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
  endif(COMMAND cmake_policy)
FILE(GLOB SOURCES src/*.cpp)
FILE(GLOB HEADERS integritylistlib/include/*.h ${CMAKE_SOURCE_DIR}/jsonlib/json.hpp)

add_library(IntegrityListLib ${SOURCES} ${HEADERS})
target_link_libraries(IntegrityListLib PUBLIC CryptLib)