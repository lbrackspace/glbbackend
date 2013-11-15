#include "GlbContainer.h"

#include<boost/unordered_map.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/thread.hpp>
#include<string>

static boost::unordered_map<std::string, short> strToIpTypeMap;
static boost::unordered_map<std::string, short> strToGlbTypeMap;

boost::unordered_map<std::string, short> buildStrToIpTypeMap() {
    boost::unordered_map<std::string, short> m;
    m["NONE"] = IPRecordType::NONE;
    m["IPv4"] = IPRecordType::IPv4;
    m["IPv6"] = IPRecordType::IPv6;
    return m;
}

boost::unordered_map<std::string, short> buildStrToGlbTypeMap() {
    boost::unordered_map<std::string, short> m;
    m["NONE"] = GlbType::NONE;
    m["RANDOM"] = GlbType::RANDOM;
    m["GEOIP"] = GlbType::GEOIP;
    m["WEIGHTED"] = GlbType::WEIGHTED;
    return m;

}

short strToGlbType(std::string str) {
    static boost::unordered_map<std::string, short> strToGlbTypeMap = buildStrToGlbTypeMap();
    boost::unordered_map<std::string, short>::const_iterator it = strToGlbTypeMap.find(str);
    if (it == strToIpTypeMap.end()) {
        return GlbType::NONE;
    }
    return it->second;
}

short strToIpType(std::string str) {
    static boost::unordered_map<std::string, short> strToIpTypeMap = buildStrToIpTypeMap();
    boost::unordered_map<std::string, short>::const_iterator it = strToIpTypeMap.find(str);
    if (it == strToIpTypeMap.end()) {
        return IPRecordType::NONE;
    }
    return it->second;
}

std::string ipTypeToStr(short ipt) {
    switch (ipt) {
        case IPRecordType::IPv4:
            return std::string("IPv4");
        case IPRecordType::IPv6:
            return std::string("IPv6");
        case IPRecordType::NONE:
            return std::string("NONE");
    }
    return std::string("NONE");
}

std::string glbTypeToStr(short gt) {
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
