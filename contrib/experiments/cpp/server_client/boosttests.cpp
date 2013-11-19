#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include<iostream>
#include<boost/test/unit_test.hpp>
#include<string>
#include<vector>


#include"ring_buffer.h"
#include"GlbContainer.h"
#include"IPRecord.h"

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

BOOST_AUTO_TEST_CASE(test_str_maps) {
    BOOST_CHECK(strToIpType("IPv4") == IPRecordType::IPv4);
    BOOST_CHECK(strToIpType("IPv6") == IPRecordType::IPv6);
    BOOST_CHECK(strToIpType("NONE") == IPRecordType::NONE);
    BOOST_CHECK(ipTypeToStr(IPRecordType::IPv4).compare("IPv4") == 0);
    BOOST_CHECK(ipTypeToStr(IPRecordType::IPv6).compare("IPv6") == 0);
    BOOST_CHECK(ipTypeToStr(IPRecordType::NONE).compare("NONE") == 0);

    BOOST_CHECK(strToGlbType("NONE") == GlbType::NONE);
    BOOST_CHECK(strToGlbType("RANDOM") == GlbType::RANDOM);
    BOOST_CHECK(strToGlbType("GEOIP") == GlbType::GEOIP);
    BOOST_CHECK(strToGlbType("WEIGHTED") == GlbType::WEIGHTED);
    BOOST_CHECK(glbTypeToStr(GlbType::NONE).compare("NONE") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::RANDOM).compare("RANDOM") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::GEOIP).compare("GEOIP") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::WEIGHTED).compare("WEIGHTED") == 0);
}

BOOST_AUTO_TEST_CASE(test_IP_to_string) {
    std::string expIPRStr("{ ipType=IPv4, ip=127.0.0.1, ttl=30}");
    IPRecord ipr(IPRecordType::IPv4, "127.0.0.1", 30);


    BOOST_CHECK(expIPRStr.compare(ipr.to_string()) == 0);

    std::string expGlbStr("{ cname=testglb.rackexp.org, nsLookups=10, glbType=RANDOM, nIPv4=2, nIPv6=3}");
    std::vector<IPRecord> ipVec;
    // Add 2 records
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "127.0.0.1", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "192.168.3.0", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "10.0.0.0", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv6, "::", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv6, "ffee::", 30));
    GlbContainer glb("testglb.rackexp.org", GlbType::RANDOM);
    glb.setRandomAlgoIPVectors(ipVec);
    for (int i = 0; i < 16; i++) {
        glb.incNLookups();
    }
    std::cout << glb.to_string(true) << std::endl;
}


BOOST_AUTO_TEST_CASE(show_size_of_Glb) {
    std::cout << "sizeof(GlbContainer):: " << sizeof (GlbContainer) << std::endl;
    std::cout << "sizeof(IPRecord):      " << sizeof (IPRecord) << std::endl;
}