#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include<iostream>
#include<boost/test/unit_test.hpp>
#include<string>
#include<vector>


#include"ring_buffer.h"
#include"GlbContainer.h"
#include"IPRecord.h"
#include"GlbStaticLoader.h"

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

BOOST_AUTO_TEST_CASE(check_IPRecordType_Enum) {
    BOOST_CHECK(GlbStaticLoader::get().getIPRecordTypeInt("IPv4") == IPRecordType::IPv4);
    BOOST_CHECK(GlbStaticLoader::get().getIPRecordTypeInt("IPv6") == IPRecordType::IPv6);
    BOOST_CHECK(GlbStaticLoader::get().getIPRecordTypeInt("NONE") == IPRecordType::NONE);
    BOOST_CHECK(GlbStaticLoader::get().getIPRecordTypeInt("XXXX") == IPRecordType::NONE);
    BOOST_CHECK(GlbStaticLoader::get().getIPRecordTypeInt("IPv4") != IPRecordType::NONE);

    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("RANDOM") == GlbType::RANDOM);
    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("GEOIP") == GlbType::GEOIP);
    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("WEIGHTED") == GlbType::WEIGHTED);
    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("NONE") == GlbType::NONE);
    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("XXXX") == GlbType::NONE);
    BOOST_CHECK(GlbStaticLoader::get().getGlbTypeInt("GEOIP") != GlbType::RANDOM);

}

BOOST_AUTO_TEST_CASE(check_GlbStatic_to_string) {
    IPRecord test_record(IPRecordType::IPv4, "127.0.0.1", 30);
    BOOST_CHECK(GlbStaticLoader::get().to_string(test_record).compare("{ rType=IPv4, IPaddress=127.0.0.1, ttl=30 }") == 0);
}

BOOST_AUTO_TEST_CASE(show_size_of_Glb) {
    std::cout << "sizeof(GlbContainer):: " << sizeof (GlbContainer) << std::endl;
    std::cout << "sizeof(IPRecord):      " << sizeof (IPRecord) << std::endl;
}