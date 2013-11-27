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
    GLBBackend(const string &suffix = "");
    bool list(const string &target, int id);
    void lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId);
    bool get(DNSResourceRecord &rr);

private:
    std::deque<IPRecord> ips;
    string d_answer;
    string d_ourname;
};



#endif /* GLBBACKEND_HH_ */
