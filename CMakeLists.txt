cmake_minimum_required(VERSION 3.10)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

project(3DCO VERSION 0.1)
add_executable(3DCO src/main.cpp)

add_subdirectory(vendor/spdlog)

set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "..." FORCE)
add_subdirectory(vendor/assimp)

target_include_directories(3DCO PUBLIC
        vendor/spdlog/include
        vendor/assimp/include
        ${CMAKE_BINARY_DIR}/vendor/assimp/include)

target_link_libraries(3DCO assimp)


