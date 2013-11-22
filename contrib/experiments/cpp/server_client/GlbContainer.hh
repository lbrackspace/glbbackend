#ifndef GLBCONTAINER_H
#define	GLBCONTAINER_H

#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/thread.hpp>
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
    boost::mutex nLookupsMutex;
    boost::shared_mutex glbMutex;
    std::vector<boost::shared_ptr<IPRecord> > ipBoth;
    std::vector<boost::shared_ptr<IPRecord> > ipv4;
    std::vector<boost::shared_ptr<IPRecord> > ipv6;
    std::vector<int> weightsBoth;
    std::vector<int> weightsIpv4;
    std::vector<int> weightsIpv6;

public:

    GlbContainer(std::string cname, int glbType) : nLookups(0), cname(cname), glbType(glbType) {
    }

    GlbContainer() : nLookups(0), cname(""), glbType(GlbType::NONE) {
    }

    virtual ~GlbContainer() {
    };

    void setRandomAlgoIPVectors(std::vector<IPRecord>& ipv4Vals);

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

int gcdreduce(std::vector<int> &reduced_weights, std::vector<int> &weights, int& itercount);
void expandweights(std::vector<int> &expanded, std::vector<int>&weights);
#endif	/* GLBCONTAINER_H */

