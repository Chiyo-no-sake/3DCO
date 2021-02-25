#include <spdlog/spdlog.h>
#include <assimp/DefaultLogger.hpp>

void test(){
    spdlog::info("Spdlog is Working!");
    Assimp::DefaultLogger::create();
    Assimp::DefaultLogger::get()->info("Assimp is also working!");
    Assimp::DefaultLogger::get()->error("Test");
    Assimp::DefaultLogger::kill();
}