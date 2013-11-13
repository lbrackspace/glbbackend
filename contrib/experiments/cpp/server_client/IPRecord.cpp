#include "IPRecord.h"

#include<boost/unordered_map.hpp>
#include<boost/assign.hpp>
#include<string>
#include<sstream>

boost::unordered_map<std::string, IPRecordType> stringToRecordMap = boost::assign::map_list_of("IPv4", IPv4)("IPv6", IPv6);

std::string IPRecord::to_string() {
    return std::string("");
}

std::string ipRecordTypeToString(const IPRecordType rt) {
    switch (rt) {
        case IPv4:
            return std::string("IPv4");
        case IPv6:
            return std::string("IPv6");
        default:
            return std::string("NONE");
    }
}

IPRecordType stringToIPRecordType(const std::string& rts) {
    return NONE;
}