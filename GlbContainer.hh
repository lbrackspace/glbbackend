#ifndef GLBCONTAINER_H
#define	GLBCONTAINER_H

#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/thread.hpp>
#include<deque>
#include<string>
#include<vector>
#include"IPRecord.hh"

// Just trying to emulate a scoped enum in pre C++11
namespace GlbType {
    const int NONE = 0;
    const int RANDOM = 1;
    const int GEOIP = 2;
    const int WEIGHTED = 3;
}

class GlbContainer {
private:
    long nLookups;
    int glbType;
    std::string cname;
    std::vector<boost::shared_ptr<IPRecord> > ips;
    std::vector<int> weightsBoth;
    std::vector<int> weightsIpv4;
    std::vector<int> weightsIpv6;
    void getIPSNoneAlgo(std::deque<IPRecord>& ipsOut, int ipType);
    void getIPSRandomAlgo(std::deque<IPRecord>& dq, int ipType);
public:
    boost::mutex nLookupsMutex;
    boost::shared_mutex glbMutex;

    GlbContainer(std::string cname, int glbType) : nLookups(0), glbType(glbType), cname(cname) {
    }

    GlbContainer() : nLookups(0), glbType(GlbType::NONE),cname("") {
    }

    virtual ~GlbContainer() {
    };

    std::string getCName();

    void setIPs(std::vector<IPRecord>& ips);

    void getIPs(std::deque<IPRecord>& dq, int ipType); // The caller initializes the dq and passes by reference

    void clrNLookups() {
        boost::lock_guard<boost::mutex> lock(nLookupsMutex);
        nLookups = 0;
    }

    void incNLookups() {
        boost::lock_guard<boost::mutex> lock(nLookupsMutex);
        nLookups++;
    }

    long getNLookups() {
        boost::lock_guard<boost::mutex> lock(nLookupsMutex);
        return nLookups;
    }
    std::string to_string();
    std::string to_string(bool showIps);
};

int strToGlbType(std::string str);
std::string glbTypeToStr(int gt);

extern boost::unordered_map<std::string, boost::shared_ptr<GlbContainer > > glbMap;
extern boost::shared_mutex glbMapMutex;
extern boost::mutex debugMutex;

int gcdreduce(std::vector<int> &reduced_weights, std::vector<int> &weights, int& itercount);
void expandweights(std::vector<int> &expanded, std::vector<int>&weights);
bool matchesBaseFqdn(const std::string &fqdn, const std::string &baseFqdn);

const bool DEBUG = true;
#endif	/* GLBCONTAINER_H */

