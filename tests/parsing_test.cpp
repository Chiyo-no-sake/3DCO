#define BOOST_TEST_MODULE parsing_test_module
#include <boost/test/included/unit_test.hpp>

#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"

BOOST_AUTO_TEST_SUITE(parsing_test_suite);

FileParser fp;
coNode* scene;

BOOST_AUTO_TEST_CASE ( setup ) {
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile("..\\..\\tests\\assets\\cube_sphere_light.fbx");
}

BOOST_AUTO_TEST_CASE( test_parse_mesh ){

    BOOST_TEST(scene->m_name == "RootNode");

    coNode* cube = scene->getMChildren()[0];
    coNode* light = scene->getMChildren()[1];

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

BOOST_AUTO_TEST_CASE( test_parse_light ){

    std::vector<int> a{1, 2};
    std::vector<int> b{1, 2};

    BOOST_TEST( a == b);
}

BOOST_AUTO_TEST_SUITE_END();
