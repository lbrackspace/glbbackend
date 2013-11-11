#ifndef __MUTEX_H
#define __MUTEX_H

#include<boost/thread.hpp>
#include<boost/ptr_container/ptr_vector.hpp>
#include<boost/random.hpp>

int usage(char *prog);
double gettimevalue();
void mutexThread(int nThreads, int nIterations, boost::random::mt19937 rnd, boost::random::uniform_int_distribution<> dist, std::vector<boost::mutex*>* mutexes);
void sharedMutexThread(int nThreads, int nIterations, boost::random::mt19937, boost::random::uniform_int_distribution<>, std::vector<boost::shared_mutex*>* mutexes);
#endif
