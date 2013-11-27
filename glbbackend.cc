#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include "pdns/utility.hh"
#include "pdns/dnsbackend.hh"
#include "pdns/dnspacket.hh"
#include "pdns/logger.hh"
#include "glbbackend.hh"
#include "server.hh"

GLBBackend::GLBBackend(const string & suffix) {
  /*std::string ip_addr_str = "127.0.0.1";
  int port = 8002;
  boost::asio::io_service ios;
  boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(ip_addr_str), port);
  boost::asio::ip::tcp::acceptor ac(ios, ep);

  boost::shared_ptr<boost::asio::ip::tcp::iostream> stream(new boost::asio::ip::tcp::iostream());
  ac.accept(*stream->rdbuf());
  boost::thread th(bind(server, stream));
  th.detach();*/
}

bool GLBBackend::list(const string &target, int id) {
  return false; // we don't support AXFR
}

void GLBBackend::lookup(const QType &type, const string &qdomain, DNSPacket *p, int zoneId)
{
    L << Logger::Debug << "lookup called on " << qdomain << " for type " << type.getName() << std::endl;
    ostringstream os;
    os<<Utility::random()%256<<"."<<Utility::random()%256<<"."<<Utility::random()%256<<"."<<Utility::random()%256;
    d_answer=os.str();                                           // our random ip address
}

bool GLBBackend::get(DNSResourceRecord &rr)
{
  if(!d_answer.empty()) {
    rr.qname=d_ourname;                                           // fill in details
    rr.qtype=QType::A;                                            // A record
    rr.ttl=5;                                                 // 5 seconds
    rr.auth = 1;  // it may be random.. but it is auth!
    rr.content=d_answer;

    d_answer="";                                                  // this was the last answer

    return true;
  }
  return false;                                                   // no more data
}

class GLBFactory : public BackendFactory
{
public:
  GLBFactory() : BackendFactory("glb") {}
  void declareArguments(const string &suffix="")
  {
    declare(suffix,"darkside","Something something something darkside.","vader");
  }
  DNSBackend *make(const string &suffix="")
  {
    return new GLBBackend(suffix);
  }
};

class GLBLoader
{
public:
  GLBLoader()
  {
    BackendMakers().report(new GLBFactory);
    
    L<<Logger::Warning<<" [GLBBackend] This is the glbbackend version " << GLB_VERSION << " ("__DATE__", "__TIME__") reporting" << endl;
  }  
};

static GLBLoader glbLoader;

