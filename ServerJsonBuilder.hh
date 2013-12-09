#ifndef SERVERJSONBUILDER_HH_
#define SERVERJSONBUILDER_HH_

#include<string>

struct FqdnCount {
    std::string fqdn;
    long count;
};

struct IpJson {
    bool hasError;
    std::string error;
    std::string ip;
};

class ServerJsonBuilder {
 public:
  ServerJsonBuilder();
  void setFqdn(std::string _fqdn);
  void setType(std::string _type);
  void setError(std::string _error);
  void addError(std::string errorEntry);
  void setStatus(std::string _status);
  void addNSRecord(std::string ns);
  void addCount(std::string fqdn, long count);
  void addIp(std::string ip, std::string _error);
  std::string toJson();
  virtual ~ServerJsonBuilder();

 private:
  std::string error;
  std::string type;
  std::string fqdn;
  std::string status;
  std::vector<FqdnCount> counts;
  std::vector<std::string> errors;
  std::vector<IpJson> ips;
  std::vector<std::string> nsRecords;
  bool hasError;
  bool hasType;
  bool hasFqdn;
  bool hasStatus;
  bool hasCounts;
  bool hasErrors;
  bool hasIps;
  bool hasNsRecords;
};

#endif /* SERVERJSONBUILDER_HH_ */
