#define BOOST_TEST_MODULE parsing_test_module
#include <boost/test/included/unit_test.hpp>

#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"

#define FILENAME "cube_sphere_light.fbx"
#ifdef _WINDOWS
#define ASSETSDIR "..\\..\\tests\\assets\\"
#else
#define ASSETSDIR "../tests/assets/"
#endif


BOOST_AUTO_TEST_SUITE(parsing_test_suite);

FileParser fp;
coScene* scene;

BOOST_AUTO_TEST_CASE ( setup ) {
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile(std::string(ASSETSDIR) + FILENAME);
}

BOOST_AUTO_TEST_CASE( test_parse_mesh_lights ){

    coNode* rootNode = scene->m_rootNode;
    BOOST_TEST(rootNode->m_name == "RootNode");

    coNode* cube = rootNode->getMChildren()[0];
    coNode* light = rootNode->getMChildren()[1];

    BOOST_TEST(cube->m_name == "Cube");
    BOOST_TEST(cube->m_numChildren == 2);

    BOOST_TEST(light->m_name == "Point");
    BOOST_TEST(light->m_numChildren == 0);

    int lightsNum = light->getMLights().size();

    BOOST_TEST(lightsNum == 1);

    coLight* spLight = light->getMLights()[0];
    BOOST_TEST(spLight->m_type == lightType::OMNI);

    coNode* cube2 = cube->getMChildren()[0];
    coNode* sphere = cube->getMChildren()[1];

    BOOST_TEST(cube2->m_name == "Cube.001");
    BOOST_TEST(sphere->m_name == "Sphere");
}

BOOST_AUTO_TEST_SUITE_END();
