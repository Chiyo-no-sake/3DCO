#include <log/Log.h>
#include <optimization/OptimizationPipeline.h>
#include <optimization/IndexingStep.h>
#include <optimization/CacheOptimizationStep.h>
#include <optimization/OverdrawOptimizationStep.h>
#include <optimization/VertexFetchOptimizationStep.h>
#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"
#include "ovosdk/OVOExporter.h"

#define FILENAME "cube_sphere_light.fbx"

#ifdef _WINDOWS
#define ASSETSDIR "..\\..\\tests\\assets\\"
#else
#define ASSETSDIR "../../tests/assets/"
#endif

void parseArguments(int argc, char* argv[]){



}

int main(int argc, char* argv[]){

    // HANDLE PROGRAM ARGUMENTS

    parseArguments(argc, argv);

    //IMPORT SCENE

    FileParser fp{};
    coScene* scene;
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile(std::string(ASSETSDIR) + std::string(FILENAME));

    //OPTIMIZE MESHES


    //OVOExporter::exportTo(scene, "testOut.OVO");
}

