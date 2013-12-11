#ifndef SOACONTAINER_H
#define	SOACONTAINER_H

#include<string>
#include<boost/shared_ptr.hpp>
#include<boost/thread/locks.hpp>
#include<boost/thread.hpp>

class SOAContainer {
 private:
  std::string rawSOA;
  std::string baseFQDN;
  std::string nameserver;
  std::string email;
  uint32_t serial;
  uint32_t refresh;
  uint32_t retry;
  uint32_t expire;
  uint32_t defaultTTL;

  void buildFromRawSOA();

 public:
  SOAContainer(std::string fqdn, std::string rawSOA);
  SOAContainer(std::string fqdn, std::string sourceHost, std::string email,
               unsigned int serial, int refresh, int retry, int expire,
               int minTTL);
  SOAContainer(SOAContainer& orig);
  std::string getRawSOA();
  std::string getBaseFQDN();
  std::string getNameserver();
  std::string getEmail();
  uint32_t getSerial();
  uint32_t getRefresh();
  uint32_t getRetry();
  uint32_t getExpire();
  uint32_t getDefaultTTL();
  virtual ~SOAContainer();

};

extern boost::shared_ptr<SOAContainer> globalSOA;
extern boost::shared_mutex soaMutex;
extern boost::shared_ptr<std::vector<std::string> > globalNsRecords;
void setGlobalSOARecord(const std::string &soaValue,
                        const std::string &baseFQDNValue);
boost::shared_ptr<SOAContainer> getGlobalSOARecord();
void setNSRecords(std::vector<std::string> recs);
boost::shared_ptr<std::vector<std::string> > getNSRecords();

#endif	/* SOACONTAINER_H */

