project(IntegrityCheck)

add_subdirectory(cryptolib)
add_subdirectory(fileselectorlib)
add_subdirectory(integritylistlib)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(IntegrityCheck integrity_checker.cpp)
target_link_libraries(IntegrityCheck PUBLIC IntegrityListLib)
target_link_libraries(IntegrityCheck PUBLIC FileSelectLib)
