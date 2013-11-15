#ifndef IPRECORD_H
#define	IPRECORD_H

#include<sstream>
#include<string>
#include<boost/unordered_map.hpp>

class IPRecord {
private:
    std::string ipAddress;
    int ttl;
public:

    IPRecord(int rt, std::string ipa, int tl) {
        ipAddress = ipa;
        ttl = tl;
    }

    IPRecord() {
    }

    IPRecord(const IPRecord& orig) {
        this->ttl = orig.ttl;
        this->ipAddress = orig.ipAddress;
    }

    virtual ~IPRecord() {
    }

    std::string getIPAddress() const {
        return ipAddress;
    }

    int getTtl() const {
        return ttl;
    }

    void setTtl(int tl) {
        ttl = tl;
    }

    void setIPAddress(std::string ipa) {
        ipAddress = ipa;
    }

    std::string to_string() {
        std::ostringstream os;
        os << "{ ipAddress = " << ipAddress << ", ttl=" << ttl;
        return os.str();
    }

};
#endif	/* IPRECORD_H */

