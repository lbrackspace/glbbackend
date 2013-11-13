#ifndef GLBCONTAINER_H
#define	GLBCONTAINER_H

#include<boost/unordered_map.hpp>

#include<boost/thread.hpp>
#include<string>
#include<vector>
#include"IPRecord.h"

enum GLBType {
    RANDOM, GEOIP, WEIGHTED, NONE
};

class GlbContainer {
private:
    long nLookups;
    int glbType;

    boost::mutex nLookupsLock;
    boost::shared_mutex lock;
    boost::unordered_map<std::string, std::vector<boost::shared_ptr<IPRecord> > > geoIpv4Records;
    boost::unordered_map<std::string, std::vector<boost::shared_ptr<IPRecord> > > geoIpv6Records;
    std::vector<boost::shared_ptr<IPRecord> > ipv6Records;
    std::vector<boost::shared_ptr<IPRecord> > ipv4Records;
    std::vector<int> ipv4Weights;
    std::vector<int> ipv6Weights;

public:

    GlbContainer() {
        nLookups = 0;
    };

    GlbContainer(int glbType) {
        this->glbType = glbType;
        nLookups = 0;
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

extern boost::unordered_map<std::string, boost::shared_ptr<GlbContainer > > glbMap;
extern boost::shared_mutex glbMapLock;

#endif	/* GLBCONTAINER_H */

