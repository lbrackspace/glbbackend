/*
 * glbbackend.hh
 *
 *  Created on: Oct 21, 2013
 *      Author: brandon
 */

#ifndef GLBBACKEND_HH_
#define GLBBACKEND_HH_
#define GLB_VERSION 0.01
#include <string>
#include <map>
//#include "pdns/backends/gsql/gsqlbackend.hh"
#include "GlbContainer.hh"
#include "IPRecord.hh"
#include "pdns/namespaces.hh"

class GLBBackend : public DNSBackend {
public:
    GLBBackend(const string &suffix);
    bool getSOA(const string &name, SOAData &soadata, DNSPacket *p);
    bool list(const string &target, int id);
    void lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId);
    bool get(DNSResourceRecord &rr);

private:
    bool sendSOA;
    bool sendNS;
    std::deque<IPRecord> ips;
    boost::shared_ptr<std::vector<std::string> > nsRecords;
    std::vector<std::string>::iterator nsIterator;
    std::string d_answer;
    std::string d_ourname;
    std::string soaRecord;
    std::string baseDomain;
};

std::string debugRR(const DNSResourceRecord &rr);

#endif /* GLBBACKEND_HH_ */
