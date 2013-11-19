#include "IPRecord.h"
#include "GlbContainer.h"

#include<sstream>


static boost::unordered_map<std::string, int> strToIpTypeMap;

boost::unordered_map<std::string, int> buildStrToIpTypeMap() {
    boost::unordered_map<std::string, int> m;
    m["NONE"] = IPRecordType::NONE;
    m["IPv4"] = IPRecordType::IPv4;
    m["IPv6"] = IPRecordType::IPv6;
    return m;
}

std::string IPRecord::to_string() {
    std::ostringstream os;
    os << "{ ipType=" << ipTypeToStr(ipType)
            << ", ip=" << ipAddress
            << ", ttl=" << ttl
            << "}";
    return os.str();
}

int strToIpType(std::string str) {
    static boost::unordered_map<std::string, int> strToIpTypeMap = buildStrToIpTypeMap();
    boost::unordered_map<std::string, int>::const_iterator it = strToIpTypeMap.find(str);
    if (it == strToIpTypeMap.end()) {
        return IPRecordType::NONE;
    }
    return it->second;
}

std::string ipTypeToStr(int ipt) {
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

