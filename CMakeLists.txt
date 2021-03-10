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
SET(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
SET(ASSIMP_BUILD_TESTS OFF)
SET(ASSIMP_INSTALL OFF)
SET(ASSIMP_INJECT_DEBUG_POSTFIX OFF)


add_subdirectory(vendor/assimp EXCLUDE_FROM_ALL)
add_subdirectory(vendor/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(vendor/meshopt EXCLUDE_FROM_ALL)

# INCLUDING VENDOR DIRECTORIES

include_directories(
		src
		vendor/spdlog/include
		vendor/assimp/include
		vendor/glm
		vendor/meshopt/src
		${CMAKE_BINARY_DIR}/vendor/assimp/include
)

# LINK ASSIMP LIBRARY (DYNAMIC)

link_libraries(assimp)
link_libraries(meshoptimizer)

# DEFINE DEFAULT PROJECT EXECUTABLE

add_executable(3DCO
		src/main.cpp
		src/model/coNode.cpp
		src/model/coNode.h
		src/model/coMesh.cpp
		src/model/coMesh.h
		src/model/coLight.cpp
		src/model/coLight.h
		src/parsing/AssimpStrategy.cpp
		src/parsing/AssimpStrategy.h
		src/parsing/ParsingStrategy.h
		src/parsing/ParsingStrategy.cpp
		src/parsing/FileParser.cpp
		src/parsing/FileParser.h
		src/common/IExecutable.h
		src/log/Log.h
		src/log/Log.cpp
		src/model/coScene.cpp
		src/model/coScene.h
		src/model/coMaterial.cpp
		src/model/coMaterial.h
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
		src/optimization/VertexQuantizationStep.cpp
		src/optimization/VertexQuantizationStep.h
		src/optimization/BufferCompressionStep.cpp
		src/optimization/BufferCompressionStep.h)

if (WIN32)
	add_custom_command(TARGET 3DCO POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different $<TARGET_FILE:assimp> $<TARGET_FILE_DIR:3DCO>)
endif()

# TEST CONFIGURATION

find_package(Boost)

if(Boost_FOUND)
	
	message("Building Tests in tests folder")
	add_executable(3DCO_tests
			src/model/coNode.cpp
			src/model/coNode.h
			src/model/coMesh.cpp
			src/model/coMesh.h
			src/model/coLight.cpp
			src/model/coLight.h
			src/parsing/AssimpStrategy.cpp
			src/parsing/AssimpStrategy.h
			src/parsing/ParsingStrategy.h
			src/parsing/ParsingStrategy.cpp
			src/parsing/FileParser.cpp
			src/parsing/FileParser.h
			src/common/IExecutable.h
			src/log/Log.h
			src/log/Log.cpp
			tests/parsing_test.cpp
			src/model/coScene.cpp
			src/model/coScene.h
			src/ovosdk/OVOExporter.cpp
			src/ovosdk/OVOExporter.h
			src/optimization/OptimizationPipeline.cpp
			src/optimization/OptimizationPipeline.h
			src/optimization/IndexingStep.cpp
			src/optimization/IndexingStep.h
			src/optimization/CacheOptimizationStep.cpp
			src/optimization/CacheOptimizationStep.h
			src/optimization/OverdrawOptimizationStep.cpp
			src/optimization/OverdrawOptimizationStep.h
			src/optimization/VertexFetchOptimizationStep.cpp
			src/optimization/VertexFetchOptimizationStep.h
			src/optimization/VertexQuantizationStep.cpp
			src/optimization/VertexQuantizationStep.h
			src/optimization/BufferCompressionStep.cpp
			src/optimization/BufferCompressionStep.h)

	set_target_properties(3DCO_tests PROPERTIES LINKER_LANGUAGE CXX)
	target_include_directories(3DCO_tests PUBLIC ${Boost_INCLUDE_DIRS} src)
	target_link_libraries(3DCO_tests ${Boost_LIBRARIES})
else()
	message("Boost not installed, not building tests")

endif()



