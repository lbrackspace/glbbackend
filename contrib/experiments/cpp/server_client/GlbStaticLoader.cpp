#include<boost/unordered_map.hpp>
#include<ostream>
#include "GlbStaticLoader.h"
#include "IPRecord.h"
#include "GlbContainer.h"

int getInstanceCount = 0;
int constructorCallCount = 0;

std::string GlbStaticLoader::getIPRecordTypeString(int rt) const {
    switch (rt) {
        case IPRecordType::IPv4:
            return std::string("IPv4");
        case IPRecordType::IPv6:
            return std::string("IPv6");
    }
    return std::string("NONE");
}

int GlbStaticLoader::getIPRecordTypeInt(std::string rtStr) const{
    boost::unordered_map<std::string, int>::const_iterator it = strIPRecordTypeMap.find(rtStr);
    if (it == strIPRecordTypeMap.end()) {
        return IPRecordType::NONE;
    }
    return it->second;
}

std::string GlbStaticLoader::getGlbTypeString(int gt) const{
    switch (gt) {
        case GlbType::RANDOM:
            return std::string("RANDOM");
        case GlbType::GEOIP:
            return std::string("GEOIP");
        case GlbType::WEIGHTED:
            return std::string("WEIGHTED");
    }
    return std::string("NONE");
}

int GlbStaticLoader::getGlbTypeInt(std::string gtStr) const{
    boost::unordered_map<std::string, int>::const_iterator it = strGlbTypeMap.find(gtStr);
    if (it == strGlbTypeMap.end()) {
        return GlbType::NONE;
    }
    return it->second;
}

std::string GlbStaticLoader::to_string(const IPRecord &ipr)const {
    std::ostringstream os;
    os << "{ rType=" << getIPRecordTypeString(ipr.getRtype())
            << ", IPaddress=" << ipr.getIPAddress()
            << ", ttl=" << ipr.getTtl()
            << " }";
    return os.str();
}

std::string GlbStaticLoader::to_string(const GlbContainer &glb)const {
    return std::string("to_string() not implemented");
}