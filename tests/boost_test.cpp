//
// Created by ubuntu on 24/02/21.
//

#define BOOST_TEST_MODULE default_test_module
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE( default_test_case ){

    std::vector<int> a{1, 2};
    std::vector<int> b{1, 2};

    BOOST_TEST( a == b);

}