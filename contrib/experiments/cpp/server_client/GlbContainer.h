#ifndef GLBCONTAINER_H
#define	GLBCONTAINER_H

#include<boost/unordered_map.hpp>

#include<boost/thread.hpp>
#include<string>
#include<vector>

class GlbContainer {
private:
    long nLookups;
    int glbType;

    boost::mutex nLookupsLock;
    boost::shared_mutex lock;
    boost::unordered_map<std::string, std::vector<std::string> > geoIpv4Records;
    boost::unordered_map<std::string, std::vector<std::string> > geoIpv6Records;
    std::vector<std::string> ipv6Records;
    std::vector<std::string> ipv4Records;
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
        nLookupsLock.lock();
        nLookups++;
        nLookupsLock.unlock();

    }

    long getNLookups() {
        long out;
        nLookupsLock.lock();
        out = nLookups;
        nLookupsLock.unlock();
        return out;
    }
};

extern boost::unordered_map<std::string, boost::shared_ptr<GlbContainer > > glbMap;
extern boost::shared_mutex glbMapLock;

#endif	/* GLBCONTAINER_H */

