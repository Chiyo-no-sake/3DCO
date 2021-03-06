#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"

int main(int argc, char* argv[]){
    FileParser fp;
    coNode* scene;
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile("tests/assets/cube_sphere_light.fbx");
}