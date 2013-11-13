#ifndef IPRECORD_H
#define	IPRECORD_H

#include<string>
#include<boost/unordered_map.hpp>

enum IPRecordType {
    IPv4 = 1, IPv6 = 2
};

class IPRecord {
private:
    IPRecordType rType;
    std::string IPaddress;
    int ttl;
public:

    IPRecord(IPRecordType rt, std::string ipa, int tl) {
        rType = rt;
        IPaddress = ipa;
        ttl = tl;
    }

    IPRecord() {
    }

    IPRecord(const IPRecord& orig) {
        this->ttl = orig.ttl;
        this->rType = orig.rType;
        this->IPaddress = orig.IPaddress;
    }
    virtual ~IPRecord();

    IPRecordType getRtype() const {
        return rType;
    }

    std::string getIPAddress() const {
        return IPaddress;
    }

    int getTtl() const {
        return ttl;
    }

    void setRtype(IPRecordType rt) {
        rType = rt;
    }

    void setTtl(int tl) {
        ttl = tl;
    }

    void setIPAddress(std::string ipa) {
        IPaddress = ipa;
    }

    std::string to_string();
};

std::string ipRecordTypeToString(const IPRecordType rt);
IPRecordType stringToIPRecordType(const std::string& rts);

extern boost::unordered_map<std::string, IPRecordType> stringToRecordMap;

#endif	/* IPRECORD_H */

