cmake_minimum_required(VERSION 3.10)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# DEFAULT PROJECT CONFIGURATION

project(3DCO LANGUAGES CXX VERSION 0.1 )
add_executable(3DCO src/main.cpp)

set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "..." FORCE)

add_subdirectory(vendor/spdlog)
add_subdirectory(vendor/assimp)

target_include_directories(3DCO PUBLIC
        vendor/spdlog/include
        vendor/assimp/include
        ${CMAKE_BINARY_DIR}/vendor/assimp/include)


target_link_libraries(3DCO assimp)

# TEST CONFIGURATION

find_package(Boost)

add_executable(Boost_Tests_run tests/boost_test.cpp)

set_target_properties(Boost_Tests_run PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Boost_Tests_run PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(Boost_Tests_run ${Boost_LIBRARIES})



