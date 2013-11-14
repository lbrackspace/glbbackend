#ifndef IPRECORD_H
#define	IPRECORD_H

#include<string>
#include<boost/unordered_map.hpp>


// Just trying to emulate a scoped enum in pre C++11
namespace IPRecordType {
    const int NONE = 0;
    const int IPv4 = 1;
    const int IPv6 = 2;
}

class IPRecord {
private:
    int rType;
    std::string IPaddress;
    int ttl;
public:

    IPRecord(int rt, std::string ipa, int tl) {
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

    virtual ~IPRecord() {
    }

    int getRtype() const {
        return rType;
    }

    std::string getIPAddress() const {
        return IPaddress;
    }

    int getTtl() const {
        return ttl;
    }

    void setRtype(int rt) {
        rType = rt;
    }

    void setTtl(int tl) {
        ttl = tl;
    }

    void setIPAddress(std::string ipa) {
        IPaddress = ipa;
    }

};
#endif	/* IPRECORD_H */

