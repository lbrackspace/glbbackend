#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include<iostream>
#include<sys/time.h>
#include<deque>
#include<vector>
#include<boost/test/unit_test.hpp>
#include<boost/shared_ptr.hpp>
#include<string>
#include<vector>
#include<ctime>
#include <sys/types.h>
#include <unistd.h>
#include"GlbContainer.hh"
#include"IPRecord.hh"
#include"SOAContainer.hh"
#include"server.hh"

using namespace boost;
using namespace std;

string itoa(int i) {
    ostringstream os;
    os << i;
    return os.str();
}

double gettimevalue() {
    double out;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return NAN;
    }
    out = (double) tv.tv_sec + (double) tv.tv_usec * 0.000001;
    return out;
}

class MyTimer {
private:
    double baseTime;
public:

    MyTimer() {
        baseTime = gettimevalue();
    }

    void reset() {
        baseTime = gettimevalue();
    }

    double read() {
        return gettimevalue() - baseTime;
    }
};

BOOST_AUTO_TEST_CASE(test_str_maps) {
    BOOST_CHECK(strToIpType("IPv4") == IPRecordType::IPv4);
    BOOST_CHECK(strToIpType("IPv6") == IPRecordType::IPv6);
    BOOST_CHECK(strToIpType("NONE") == IPRecordType::NONE);
    BOOST_CHECK(strToIpType("ICMP") < 0); // ICMP IS NOT A IP TYPE
    BOOST_CHECK(ipTypeToStr(IPRecordType::IPv4).compare("IPv4") == 0);
    BOOST_CHECK(ipTypeToStr(IPRecordType::IPv6).compare("IPv6") == 0);
    BOOST_CHECK(ipTypeToStr(IPRecordType::NONE).compare("NONE") == 0);


    BOOST_CHECK(strToGlbType("NONE") == GlbType::NONE);
    BOOST_CHECK(strToGlbType("RANDOM") == GlbType::RANDOM);
    BOOST_CHECK(strToGlbType("GEOIP") == GlbType::GEOIP);
    BOOST_CHECK(strToGlbType("WEIGHTED") == GlbType::WEIGHTED);
    BOOST_CHECK(strToGlbType("SILLY") < 0); // SILLY IS NOT A GLB TYPE
    BOOST_CHECK(glbTypeToStr(GlbType::NONE).compare("NONE") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::RANDOM).compare("RANDOM") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::GEOIP).compare("GEOIP") == 0);
    BOOST_CHECK(glbTypeToStr(GlbType::WEIGHTED).compare("WEIGHTED") == 0);
}

BOOST_AUTO_TEST_CASE(test_IP_to_string) {
    std::string expIPRStr("{ ipType=IPv4, ip=127.0.0.1, ttl=30}");
    IPRecord ipr(IPRecordType::IPv4, "127.0.0.1", 30);


    BOOST_CHECK(expIPRStr.compare(ipr.to_string()) == 0);

    std::string expGlbStr("{ cname=testglb.rackexp.org, nsLookups=16, glbType=RANDOM, nIp=5}");
    std::vector<IPRecord> ipVec;
    // Add 2 records
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "127.0.0.1", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "192.168.3.0", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv4, "10.0.0.0", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv6, "::", 30));
    ipVec.push_back(IPRecord(IPRecordType::IPv6, "ffee::", 30));
    GlbContainer glb("testglb.rackexp.org", GlbType::RANDOM);
    glb.setIPs(ipVec);
    for (int i = 0; i < 16; i++) {
        glb.incNLookups();
    }
    cout << "glb=" << glb.to_string() << endl;
    BOOST_CHECK(glb.to_string().compare(expGlbStr) == 0);
}

BOOST_AUTO_TEST_CASE(show_size_of_Glb) {
    std::cout << "sizeof(GlbContainer):: " << sizeof (GlbContainer) << std::endl;
    std::cout << "sizeof(IPRecord):      " << sizeof (IPRecord) << std::endl;
    std::cout << "sizeof(SOAContainer):  " << sizeof (SOAContainer) << std::endl;
    std::cout << "sizeof(ServerJsonBuilder): " << sizeof (ServerJsonBuilder) << std::endl;
    std::cout << "sizeof(IpJson): " << sizeof (IpJson) << std::endl;
}

