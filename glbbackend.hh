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
#include "pdns/namespaces.hh"

/** The gMySQLBackend is a DNSBackend that can answer DNS related questions. It looks up data
    in MySQL */
class GLBBackend : public DNSBackend
{
public:
  GLBBackend(const string &suffix); //!< Makes our connection to the database. Throws an exception if it fails.
  bool list(const string &target, int id);
  void lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId);
  bool get(DNSResourceRecord &rr);

private:
  string d_answer;
  string d_ourname;
};



#endif /* GLBBACKEND_HH_ */
