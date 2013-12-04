#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/shared_ptr.hpp>
#include<iterator>
#include<vector>
#include<iostream>
#include "pdns/utility.hh"
#include "pdns/dnsbackend.hh"
#include "pdns/dnspacket.hh"
#include "pdns/logger.hh"
#include "glbbackend.hh"
#include "SOAContainer.hh"
#include "server.hh"

GLBBackend::GLBBackend(const string & suffix) {
    sendSOA = false;
    sendNS = false;
}

bool GLBBackend::list(const string &target, int id) {
    return false; // we don't support AXFR
}

void GLBBackend::lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId) {
    // See if the domain even exists
    ips.clear(); // Initialize the list as empty
    d_ourname = qdomain;
    sendSOA = false;
    sendNS = false;
    cout << "qType(" << type.getCode() << ")=" << type.getName() << " domain=" << qdomain << endl;
    string domainLowerCase(boost::algorithm::to_lower_copy(qdomain));
    string baseFQDN;
    shared_ptr<GlbContainer> glb;
    {
        boost::shared_lock<boost::shared_mutex > (glbMapMutex);
        boost::unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(domainLowerCase);


        if (it == glbMap.end()) {
            return;
        }
        glb = it->second;
    }
    if (type == QType::SOA || type == QType::ANY || type == QType::NS) {
        shared_ptr<SOAContainer> localSOA = getGlobalSOARecord();
        baseFQDN = localSOA->getBaseFQDN();
        if (matchesBaseFqdn(domainLowerCase, baseFQDN)) {
            soaRecord = localSOA->getSOARecord();
        }

    }
    if (type == QType::SOA) {
        sendSOA = true;
    }
    if ((type == QType::NS || type == QType::ANY) && matchesBaseFqdn(domainLowerCase, baseFQDN)) {
        nsRecords = getNSRecords();
        nsIterator = nsRecords->begin();
        sendNS = true;
    }

    int ipType;
    if (type == QType::A) {
        ipType = IPRecordType::IPv4;
    } else if (type == QType::AAAA) {
        ipType = IPRecordType::IPv6;
    } else if (type == QType::ANY) {
        ipType = IPRecordType::IPv4 | IPRecordType::IPv6;
    } else {

        return; // leave the list empty Its not an A record or a AAAA record. :|
    }
    cout << "ipType=" << ipType << endl;
    (*glb).getIPs(ips, ipType); // getIPs will populate the ips deque
}

bool GLBBackend::get(DNSResourceRecord &rr) {
    if (sendSOA) {
        rr.content = soaRecord;
        rr.ttl = 60;
        sendSOA = false; // So we don't get stuck in an endless loop.
        rr.qname = d_ourname;
        rr.auth = 1;
        return true;
    }
    if (sendNS) {
        if (nsIterator == nsRecords->end()) {
            sendNS = false;
        } else {
            rr.content = *nsIterator;
            rr.ttl = 60;
            rr.qname = d_ourname;
            rr.auth = 1;
            nsIterator++;
            return true;
        }
    }
    if (ips.size() > 0) {
        IPRecord ipr = *(ips.begin());
        switch (ipr.getIPType()) {
            case IPRecordType::IPv4:
                rr.qtype = QType::A;
                break;
            case IPRecordType::IPv6:
                rr.qtype = QType::AAAA;
                break;
        }
        rr.ttl = ipr.getTtl();
        rr.auth = 1;
        rr.content = ipr.getIPAddress();
        rr.qname = d_ourname;
        // Pop the first record off the queue
        ips.pop_front();

        return true;
    }
    return false; // no more data
}

class GLBFactory : public BackendFactory {
public:

    GLBFactory() : BackendFactory("glb") {
    }

    void declareArguments(const string &suffix = "") {

        declare(suffix, "darkside", "Something something something darkside.", "vader");
    }

    DNSBackend *make(const string &suffix = "") {

        return new GLBBackend(suffix);
    }
};

class GLBLoader {
public:

    GLBLoader() {
        BackendMakers().report(new GLBFactory);
        L << Logger::Warning << " [GLBBackend] This is the glbbackend version " << GLB_VERSION << " ("__DATE__", "__TIME__") reporting" << endl;
        cout << "Spinning up server" << endl;
        start_server_thread("127.0.0.1", 8888);
        cout << "Server running" << endl;
    }
};

static GLBLoader glbLoader;

