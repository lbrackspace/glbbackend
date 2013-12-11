#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "SOAContainer.hh"

void setGlobalSOARecord(const std::string &baseFQDNValue,
                        const std::string &soaValue) {
  using namespace boost;
  lock_guard<shared_mutex> lock(soaMutex);
  globalSOA = shared_ptr<SOAContainer>(
      new SOAContainer(baseFQDNValue, soaValue));
}

boost::shared_ptr<SOAContainer> getGlobalSOARecord() {
  using namespace boost;
  shared_lock<shared_mutex> lock(soaMutex);
  return shared_ptr<SOAContainer>(globalSOA);
}

void setNSRecords(std::vector<std::string> recs) {
  using namespace boost;
  using namespace std;
  shared_ptr<vector<string> > newRecords(new vector<string>);
  vector<string>::iterator it;
  for (it = recs.begin(); it != recs.end(); it++) {
    newRecords->push_back(*it);
  }

  {  // Lock and set the new NSRecords pointer
    lock_guard<shared_mutex> lock(soaMutex);
    globalNsRecords = newRecords;
  }
}

boost::shared_ptr<std::vector<std::string> > getNSRecords() {
  using namespace boost;
  using namespace std;
  shared_lock<shared_mutex> lock(soaMutex);
  return globalNsRecords;
}

boost::shared_ptr<SOAContainer> globalSOA = boost::shared_ptr<SOAContainer>(
    new SOAContainer("", ""));
boost::shared_mutex soaMutex;

boost::shared_ptr<std::vector<std::string> > globalNsRecords(
    new std::vector<std::string>);

SOAContainer::SOAContainer(std::string fqdn, std::string rawSOA) {
  this->rawSOA = rawSOA;
  this->baseFQDN = fqdn;
  this->buildFromRawSOA();
}

SOAContainer::SOAContainer(std::string fqdn, std::string sourceHost,
                           std::string email, unsigned int serial, int refresh,
                           int retry, int expire, int minTTL) {
  this->baseFQDN = fqdn;
  this->nameserver = sourceHost;
  this->email = email;
  this->serial = serial;
  this->refresh = refresh;
  this->retry = retry;
  this->expire = expire;
  this->defaultTTL = minTTL;
}

SOAContainer::SOAContainer(SOAContainer& orig) {
  this->rawSOA = orig.getRawSOA();
  this->baseFQDN = orig.getBaseFQDN();
  this->nameserver = orig.getNameserver();
  this->email = orig.getEmail();
  this->serial = orig.getSerial();
  this->refresh = orig.getRefresh();
  this->retry = orig.getRetry();
  this->expire = orig.getExpire();
  this->defaultTTL = orig.getDefaultTTL();
}

std::string SOAContainer::getRawSOA() {
  return rawSOA;
}

std::string SOAContainer::getBaseFQDN() {
  return baseFQDN;
}

std::string SOAContainer::getNameserver() {
  return nameserver;
}

std::string SOAContainer::getEmail() {
  return email;
}

uint32_t SOAContainer::getSerial() {
  return serial;
}

uint32_t SOAContainer::getRefresh() {
  return refresh;
}

uint32_t SOAContainer::getRetry() {
  return retry;
}

uint32_t SOAContainer::getExpire() {
  return expire;
}

uint32_t SOAContainer::getDefaultTTL() {
  return defaultTTL;
}

void SOAContainer::buildFromRawSOA() {
  std::string result = boost::algorithm::replace_all_copy(rawSOA, "(", "");
  boost::algorithm::trim(result);
  if(result.length() == 0){
    std::cout << "Error: SOA string is empty." << std::endl;
    return;
  }
  boost::algorithm::replace_all(result, ")", "");
  std::vector<std::string> tokens;
  boost::algorithm::split(tokens, result, boost::algorithm::is_any_of(" "), boost::token_compress_on);
  if(tokens.size() < 7){
    std::cout << "Error: SOA string missing components." << std::endl;
    return;
  }
  this->nameserver = tokens[0];
  this->email = tokens[1];
  try{
    this->serial = boost::lexical_cast<unsigned int>(tokens[2]);
    this->refresh = boost::lexical_cast<int>(tokens[3]);
    this->retry = boost::lexical_cast<int>(tokens[4]);
    this->expire = boost::lexical_cast<int>(tokens[5]);
    this->defaultTTL = boost::lexical_cast<int>(tokens[6]);
  } catch(boost::bad_lexical_cast const&){
    std::cout << "Error: SOA string was not in correct format.";
  }
}

SOAContainer::~SOAContainer() {
}
