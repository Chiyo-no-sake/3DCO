cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/lib/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/$<CONFIG>)

# DEFAULT PROJECT CONFIGURATION

project(3DCO LANGUAGES CXX VERSION 0.1 )

# FIX FOR DRACO MESH COMPRESSION ALGORITHM

set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "..." FORCE)

# BUILDING VENDOR LIBRARIES

SET(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "Assimp not builds tools" FORCE)
SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "Assimp not build tests" FORCE)
SET(ASSIMP_INSTALL OFF CACHE BOOL "Assimp not installs" FORCE)
SET(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "Assimp no inject debug postfix" FORCE)

add_subdirectory(vendor/assimp EXCLUDE_FROM_ALL)
add_subdirectory(vendor/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(vendor/meshopt EXCLUDE_FROM_ALL)
add_subdirectory(vendor/args EXCLUDE_FROM_ALL)

# INCLUDING VENDOR DIRECTORIES

include_directories(
		src
		vendor/spdlog/include
		vendor/assimp/include
		vendor/glm
		vendor/meshopt/src
		vendor/args
		vendor/FreeImage/include
		vendor/vhacd/include
)

# ADDITIONAL LINK DIRECTORIES FOR NON BUILT-IN LIBRARIES (FREEIMAGE, VHACD)

if(WIN32 OR MSVC)
	if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
		link_directories(vendor/vhacd/windows/Debug)
	else()
		link_directories(vendor/vhacd/windows/Release)
	endif()
	link_directories(vendor/FreeImage/win/lib)
else()
	link_directories(vendor/vhacd/linux)
	link_directories(vendor/FreeImage/linux)
endif()

# LINK VENDOR LIBRARIES (DYNAMIC)

link_libraries(assimp)
link_libraries(meshoptimizer)
link_libraries(vhacd)

if(WIN32)
	link_libraries(FreeImage)
else()
	link_libraries(freeimage)
endif()

# DEFINE DEFAULT PROJECT EXECUTABLE

add_executable(
		3DCO
		src/main.cpp

		src/model/coNode.cpp
		src/model/coNode.h
		src/model/coMeshData.cpp
		src/model/coMeshData.h
		src/model/coLight.cpp
		src/model/coLight.h
		src/model/coScene.cpp
		src/model/coScene.h
		src/model/coMaterial.cpp
		src/model/coMaterial.h
		src/model/coMesh.cpp
		src/model/coMesh.h

		src/parsing/AssimpStrategy.cpp
		src/parsing/AssimpStrategy.h
		src/parsing/ParsingStrategy.h
		src/parsing/ParsingStrategy.cpp
		src/parsing/FileParser.cpp
		src/parsing/FileParser.h
		src/parsing/material_utils.h
		src/parsing/material_utils.cpp

		src/common/IExecutable.h
		src/common/IChunkable.h

		src/log/Log.h
		src/log/Log.cpp

		src/optimization/OptimizationPipeline.cpp
		src/optimization/OptimizationPipeline.h
		src/optimization/OptimizationStep.cpp
		src/optimization/OptimizationStep.h
		src/optimization/IndexingStep.cpp
		src/optimization/IndexingStep.h
		src/optimization/CacheOptimizationStep.cpp
		src/optimization/CacheOptimizationStep.h
		src/optimization/OverdrawOptimizationStep.cpp
		src/optimization/OverdrawOptimizationStep.h
		src/optimization/VertexFetchOptimizationStep.cpp
		src/optimization/VertexFetchOptimizationStep.h

		src/utils/file_utils.h
		src/utils/file_utils.cpp

		src/ovosdk/OVOExporter.h
		src/ovosdk/OVOExporter.cpp
		src/model/coHull.cpp
		src/model/coHull.h
		src/hullgen/VHACDGenerator.cpp
		src/hullgen/VHACDGenerator.h
		src/log/VHACDLogCallback.h
		src/log/VHACDLogger.cpp
		src/log/VHACDLogger.h)

if (WIN32)
	add_custom_command(TARGET 3DCO POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:3DCO>)
	add_custom_command(TARGET 3DCO POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E
			copy_if_different ${CMAKE_HOME_DIRECTORY}/vendor/FreeImage/win/lib/FreeImage.dll $<TARGET_FILE_DIR:3DCO>)
	add_custom_command(TARGET 3DCO POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E
			copy_if_different ${CMAKE_HOME_DIRECTORY}/vendor/img2dds.exe $<TARGET_FILE_DIR:3DCO>)
else()
	add_custom_command(TARGET 3DCO POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E
			copy_if_different ${CMAKE_HOME_DIRECTORY}/vendor/img2dds $<TARGET_FILE_DIR:3DCO>)
endif()



