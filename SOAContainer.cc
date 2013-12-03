#include "SOAContainer.hh"
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
#include<boost/thread/locks.hpp>
#include<boost/thread.hpp>

void setSOA(const std::string &soaValue, const std::string &baseFQDNValue) {
    using namespace boost;
    lock_guard<shared_mutex> lock(soaMutex);
    globalSOA = shared_ptr<SOAContainer > (new SOAContainer(soaValue, baseFQDNValue));
}

boost::shared_ptr<SOAContainer> getSOA() {
    using namespace boost;
    shared_lock<shared_mutex> lock(soaMutex);
    return shared_ptr<SOAContainer > (globalSOA);
}

void setNSRecords(std::vector< std::string> recs) {
    using namespace boost;
    using namespace std;
    shared_ptr<vector<string> > newRecords(new vector<string>);
    vector<string>::iterator it;
    for (it = recs.begin(); it != recs.end(); it++) {
        newRecords->push_back(*it);
    }

    { // Lock and set the new NSRecords pointer
        lock_guard<shared_mutex> lock(soaMutex);
        nsRecords = newRecords;
    }
}

boost::shared_ptr< std::vector< std::string> > getNSRecords() {
    using namespace boost;
    using namespace std;
    shared_lock<shared_mutex> lock(soaMutex);
    return nsRecords;
}



boost::shared_ptr<SOAContainer>globalSOA = boost::shared_ptr<SOAContainer>(new SOAContainer("", ""));
boost::shared_mutex soaMutex;

boost::shared_ptr< std::vector<std::string> > nsRecords(new std::vector<std::string>);

