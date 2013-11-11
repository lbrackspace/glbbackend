/* 
 * File:   ThreadManager.h
 * Author: crc
 *
 * Created on October 25, 2013, 6:28 PM
 */

#ifndef THREADMANAGER_H
#define	THREADMANAGER_H

#include<boost/thread.hpp>
#include<vector>
#include<ostream>

using namespace boost;
using namespace std;

class ThreadManager {
public:
    ThreadManager(ostream& _os);
    void newThread(int nSecs, int nTimes);
    void joinThreads();
    void detachThreads();
    string to_string();
    int getNThreads() const;
    virtual ~ThreadManager();
private:
    vector<thread*> threads;
    ostream& os;

};

void runner(ostream &stream, int tId, int nSecs, int nTimes);

#endif	/* THREADMANAGER_H */

