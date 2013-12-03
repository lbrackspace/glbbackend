#ifndef SOACONTAINER_H
#define	SOACONTAINER_H

#include<string>
#include<boost/shared_ptr.hpp>
#include<boost/thread/locks.hpp>
#include<boost/thread.hpp>

class SOAContainer {
private:
    std::string soaRecord;
    std::string baseFQDN;

public:

    SOAContainer(std::string soa, std::string fqdn) : soaRecord(soa), baseFQDN(fqdn) {
    }

    SOAContainer(const SOAContainer& orig) {
        soaRecord = orig.soaRecord;
        baseFQDN = orig.baseFQDN;
    }

    std::string getSOARecord() {
        return soaRecord;
    }

    std::string getBaseFQDN() {
        return baseFQDN;
    }

    virtual ~SOAContainer() {
    }

};

extern boost::shared_ptr<SOAContainer> globalSOA;
extern boost::shared_mutex soaMutex;
extern boost::shared_ptr< std::vector< std::string > > nsRecords;
void setSOA(const std::string &soaValue, const std::string &baseFQDNValue);
boost::shared_ptr<SOAContainer> getSOA();

void setNSRecords(std::vector< std::string> recs);
boost::shared_ptr< std::vector< std::string> > getNSRecords();


#endif	/* SOACONTAINER_H */

