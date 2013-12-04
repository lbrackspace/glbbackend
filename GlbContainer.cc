#include "GlbContainer.hh"
#include "IPRecord.hh"
#include "contrib/experiments/cpp/server_client/GlbContainer.hh"

#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/thread.hpp>
#include<boost/random/mersenne_twister.hpp>
#include<boost/random/uniform_int.hpp>
#include<boost/random/variate_generator.hpp>
#include<boost/algorithm/string.hpp>
#include<iostream>
#include<vector>
#include<string>

static boost::unordered_map<std::string, int> strToGlbTypeMap;

boost::mt19937 rng(static_cast<boost::uint32_t> (std::time(0)));
boost::uniform_int<int> ud;
boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rn(rng, ud);

boost::unordered_map<std::string, int> buildStrToGlbTypeMap() {
    boost::unordered_map<std::string, int> m;
    m["NONE"] = GlbType::NONE;
    m["RANDOM"] = GlbType::RANDOM;
    m["GEOIP"] = GlbType::GEOIP;
    m["WEIGHTED"] = GlbType::WEIGHTED;
    return m;
}

std::string GlbContainer::to_string() {
    return to_string(false);
}

std::string GlbContainer::to_string(bool showIps) {
    std::ostringstream os;
    boost::shared_lock<boost::shared_mutex> sl(glbMutex);
    os << "{ cname=" << cname
            << ", nsLookups=" << nLookups
            << ", glbType=" << glbTypeToStr(glbType)
            << ", nIp=" << ips.size()
            << "}";
    if (showIps) {
        std::vector<boost::shared_ptr<IPRecord> >::iterator it;
        std::vector<boost::shared_ptr<IPRecord> >::iterator end;
        end = ips.end();
        for (it = ips.begin(); it != end; it++) {
            IPRecord *ptr = (*it).get();
            os << ptr->to_string() << " ";
        }
    }
    return os.str();
}

int strToGlbType(std::string str) {
    static boost::unordered_map<std::string, int> strToGlbTypeMap = buildStrToGlbTypeMap();
    boost::unordered_map<std::string, int>::const_iterator it = strToGlbTypeMap.find(str);
    if (it == strToGlbTypeMap.end()) {
        return -1;
    }
    return it->second;
}

std::string glbTypeToStr(int gt) {
    switch (gt) {
        case GlbType::NONE:
            return std::string("NONE");
        case GlbType::GEOIP:
            return std::string("GEOIP");
        case GlbType::RANDOM:
            return std::string("RANDOM");
        case GlbType::WEIGHTED:
            return std::string("WEIGHTED");
    }
    return std::string("NONE");
}

void GlbContainer::setIPs(std::vector<IPRecord>& ipsIn) {
    boost::lock_guard<boost::shared_mutex> lock(glbMutex);
    std::vector<IPRecord>::iterator beg = ipsIn.begin();
    std::vector<IPRecord>::iterator end = ipsIn.end();
    std::vector<IPRecord>::iterator it;
    for (it = beg; it != end; it++) {
        ips.push_back(boost::shared_ptr<IPRecord > (new IPRecord(*it)));
    }
}

void expandweights(std::vector<int> &expanded, std::vector<int>&weights) {
    int ws = weights.size();
    expanded.clear();
    for (int i = 0; i < ws; i++) {
        int weight = weights[i];
        for (int j = 0; j < weight; j++) {
            weights.push_back(i);
        }
    }
}

void GlbContainer::getIPs(std::deque<IPRecord>& dq, int ipType) {
    incNLookups();
    switch (glbType) {
        case GlbType::NONE:
            getIPSNoneAlgo(dq, ipType);
            return;
        case GlbType::RANDOM:
            getIPSRandomAlgo(dq, ipType);
    }
}

void GlbContainer::getIPSNoneAlgo(std::deque<IPRecord>& dq, int ipType) {
    boost::shared_lock<boost::shared_mutex> lock(glbMutex);
    int nRecords = ips.size();
    for (int i = 0; i < nRecords; i++) {
        if (((ips[i]->getIPType()) & ipType) > 0) {
            dq.push_back(*(ips[i])); // Copy the ip into the deque
        }
    }
}

void GlbContainer::getIPSRandomAlgo(std::deque<IPRecord>& dq, int ipType) {
    boost::shared_lock<boost::shared_mutex> lock(glbMutex);
    std::vector<int> ip_indexes;
    int nRecords = ips.size();
    for (int i = 0; i < nRecords; i++) {
        if (ips[i]->getIPType() & ipType != 0) {
            ip_indexes.push_back(i);
        }
    }
    random_shuffle(ip_indexes.begin(), ip_indexes.end(), rn);
    int nShuffled = ip_indexes.size();
    for (int i = 0; i < nShuffled; i++) {
        dq.push_back(*(ips[ip_indexes[i]]));
    }
}


// Besure you don't have all vectors weighted at zero as this triggers division by zero

int gcdreduce(std::vector<int> &reduced_weights, std::vector<int> &weights, int& itercount) {
    int r = 0;
    int nWeights = weights.size();
    if (nWeights <= 0) {
        return 0;
    }
    r = weights[0];
    itercount++;
    for (int i = 1; i < nWeights; i++) {
        int a = r;
        int b = weights[i];
        while (a != 0) {
            itercount++;
            int c = a;
            a = b % a;
            b = c;
        }
        r = b;
    }
    reduced_weights.clear();
    for (int i = 0; i < nWeights; i++) {
        reduced_weights.push_back(weights[i] / r);
    }
    return r;
}

bool matchesBaseFqdn(const std::string &fqdn, const std::string &baseFqdn) {
    std::string dotBase("." + baseFqdn);
    if (boost::algorithm::ends_with(fqdn, dotBase)) {
        return true;
    }
    if (fqdn.compare(baseFqdn) == 0) {
        return true;
    }
    return false;
}

boost::unordered_map<std::string, boost::shared_ptr<GlbContainer> > glbMap;
boost::shared_mutex glbMapMutex;
