#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include<iostream>
#include<boost/test/unit_test.hpp>
#include<string>
#include<vector>


#include"ring_buffer.h"
#include"GlbContainer.h"

using namespace boost;
using namespace std;

const string TEST_STR(string(" THIS    IS      A  TEST\n"));

BOOST_AUTO_TEST_CASE(ring_buffer_shouldent_strip_line_feed) {
    vector<string> strVector;
    ring_buffer::stringToVector(TEST_STR, strVector, ' ', false);
    BOOST_CHECK(strVector.size() == 4);
    BOOST_CHECK(strVector[0].compare("THIS") == 0);
    BOOST_CHECK(strVector[1].compare("IS") == 0);
    BOOST_CHECK(strVector[2].compare("A") == 0);
    BOOST_CHECK(strVector[3].compare("TEST\n") == 0);
}

BOOST_AUTO_TEST_CASE(ring_buffer_should_strip_line_feed) {
    vector<string> strVector;
    ring_buffer::stringToVector(TEST_STR, strVector, ' ', true);
    BOOST_CHECK(strVector.size() == 4);
    BOOST_CHECK(strVector[0].compare("THIS") == 0);
    BOOST_CHECK(strVector[1].compare("IS") == 0);
    BOOST_CHECK(strVector[2].compare("A") == 0);
    BOOST_CHECK(strVector[3].compare("TEST") == 0);
}

BOOST_AUTO_TEST_CASE(ring_buffer_should_strip_all_line_feeds_too) {
    string lineFeededString("this\n\n is \na test \nblah\nblah\n");
    vector<string> strVector;
    ring_buffer::stringToVector(lineFeededString, strVector, ' ', true);
    BOOST_CHECK(strVector.size() == 6);
    BOOST_CHECK(strVector[0].compare("this") == 0);
    BOOST_CHECK(strVector[1].compare("is") == 0);
    BOOST_CHECK(strVector[2].compare("a") == 0);
    BOOST_CHECK(strVector[3].compare("test") == 0);
    BOOST_CHECK(strVector[4].compare("blah") == 0);
    BOOST_CHECK(strVector[5].compare("blah") == 0);
}

BOOST_AUTO_TEST_CASE(show_size_of_Glb) {
    std::cout << "sizeof(GlbContainer): " << sizeof (GlbContainer) << std::endl;
}