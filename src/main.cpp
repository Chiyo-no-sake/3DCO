//
// Created by kalu on 2/24/21.
//

#include <cstdio>
#include <spdlog/spdlog.h>
#include <assimp/DefaultLogger.hpp>

int main(int argc, char* argv[]){
    spdlog::info("Spdlog is Working!");
    Assimp::DefaultLogger::create();
    Assimp::DefaultLogger::get()->info("Assimp is also working!");
    Assimp::DefaultLogger::get()->error("Test");
    Assimp::DefaultLogger::kill();
}
