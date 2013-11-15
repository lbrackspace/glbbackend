#ifndef GLBCONTAINER_H
#define	GLBCONTAINER_H

#include<boost/unordered_map.hpp>

#include<boost/thread.hpp>
#include<string>
#include<vector>
#include"IPRecord.h"

// Just trying to emulate a scoped enum in pre C++11
namespace GlbType {
    const short NONE = 0;
    const short RANDOM = 1;
    const short GEOIP = 2;
    const short WEIGHTED = 3;
}
// Just trying to emulate a scoped enum classesin pre C++11
namespace IPRecordType {
    const short NONE = 0;
    const short IPv4 = 1;
    const short IPv6 = 2;
}

class GlbContainer {
private:
    long nLookups;
    short ipType;
    short glbType;
    std::string cname;
    boost::mutex nLookupsLock;
    boost::shared_mutex lock;
    std::vector<boost::shared_ptr<IPRecord> > ips;
    std::vector<int> weights;

public:

    GlbContainer(std::string cname, int glbType) : nLookups(0), cname(cname), glbType(glbType) {
    }

    GlbContainer() : nLookups(0), cname(""), glbType(GlbType::NONE) {
    }

    virtual ~GlbContainer() {
    };

    void incNLookups() {
        boost::lock_guard<boost::mutex> lock(nLookupsLock);
        nLookups++;
    }

    long getNLookups() {
        boost::lock_guard<boost::mutex> lock(nLookupsLock);
        return nLookups;
    }
};

short strToGlbType(std::string str);
short strToIpType(std::string str);
std::string ipTypeToStr(short ipt);
std::string glbTypeToStr(short gt);

extern boost::unordered_map<std::string, boost::shared_ptr<GlbContainer > > glbMap;
extern boost::shared_mutex glbMapLock;

#endif	/* GLBCONTAINER_H */

