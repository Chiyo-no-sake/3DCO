#include <log/Log.h>
#include <optimization/OptimizationPipeline.h>
#include <optimization/IndexingStep.h>
#include <optimization/CacheOptimizationStep.h>
#include <optimization/OverdrawOptimizationStep.h>
#include <optimization/VertexFetchOptimizationStep.h>
#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"
#include "ovosdk/OVOExporter.h"
#include <args.hxx>
#include <VHACD.h>
#include <hullgen/VHACDGenerator.h>

#ifdef _WINDOWS
#define ASSETSDIR "..\\..\\tests\\assets\\"
#else
#define ASSETSDIR "../../tests/assets/"
#endif

struct user_flags {

    int _abort = 0;
    int _doOptimize = 1;
    std::string _filePath;
    std::vector<OptimizationStep *> _steps{};
    //hullgen

} typedef user_flags;


user_flags *parseArguments(int argc, char *argv[]) {

    //GLOBAL ARGUMENTS

    args::ArgumentParser parser("3D Converter and Optimizer - SUPSI");
    args::HelpFlag helpFlag(parser, "help", "Display this help menu", {'h', "help"});

    args::Flag noOptimizeFlag(parser, "no-optimize", "If this flag is set no mesh optimization is performed",
                              {"no-optimize"});
    //args::Flag hullFlag(parser, "genhull", "If this flag is set the valid input file is fed to the optimization pipeline after being imported", {'h', "generate-hull"});

    args::ValueFlag<unsigned int> verboseFlag(
            parser,
            "verbose",
            "If a value from 0 to 3 is given, the granularity of the logging module changes. 0 is errors only, 3 is all logs. The default is 2.",
            {'v', "verbose"}
    );

    //OPTIMIZATION ARGUMENTS

    args::Group optimizationArgumentsGroup(parser,
                                           "Optimization options. If one or more are specified, only those will be performed",
                                           args::Group::Validators::DontCare);

//    args::Flag allSteps(optimizationArgumentsGroup, "all arguments", "All optimization steps will be performed", {'a', "opt-all"});
//    args::Group optimizationAtLeastOne(optimizationArgumentsGroup, "Optimization steps", args::Group::Validators::AtLeastOne);

    args::Flag indexingStep(optimizationArgumentsGroup, "indexing",
                            "Mesh indices will be re-indexed if present, generated if absent",
                            {'i', "opt-index"});
    args::Flag cacheoptStep(optimizationArgumentsGroup, "cache",
                            "Mesh indices will be re-arranged for cache optimization", {'c', "opt-cache"});
    args::Flag overdrawStep(optimizationArgumentsGroup, "overdraw",
                            "Mesh triangles will be reordered to minimize overdraw", {'d', "opt-overdraw"});
    args::Flag vertexfetchStep(optimizationArgumentsGroup, "vertex fetch",
                               "Index and vertex buffers will be optimized for vertex fetch efficiency",
                               {'f', "opt-fetch"});

    // HULLGEN ARGUMENTS

    // FILE PATH

    args::Positional<std::string> path(parser, "PATH", "Path to the file containing the 3D scene");

    auto *flags = new user_flags();

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help &) {
        std::cout << parser;
        flags->_abort = 1;
    }
    catch (args::ValidationError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        flags->_abort = 1;
        return flags;
    }

    flags->_filePath = path.Get();

    if (verboseFlag) {

        switch (args::get(verboseFlag)) {
            case 0:
                Log::getInstance()->setLevel(spdlog::level::err);
                break;
            case 1:
                Log::getInstance()->setLevel(spdlog::level::warn);
                break;
            case 3:
                Log::getInstance()->setLevel(spdlog::level::trace);
                break;
            case 2:
            default:
                Log::getInstance()->setLevel(spdlog::level::info);
        }

    } else Log::getInstance()->setLevel(spdlog::level::info);

    if (!noOptimizeFlag) {

        flags->_doOptimize = 0;

        if (indexingStep || cacheoptStep || overdrawStep || vertexfetchStep) {

            if (indexingStep)
                flags->_steps.push_back(new IndexingStep{});

            if (cacheoptStep)
                flags->_steps.push_back(new CacheOptimizationStep{});

            if (overdrawStep)
                flags->_steps.push_back(new OverdrawOptimizationStep{});

            if (vertexfetchStep)
                flags->_steps.push_back(new VertexFetchOptimizationStep{});

        } else {
            flags->_steps.push_back(new IndexingStep{});
            flags->_steps.push_back(new CacheOptimizationStep{});
            flags->_steps.push_back(new OverdrawOptimizationStep{});
            flags->_steps.push_back(new VertexFetchOptimizationStep{});
        }

    } else flags->_doOptimize = 1;

    return flags;

}


//TODO copy texture to output folder when written
int main(int argc, char *argv[]) {

    // HANDLE PROGRAM ARGUMENTS

    user_flags *flags = parseArguments(argc, argv);

    if (flags->_abort == 1)
        return 1;

    //IMPORT SCENE
    FileParser fp{};
    coScene *scene;
    fp.setStrategy(new AssimpStrategy());

    scene = fp.loadFromFile(flags->_filePath);

    if(scene==nullptr)
        return 1;

    //OPTIMIZATION

    if (!flags->_doOptimize) {

        OptimizationPipeline pipeline{scene};
        for (auto &step : flags->_steps)
            pipeline.append(step);

        pipeline.execute();

    }

    //HULLGEN

    VHACDGenerator::getInstance()->init();
    VHACDGenerator::getInstance()->compute(scene);
    VHACDGenerator::getInstance()->deinit();

    OVOExporter::exportTo(scene, "testOut.OVO");
}


