cmake_minimum_required(VERSION 3.10)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)

# DEFAULT PROJECT CONFIGURATION

project(3DCO LANGUAGES CXX VERSION 0.1 )

# FIX FOR DRACO MESH COMPRESSION ALGORITHM

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "..." FORCE)

# BUILDING VENDOR LIBRARIES

add_subdirectory(vendor/assimp)
add_subdirectory(vendor/spdlog)

# INCLUDING VENDOR DIRECTORIES

include_directories(
		vendor/spdlog/include
		vendor/assimp/include
		${CMAKE_BINARY_DIR}/vendor/assimp/include
)

# LINK ASSIMP LIBRARY (DYNAMIC)

link_libraries(assimp)

# DEFINE DEFAULT PROJECT EXECUTABLE

add_executable(3DCO src/main.cpp src/lib.h)

if (WIN32)
	add_custom_command(TARGET 3DCO POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E 
		copy_if_different ${CMAKE_BINARY_DIR}/vendor/assimp/bin/$<CONFIG>/assimp-vc142-mtd.dll $<TARGET_FILE_DIR:3DCO>)
endif()

# TEST CONFIGURATION

find_package(Boost)

if(Boost_FOUND)
	
	message("Building Tests in tests folder")
	add_executable(Boost_Tests_run tests/boost_test.cpp)

	set_target_properties(Boost_Tests_run PROPERTIES LINKER_LANGUAGE CXX)
	target_include_directories(Boost_Tests_run PUBLIC ${Boost_INCLUDE_DIRS} src)
	target_link_libraries(Boost_Tests_run ${Boost_LIBRARIES})
else()
	message("Boost not installed, not building tests")

endif()



