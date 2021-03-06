//
// Created by ubuntu on 24/02/21.
//

#define BOOST_TEST_MODULE parsing_test_module
#include <boost/test/included/unit_test.hpp>

#include "parsing/FileParser.h"
#include "parsing/AssimpStrategy.h"

BOOST_AUTO_TEST_SUITE(parsing_test_suite);

FileParser fp;
coNode* scene;

BOOST_AUTO_TEST_CASE ( setup ) {
    fp.setStrategy(new AssimpStrategy());
    scene = fp.loadFromFile("assets/cube_sphere_light.fbx");
}

BOOST_AUTO_TEST_CASE( test_parse_mesh ){

    int lodNum = scene->m_numLods;
    BOOST_TEST(lodNum == 1);

    coMesh* mesh = scene->getMMeshes()[0];

    BOOST_TEST(mesh->m_name == "Cube");
    //BOOST_TEST(mesh->)
}

BOOST_AUTO_TEST_CASE( test_parse_light ){

    std::vector<int> a{1, 2};
    std::vector<int> b{1, 2};

    BOOST_TEST( a == b);
}

BOOST_AUTO_TEST_SUITE_END();