BOOST_AUTO_TEST_CASE(time_tests) {
    int n = 10000000;
    return; // not running these time consuming tests.
    MyTimer tmr;
    tmr.reset();
    vector<string> vc;
    for (int i = 0; i < n; i++) {
        vc.push_back(itoa(i));
    }
    cout << tmr.read() << " to build vector<string> " << n << endl;

    boost::unordered_map<string, string> map;
    tmr.reset();
    for (int i = 0; i < n; i++) {
        map[vc[i]] = vc[i];
    }
    cout << tmr.read() << " to build map<string,string>" << n << " items to map" << endl;
    tmr.reset();
    unsigned long sum = 0;
    for (int i = 0; i < n; i++) {
        sum += map[vc[i]].size();
    }
    cout << tmr.read() << " to iteratre " << n << " entries in map. sum = " << sum << endl;

    vector<IPRecord> ips;
    tmr.reset();
    for (int i = 0; i < n; i++) {
        ips.push_back(IPRecord(4, "127.0.0.1", 30));
    }
    cout << tmr.read() << " to create " << n << " IPRecords" << endl;

    tmr.reset();
    vector<IPRecord> org;
    for (int i = 0; i < 10; i++) {
        org.push_back(IPRecord(4, "127.0.0.1", 30));
    }
    int isum = 0;
    for (int i = 0; i < 1000000; i++) {
        vector<IPRecord> cpy;
        for (int j = 0; j < 10; j++) {
            cpy.push_back(IPRecord(org[j]));
        }
        isum += cpy.size();
    }
    cout << tmr.read() << " to copy vector<string> of 10 strings total is " << isum << endl;

    isum = 0;
    tmr.reset();
    for (int i = 0; i < 10; i++) {
        vector<IPRecord> cpy;
        for (int j = 0; j < 1000000; j++) {
            cpy.push_back(ips[j]);
            isum += cpy.size();
        }
    }
    cout << tmr.read() << " to  copy 10000000 IPRECORD array 10 times" << endl;

    tmr.reset();
    deque<IPRecord> dq;
    for (int i = 0; i < n; i++) {
        dq.push_front(ips[i]);
    }
    cout << tmr.read() << " to push 10000000 IPRECORDS on deque" << endl;
    tmr.reset();
    vector<IPRecord> cpy;
    for (int i = 0; i < n; i++) {
        cpy.push_back(*dq.begin());
        dq.pop_front();
    }
    cout << tmr.read() << " to pop " << n << " IPRECORDS off the deque" << endl;
}

BOOST_AUTO_TEST_CASE(test_soa) {
    string expSOA("ns1.rackexp.org. root.rackexp.org. 2013102907 28800 14400 3600000 300");
    string expBaseFQDN(".rackspace.org");
    setGlobalSOARecord(expSOA, expBaseFQDN);
    shared_ptr<SOAContainer> soa = getGlobalSOARecord();
    BOOST_CHECK(soa->getSOARecord().compare(expSOA) == 0);
    BOOST_CHECK(soa->getBaseFQDN().compare(expBaseFQDN) == 0);
}

BOOST_AUTO_TEST_CASE(test_nsRecords) {
    shared_ptr<vector<string> > oldRecords = getNSRecords();
    BOOST_CHECK(oldRecords->size() == 0); // No records yet
    vector<string> records;
    records.push_back("ns1.rackexp.org");
    records.push_back("ns2.rackexp.org");
    setNSRecords(records);
    shared_ptr<vector<string> > newRecords = getNSRecords();
    // Verify that setNSRecords didn't clober oldRecords as some threads might
    // still be in the middle of serving up old records
    BOOST_CHECK(oldRecords->size() == 0);
    // Like wise verify that new threads did get the new 2 NS records
    BOOST_CHECK(newRecords);
}

BOOST_AUTO_TEST_CASE(test_matchesBaseFQDN) {
    BOOST_CHECK(matchesBaseFqdn("some.domain.rackexp.org", "rackexp.org"));
    BOOST_CHECK(matchesBaseFqdn("rackexp.org", "rackexp.org"));
    BOOST_CHECK(!matchesBaseFqdn("some.otherdomain.org", "rackexp.org"));
    BOOST_CHECK(!matchesBaseFqdn("otherdomain.org", "rackexp.org"));
}
