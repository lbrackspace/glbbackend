#include "GlbContainer.h"


#include<boost/unordered_map.hpp>
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
    std::ostringstream os;
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

boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
boost::shared_mutex glbMapLock;
