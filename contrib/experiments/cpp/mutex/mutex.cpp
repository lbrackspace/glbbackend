#include<cstdlib>
#include<iostream>
#include<sys/time.h>
#include<boost/thread.hpp>
#include<boost/random.hpp>
#include<boost/generator_iterator.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include"mutex.h"

using namespace boost;
using namespace std;

long *counters;

int main(int argc, char **argv) {
    if (argc < 5) {
        usage(argv[0]);
        return 0;
    }

    string cmd(argv[1]);
    vector<mutex*> simpleMutexes;
    vector<shared_mutex*> sharedMutexes;
    int nThreads = atoi(argv[2]);
    int nMutexes = atoi(argv[3]);
    int nIterations = atoi(argv[4]);
    counters = new long[nMutexes];
    random::mt19937 rng;
    random::uniform_int_distribution<> dist(0, nMutexes - 1);

    vector<thread*> threads;
    cout << "nCpus = " << thread::hardware_concurrency() << endl;
    for (int i = 0; i < nMutexes; i++) {
        simpleMutexes.push_back(new mutex);
        sharedMutexes.push_back(new shared_mutex);
    }
    if (cmd.compare("mutex") == 0) {
        cout << "spawning " << nThreads << " threads for " << nIterations << " for a simple mutex" << endl;
        double startTime = gettimevalue();
        for (int i = 0; i < nThreads; i++) {
            thread *th = new thread(mutexThread, nThreads, nIterations, rng, dist, &simpleMutexes);
            threads.push_back(th);
        }
        cout << "Threads all started joing" << endl;
        for (int i = 0; i < nThreads; i++) {
            threads[i]->join();
        }

        double stopTime = gettimevalue();
        double deltaTime = stopTime - startTime;
        cout << "Finished joining threads in " << deltaTime << " seconds" << endl;
        long sum = 0;
        for (int i = 0; i < nMutexes; i++) {
            sum += counters[i];
        }
        delete[] counters;
        cout << "counters = " << sum << endl;
        for (int i = 0; i < nMutexes; i++) {
            delete simpleMutexes[i];
            delete sharedMutexes[i];
        }
        threads.clear();
    } else if (cmd.compare("shared_mutex") == 0) {
        cout << "spawning " << nThreads << " threads for " << nIterations << " for a shared_mutex" << endl;
        double startTime = gettimevalue();
        for (int i = 0; i < nThreads; i++) {
            thread *th = new thread(sharedMutexThread, nThreads, nIterations, rng, dist, &sharedMutexes);
            threads.push_back(th);
        }
        cout << "Threads all started joined" << endl;
        for (int i = 0; i < nThreads; i++) {
            threads[i]->join();
        }
        double stopTime = gettimevalue();
        double deltaTime = stopTime - startTime;
        cout << "Finished joining threads in " << deltaTime << " seconds" << endl;
        long sum = 0;
        for (int i = 0; i < nMutexes; i++) {
            sum += counters[i];
        }
        delete[] counters;
        cout << "counters = " << sum << endl;

        for (int i = 0; i < nMutexes; i++) {
            delete simpleMutexes[i];
            delete sharedMutexes[i];
        }
        threads.clear();
    } else {
        cout << "Unknown comand " << cmd << endl;
        usage(argv[0]);
        return 0;
    }

    return 0;
}

int usage(char *prog) {
    cout << "Usage is " << prog << " <mutex|shared_mutex> <nThreads> <nMutexes> <nIterations>" << endl;
    return 0;
}

double gettimevalue() {
    double out;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return NAN;
    }
    out = (double) tv.tv_sec + (double) tv.tv_usec * 0.000001;
    return out;
}

void mutexThread(int nThreads, int nIterations, random::mt19937 rng, random::uniform_int_distribution<> dist, vector<mutex*> *mutexes) {
    int mi;
    for (int i = 0; i < nIterations; i++) {
        mi = dist(rng);
        mutex *m = (*mutexes)[mi];
        m->lock();
        counters[mi]++;
        m->unlock();
    }
}

void sharedMutexThread(int nThreads, int nIterations, random::mt19937 rng, random::uniform_int_distribution<> dist, vector<shared_mutex*> *mutexes) {
    int mi;
    for (int i = 0; i < nIterations; i++) {
        mi = dist(rng);
        shared_mutex *m = (*mutexes)[mi];
        m->lock_shared();
        counters[mi]++;
        m->unlock_shared();
    }
}