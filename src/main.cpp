#include <log/Log.h>
#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"

#define FILENAME "cube_sphere_light.fbx"

#ifdef _WINDOWS
#define ASSETSDIR "..\\..\\tests\\assets\\"
#else
#define ASSETSDIR "../tests/assets/"
#endif


int main(int argc, char* argv[]){
    Log::getInstance()->setLevel(spdlog::level::info);

    FileParser fp{};
    coScene* scene;
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile(std::string(ASSETSDIR) + std::string(FILENAME));
}