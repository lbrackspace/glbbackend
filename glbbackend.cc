#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/unordered_map.hpp>
#include<boost/thread/locks.hpp>
#include<boost/shared_ptr.hpp>
#include<iterator>
#include<vector>
#include<iostream>
#include<sstream>
#include<ostream>
#include "pdns/utility.hh"
#include "pdns/dnsbackend.hh"
#include "pdns/dnspacket.hh"
#include "pdns/logger.hh"
#include "glbbackend.hh"
#include "SOAContainer.hh"
#include "server.hh"
#include "ServerSingleton.hh"

const bool DEBUG = false;

GLBBackend::GLBBackend(const string & suffix) {
    setArgPrefix(suffix);
    string ip = getArg("server-ip");
    int port = getArgAsNum("server-port");
    ServerSingleton::getInstance(ip, port);
    sendSOA = false;
    sendNS = false;
    if (DEBUG) {
        cout << "new Thread" << endl;
    }
}

bool GLBBackend::list(const string &target, int id) {
    return false; // we don't support AXFR
}

std::string debugRR(const DNSResourceRecord &rr) {
    std::ostringstream os;
    os << "rr { content="
            << rr.content
            << " ttl=" << rr.ttl
            << " type=" << rr.qtype.getName()
            << " qname=" << rr.qname
            << " auth=" << std::boolalpha << rr.auth
            << " domain_id=" << rr.domain_id
            << " }";
    return os.str();
}

void GLBBackend::lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId) {
    // See if the domain even exists
    ips.clear(); // Initialize the list as empty
    d_ourname = qdomain;
    sendSOA = false;
    sendNS = false;
    if (DEBUG) {
        cout << "qType(" << type.getCode() << ")=" << type.getName() << " domain=" << qdomain << endl;
    }
    string domainLowerCase(boost::algorithm::to_lower_copy(qdomain));
    baseDomain = "";
    shared_ptr<GlbContainer> glb;
    if (type == QType::SOA || type == QType::ANY || type == QType::NS) {
        shared_ptr<SOAContainer> localSOA = getGlobalSOARecord();
        baseDomain = localSOA->getBaseFQDN();
        if (matchesBaseFqdn(domainLowerCase, baseDomain)) {
            soaRecord = localSOA->getSOARecord();
            if (DEBUG) {
                cout << "setting soaRecord = " << baseDomain << ":" << soaRecord << endl;
            }
        }

    }

    if (type == QType::SOA) {
        sendSOA = true;
    }
    if ((type == QType::NS || type == QType::ANY) && matchesBaseFqdn(domainLowerCase, baseDomain)) {
        if (DEBUG) {
            cout << "Adding NS records" << endl;
        }
        nsRecords = getNSRecords();
        int nns = nsRecords->size();
        if (DEBUG) {
            for (int i = 0; i < nns; i++) {
                cout << "ns: " << (*nsRecords)[i] << endl;
            }
        }
        nsIterator = nsRecords->begin();
        sendNS = true;
    }
    // Lock the glbMap and search for the domain
    {
        boost::shared_lock<boost::shared_mutex > (glbMapMutex);
        boost::unordered_map<string, shared_ptr<GlbContainer> >::iterator it = glbMap.find(domainLowerCase);


        if (it == glbMap.end()) {
            return;
        }
        glb = it->second;
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
    if (DEBUG) {
        cout << "ipType=" << ipType << endl;
    }
    glb->getIPs(ips, ipType); // getIPs will populate the ips deque
}

bool GLBBackend::get(DNSResourceRecord &rr) {
    if (sendSOA) {
        rr.content = soaRecord;
        rr.ttl = 60;
        rr.qname = baseDomain;
        rr.qtype = QType::SOA;
        rr.auth = true;
        rr.domain_id = 1;
        if (DEBUG) {
            cout << debugRR(rr) << endl;
        }
        sendSOA = false; // So we don't get stuck in an endless loop.
        return true;
    }
    if (sendNS) {
        if (DEBUG) {
            cout << "sendNS triggered" << endl;
            cout << "nsRecordsSize = " << nsRecords->size() << endl;
        }
        if (nsIterator == nsRecords->end()) {
            sendNS = false;
        } else {
            rr.content = *nsIterator;
            rr.ttl = 60;
            rr.qname = string("ns1." + baseDomain);
            rr.auth = true;
            rr.qtype = QType::NS;
            rr.domain_id = 1;
            nsIterator++;
            if (DEBUG) {
                cout << debugRR(rr) << endl;
            }
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
        rr.auth = true;
        rr.content = ipr.getIPAddress();
        rr.qname = d_ourname;
        // Pop the first record off the queue
        ips.pop_front();
        rr.domain_id = 1;
        if (DEBUG) {
            cout << debugRR(rr) << endl;
        }
        return true;
    }
    return false; // no more data
}

class GLBFactory : public BackendFactory {
public:

    GLBFactory(const string &name) : BackendFactory(name), d_name(name) {
    }

    void declareArguments(const string &suffix = "") {


        declare(suffix, "server-port", "Port for server to listen on for commands.", "8888");
        declare(suffix, "server-ip", "IP for server to listen on for commands." , "127.0.0.1");
    }

    DNSBackend *make(const string &suffix = "") {

        return new GLBBackend(d_name);
    }
private:
    string d_name;
};

class GLBLoader {
public:

    GLBLoader(const string &name) {
        BackendMakers().report(new GLBFactory(name));
        L << Logger::Warning << "[GLBBackend] This is the glbbackend version " << GLB_VERSION << " ("__DATE__", "__TIME__") reporting" << endl;
    }
};

static GLBLoader glbLoader("glb");

