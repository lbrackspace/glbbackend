#ifndef GLBSTATICLOADER_H
#define	GLBSTATICLOADER_H

#include<string>
#include<boost/unordered_map.hpp>

#include"IPRecord.h"
#include"GlbContainer.h"

class GlbStaticLoader {
private:
    boost::unordered_map<std::string, int> strIPRecordTypeMap;
    boost::unordered_map<std::string, int> strGlbTypeMap;

    GlbStaticLoader() {
        strIPRecordTypeMap["NONE"] = IPRecordType::NONE;
        strIPRecordTypeMap["IPv4"] = IPRecordType::IPv4;
        strIPRecordTypeMap["IPv6"] = IPRecordType::IPv6;
        strGlbTypeMap["NONE"] = IPRecordType::NONE;
        strGlbTypeMap["RANDOM"] = GlbType::RANDOM;
        strGlbTypeMap["GEOIP"] = GlbType::GEOIP;
        strGlbTypeMap["WEIGHTED"] = GlbType::WEIGHTED;
    }
public:

    static GlbStaticLoader& get() {
        static GlbStaticLoader instance;
        return instance;
    }

    std::string getIPRecordTypeString(int rt) const;
    int getIPRecordTypeInt(std::string rtStr) const;

    std::string getGlbTypeString(int gt)const;
    int getGlbTypeInt(std::string gtStr)const;

    std::string to_string(const GlbContainer &glb)const;
    std::string to_string(const IPRecord &glb)const;
};


#endif	/* GLBSTATICLOADER_H */

