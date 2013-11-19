#include "GlbContainer.h"

#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/thread.hpp>
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
                break;
            case IPRecordType::IPv6:
                ipv6.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
                break;
            default: // Report an Error here some how back to DC master
                break;
        }
    }
}


boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
boost::shared_mutex glbMapLock;
