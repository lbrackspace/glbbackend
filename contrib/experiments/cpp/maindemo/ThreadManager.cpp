#include "ThreadManager.h"
#include<boost/thread.hpp>
#include<boost/ref.hpp>
#include<ostream>
#include<functional>

using namespace std;
using namespace boost;

const posix_time::seconds oneSecond(1);

ThreadManager::ThreadManager(ostream& _os) : os(_os) {
    os << "Initialized ThreadManager" << endl;
}

ThreadManager::~ThreadManager() {
    os << "ThreadManager destructor called";
    for (int i = 0; i < threads.size(); i++) {
        threads[i]->interrupt();
        delete threads[i];
    }
}

void ThreadManager::newThread(int sleepSecs, int nTimes) {
    thread *th = new thread(runner, boost::ref(os), threads.size() + 1, sleepSecs, nTimes);
    threads.push_back(th);
}

void ThreadManager::detachThreads() {
    int i;
    int nThreads = threads.size();
    os << "Detaching " << nThreads << " threads" << endl;
    for (i = 0; i < nThreads; i++) {
        threads[i]->detach();
        delete threads[i];
    }
    threads.clear();
}

void ThreadManager::joinThreads() {
    int i;
    int nThreads = threads.size();
    cout << "Joining " << nThreads << " threads" << endl;
    for (i = 0; i < nThreads; i++) {
        os << "Joining thread[" << i << "]=" << threads[i]->get_id() << " " << i << " of " << nThreads << endl;
        threads[i]->join();
        delete threads[i];
    }
    threads.clear();
}

int ThreadManager::getNThreads() const {
    return threads.size();
}

string ThreadManager::to_string() {
    ostringstream strOut;
    int i;
    int nThreads = threads.size();
    for (i = 0; i < nThreads; i++) {
        strOut << "thread[" << i << "]=" << threads[i]->get_id() << endl;
    }
    return strOut.str();
}

void runner(ostream& stream, int tId, int nSecs, int nTimes) {
    int i;
    int j;
    static boost::mutex coutMtx;
    for (i = 0; i < nTimes; i++) {
        for (j = 0; j < nSecs; j++) {
            this_thread::sleep(oneSecond);
        }
        coutMtx.lock();
        stream << "thread[" << tId << "]: " << this_thread::get_id() << " slept for " << nSecs << " seconds " << i << " of " << nTimes << endl;
        coutMtx.unlock();
    }
}
