#include "GlbContainer.h"

#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/thread.hpp>
#include<vector>
#include<string>

static boost::unordered_map<std::string, int> strToGlbTypeMap;

boost::unordered_map<std::string, int> buildStrToGlbTypeMap() {
    boost::unordered_map<std::string, int> m;
    m["NONE"] = GlbType::NONE;
    m["RANDOM"] = GlbType::RANDOM;
    m["GEOIP"] = GlbType::GEOIP;
    m["WEIGHTED"] = GlbType::WEIGHTED;
    return m;
}

std::string GlbContainer::to_string() {
    return to_string(false);
}

std::string GlbContainer::to_string(bool showIps) {
    std::ostringstream os;
    boost::shared_lock<boost::shared_mutex> sl(lock);
    os << "{ cname=" << cname
            << ", nsLookups=" << nLookups
            << ", glbType=" << glbTypeToStr(glbType)
            << ", nIPv4=" << ipv4.size()
            << ", nIPv6=" << ipv6.size()
            << ", nIPBoth=" << ipBoth.size()
            << "}";
    if (showIps) {
        std::vector<boost::shared_ptr<IPRecord> >::iterator it;
        std::vector<boost::shared_ptr<IPRecord> >::iterator end;
        end = ipv4.end();
        for (it = ipv4.begin(); it != end; it++) {
            IPRecord *ptr = (*it).get();
            os << ptr->to_string() << " ";
        }
        end = ipv6.end();
        for (it = ipv6.begin(); it != end; it++) {
            IPRecord *ptr = (*it).get();
            os << ptr->to_string() << " ";
        }
        end = ipBoth.end();
        for (it = ipBoth.begin(); it != end; it++) {
            IPRecord *ptr = (*it).get();
            os << ptr->to_string() << " ";
        }
    }
    return os.str();
}

int strToGlbType(std::string str) {
    static boost::unordered_map<std::string, int> strToGlbTypeMap = buildStrToGlbTypeMap();
    boost::unordered_map<std::string, int>::const_iterator it = strToGlbTypeMap.find(str);
    if (it == strToGlbTypeMap.end()) {
        return GlbType::NONE;
    }
    return it->second;
}

std::string glbTypeToStr(int gt) {
    switch (gt) {
        case GlbType::NONE:
            return std::string("NONE");
        case GlbType::GEOIP:
            return std::string("GEOIP");
        case GlbType::RANDOM:
            return std::string("RANDOM");
        case GlbType::WEIGHTED:
            return std::string("WEIGHTED");
    }
    return GlbType::NONE;
}

void GlbContainer::setRandomAlgoIPVectors(std::vector<IPRecord>& ipVec) {
    boost::lock_guard<boost::shared_mutex> lk(lock);
    ipv4.clear();
    ipv6.clear();
    std::vector<IPRecord>::iterator it;
    std::vector<IPRecord>::iterator end = ipVec.end();
    for (it = ipVec.begin(); it != end; it++) {
        int itype = (*it).getIPType();
        switch (itype) {
            case IPRecordType::IPv4:
                ipv4.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
                ipBoth.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
                break;
            case IPRecordType::IPv6:
                ipv6.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
                ipBoth.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
                break;
            default: // Report an Error here some how back to DC master
                break;
        }
    }
}

void expandweights(std::vector<int> &expanded, std::vector<int>&weights) {
    int ws = weights.size();
    expanded.clear();
    for (int i = 0; i < ws; i++) {
        int weight = weights[i];
        for (int j = 0; j < weight; j++) {
            weights.push_back(i);
        }
    }
}

// Besure you don't have all vectors weighted at zero as this triggers division by zero

int gcdreduce(std::vector<int> &reduced_weights, std::vector<int> &weights, int& itercount) {
    int r = 0;
    int nWeights = weights.size();
    if (nWeights <= 0) {
        return 0;
    }
    r = weights[0];
    itercount++;
    for (int i = 1; i < nWeights; i++) {
        int a = r;
        int b = weights[i];
        while (a != 0) {
            itercount++;
            int c = a;
            a = b % a;
            b = c;
        }
        r = b;
    }
    reduced_weights.clear();
    for (int i = 0; i < nWeights; i++) {
        reduced_weights.push_back(weights[i] / r);
    }
    return r;
}


boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
boost::shared_mutex glbMapLock;
