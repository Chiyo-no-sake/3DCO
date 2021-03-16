#include <log/Log.h>
#include <optimization/OptimizationPipeline.h>
#include <optimization/IndexingStep.h>
#include <optimization/CacheOptimizationStep.h>
#include <optimization/OverdrawOptimizationStep.h>
#include <optimization/VertexFetchOptimizationStep.h>
#include <optimization/VertexQuantizationStep.h>
#include <optimization/BufferCompressionStep.h>
#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"
#include "ovosdk/OVOExporter.h"

#define FILENAME "cube_sphere_light.fbx"

#ifdef _WINDOWS
#define ASSETSDIR "..\\..\\tests\\assets\\"
#else
#define ASSETSDIR "../../tests/assets/"
#endif


int main(int argc, char* argv[]){
    Log::getInstance()->setLevel(spdlog::level::trace);

    //IMPORTING

    FileParser fp{};
    coScene* scene;
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile(std::string(ASSETSDIR) + std::string(FILENAME));

    //OPTIMIZATION

    OptimizationPipeline pipeline{scene};
    pipeline.append(new IndexingStep());
    pipeline.append(new CacheOptimizationStep());
    pipeline.append(new OverdrawOptimizationStep());
    pipeline.append(new VertexFetchOptimizationStep());
    pipeline.append(new VertexQuantizationStep());
    pipeline.append(new BufferCompressionStep());
    pipeline.execute();

    scene = pipeline.getResult();

    //OVOExporter::exportTo(scene, "testOut.OVO");
}