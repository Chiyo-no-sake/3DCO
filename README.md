# 3DCO - 3D format Converter and Optimizer
3DCO is a command line tool that is able to convert nearly any file format into .OVO, the 3d file format used in OverVision Engine (http://www.peternier.com/projects/overvision/overvision.php).
This tool performs material, light, texture and coordinate system conversion as well as Approximate Convex Decomposition for the hull generation and various mesh optimizations.

Mesh optimizations are made with the meshopt library: https://github.com/zeux/meshoptimizer
File parsing is made with Assimp: https://www.assimp.org/
Approximate Hull Decomposition has been achieved with V-HACD library: https://github.com/kmammou/v-hacd
And lastly, spdlog for logging: https://github.com/gabime/spdlog


![pbr_scene_collage (1)](https://user-images.githubusercontent.com/20605899/117716718-3a0f4c00-b1da-11eb-8c70-62bf947b5dad.jpg)

## Getting 3DCO

You can either choose to download binaries for your platform (release tab) and just execute them (see usage) or to build the source code for yourself.

### Building from source
The project requires cmake to be built. 
If you want to execute tests, you have to install boost-test, otherwise only the main executable will be built.

#### On Linux

On linux, move to the folder where you want the program to be downloaded, then execute:
''' 
git clone https://gitlab.com/KatonKalu/3d-converter-and-mesh-optimizer
cd 3d-converter-and-mesh-optimizer
git submodule update --init --recursive
cmake .
make
'''

If your CPU has 2 or more cores, it's advised to use 
'''
make -j <X>
''' 
where X is the number of cores to be used while compiling. This speeds up the compilation process a lot.

#### On Windows
* Clone the repo in a folder of your choice
'''
git clone https://gitlab.com/KatonKalu/3d-converter-and-mesh-optimizer
cd 3d-converter-and-mesh-optimizer
git submodule update --init --recursive
'''
* Open CMake GUI. Select the repo folder as the sources root and another or the same directory for build
* [Optional] If you want to build tests, remember to declare the environment variable BOOST_ROOT and set its value to the boost library path (the one extracted from Boost .zip file).
* Click 'configure'
* Click 'generate'
* Now that .sln files have been generated in the build directory you can open the project with Visual Studio and build as normal. 
Build project 'Boost_Tests_run' to build tests, '3DCO' to build the tool.
* Executables are built to bin directory.

##### OR
Download the project pre-configured for visual studio in the releases tab, and then build it.

## Usage

Typical use case: <code>./3DCO myfile.fbx</code>

Run with <code>-h</code> to get an overview of the features and how to use them.
