#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <pdns/ext/rapidjson/include/rapidjson/stringbuffer.h>
#include <pdns/ext/rapidjson/include/rapidjson/filestream.h>
#include <pdns/ext/rapidjson/include/rapidjson/writer.h>
#include <pdns/ext/rapidjson/include/rapidjson/document.h>
#include "ServerJsonBuilder.hh"

ServerJsonBuilder::ServerJsonBuilder() {
  hasError = false;
  hasType = false;
  hasFqdn = false;
  hasErrors = false;
  hasCounts = false;
  hasIps = false;
  hasNsRecords = false;
}

void ServerJsonBuilder::setFqdn(std::string _fqdn) {
  fqdn = _fqdn;
  hasFqdn = true;
}

void ServerJsonBuilder::setType(std::string _type) {
  type = _type;
  hasType = true;
}

void ServerJsonBuilder::setError(std::string _error) {
  error = _error;
  hasError = true;
}

void ServerJsonBuilder::addError(std::string errorEntry) {
  errors.push_back(errorEntry);
  hasErrors = true;
}

void ServerJsonBuilder::setStatus(std::string _status) {
  status = _status;
  hasStatus = true;
}

void ServerJsonBuilder::addNSRecord(std::string ns) {
  nsRecords.push_back(ns);
  hasNsRecords = true;
}

void ServerJsonBuilder::addCount(std::string fqdn, long count) {
  FqdnCount fc;
  fc.fqdn = fqdn;
  fc.count = count;
  counts.push_back(fc);
  hasCounts = true;
}

void ServerJsonBuilder::addIp(std::string ip, std::string _error) {
  IpJson ij;
  ij.ip = ip;
  if (_error.compare("") == 0) {
    ij.hasError = false;
  } else {
    ij.error = _error;
    ij.hasError = true;
  }
  ips.push_back(ij);
  hasIps = true;
}

std::string ServerJsonBuilder::toJson() {
  rapidjson::Document doc;
  rapidjson::Document::AllocatorType& aloc = doc.GetAllocator();
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> wr(sb);
  doc.SetObject();
  if (hasType)
    doc.AddMember("type", type.c_str(), aloc);
  if (hasFqdn)
    doc.AddMember("fqdn", fqdn.c_str(), aloc);
  if (hasStatus)
    doc.AddMember("status", status.c_str(), aloc);
  if (hasError)
    doc.AddMember("error", error.c_str(), aloc);
  if (hasCounts) {
    rapidjson::Value countArray;
    countArray.SetArray();
    std::vector<FqdnCount>::iterator it;
    for (it = counts.begin(); it != counts.end(); it++) {
      rapidjson::Value countElement;
      countElement.SetArray();
      rapidjson::Value countAsValue;
      countAsValue.SetArray();
      rapidjson::Value fqdnAsValue;
      fqdnAsValue.SetString((*it).fqdn.c_str());
      rapidjson::Value countJsonLong;
      countJsonLong.SetInt64((*it).count);
      countElement.PushBack(fqdnAsValue, aloc);
      countElement.PushBack(countJsonLong, aloc);
      countArray.PushBack(countElement, aloc);

    }
    doc.AddMember("counts", countArray, aloc);
  }
  if (hasErrors) {
    rapidjson::Value jsonArray;
    jsonArray.SetArray();
    std::vector<std::string>::iterator it;
    for (it = errors.begin(); it != errors.end(); it++) {
      rapidjson::Value errorMsg;
      errorMsg.SetString((*it).c_str());
      jsonArray.PushBack(errorMsg, aloc);
    }
    doc.AddMember("errors", jsonArray, aloc);
  }
  if (hasIps) {
    rapidjson::Value ipArray;
    ipArray.SetArray();
    std::vector<IpJson>::iterator it;
    for (it = ips.begin(); it != ips.end(); it++) {
      IpJson ij = *it;
      rapidjson::Value ipObj;
      ipObj.SetObject();
      rapidjson::Value ip;
      ip.SetString(ij.ip.c_str());
      ipObj.AddMember("ip", ip, aloc);
      if (ij.hasError) {
        rapidjson::Value _error;
        _error.SetString(ij.error.c_str());
        ipObj.AddMember("error", _error, aloc);
      }
      ipArray.PushBack(ipObj, aloc);
    }
    doc.AddMember("ips", ipArray, aloc);
  }
  doc.Accept(wr);
  return std::string(sb.GetString());
}

ServerJsonBuilder::~ServerJsonBuilder() {
  // TODO Auto-generated destructor stub
}

