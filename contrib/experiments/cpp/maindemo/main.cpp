#include<iostream>
#include<boost/smart_ptr.hpp>
#include<boost/asio.hpp>
#include<boost/thread.hpp>
#include<boost/thread/locks.hpp>
#include<exception>
#include<stdexcept>
#include<sstream>
#include<ostream>
#include<string>
#include<string.h>
#include<cstdlib>
#include<ctime>
#include<time.h>
#include<unistd.h>
#include<deque>
#include<boost/unordered_map.hpp>
#include<boost/interprocess/mapped_region.hpp>
#include<boost/algorithm/string.hpp>
#include<boost/random/mersenne_twister.hpp>
#include<boost/random/uniform_int.hpp>
#include<boost/random/variate_generator.hpp>
#include<rapidjson/document.h>
#include<rapidjson/writer.h>
#include"DemoUtils.h"
#include"ThreadManager.h"
#include"Matrix.h"
#include"Point.h"
#include"ring_buffer.h"
#include"BitArray.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filestream.h"
#include "rapidjson/writer.h"


using namespace std;
using namespace boost;
using namespace boost::asio;

boost::mt19937 rng(static_cast<boost::uint32_t> (std::time(0)));
boost::uniform_int<int> ud;
boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rn(rng, ud);

const int LINE_SIZE = 1024;
string buildJson(string val, int n);
Point getPoint(vector<string> args, int offset);
string showsizeof();
string help();
int nop();
int demoCopyVsMove();
std::string address_to_string(boost::asio::ip::address& addr);
boost::shared_ptr<boost::asio::ip::tcp::resolver::iterator> resolveTCP(std::string& host, std::string& service, boost::asio::io_service& ios, boost::system::error_code& ec);
int gcdreduce(vector<int> &reduced_weights, vector<int> &weights, int& itercount);

double gettimevalue() {
    double out;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return NAN;
    }
    out = (double) tv.tv_sec + (double) tv.tv_usec * 0.000001;
    return out;
}

string buildJson2(string val, int n) {
    using namespace rapidjson;
    Document document;
    Document::AllocatorType& allocator = document.GetAllocator();

    document.SetArray();

    document.PushBack("Test1", allocator);
    document.PushBack("Test2", allocator);

    Value item;
    item.SetObject();
    item.AddMember("Id", 1, allocator);
    item.AddMember("Name", "ARInside", allocator);
    document.PushBack(item, allocator);

    // check array size


    StringBuffer f;
    Writer<StringBuffer> writer(f);
    document.Accept(writer);
    return std::string(f.GetString());
}

string buildJson(string val, int n) {
    using namespace rapidjson;
    using namespace std;
    Document doc;
    Document::AllocatorType& aloc = doc.GetAllocator();
    doc.SetObject();
    doc.AddMember("someString", val.c_str(), aloc);
    doc.AddMember("someInt", 500, aloc);
    doc.AddMember("bool",false,aloc);
    Value arry;
    arry.SetArray();
    for (int i = 0; i < n; i++) {
        Value l;
        l.SetInt64(5000000000 + i);
        arry.PushBack(l, aloc);
    }
    arry.PushBack("wtf", aloc);
    doc.AddMember("someArry", arry, aloc);
    StringBuffer sb;
    Writer<StringBuffer> wr(sb);
    doc.Accept(wr);
    return string(sb.GetString());
}

class MyTimer {
private:
    double baseTime;
public:

    MyTimer() {
        baseTime = gettimevalue();
    }

    void reset() {
        baseTime = gettimevalue();
    }

    double read() {
        return gettimevalue() - baseTime;
    }
};

int main(int argc, char **argv) {
    char *cmd = new char[LINE_SIZE + 1];
    int nArgs = 0;
    int i;
    long nwb = 0;
    Point p1;
    Point p2;
    Point sum;
    MyTimer tmr;
    io_service ios;
    ring_buffer rb(0);
    deque<string> dq;
    vector<string> *strVector = new vector<string>;
    unordered_map<string, string> *strMap = new unordered_map<string, string>;
    shared_ptr<string> strPtr(new string("default"));
    vector<Matrix>matVector;
    vector < shared_ptr < string > > strPtrs;
    string prompt = "Silly Shell> ";
    ThreadManager tm(cout);
    vector<string>cmdArgs;
    BitArray ba(32);
    cout << "pid = " << getpid() << endl;
    cout << "main thread_id = " << boost::this_thread::get_id() << endl;
    do {
        try {
            cout << prompt;
            cin.getline(cmd, LINE_SIZE);
            cmdArgs.clear();
            nArgs = DemoUtils::string2vector(cmd, cmdArgs, ' ');
            if ((nArgs >= 2) && (cmdArgs[0].compare("wb") == 0)) {
                int nBytes = atoi(cmdArgs[1].c_str());
                if (nBytes <= 0) {
                    cout << "No bytes to waste" << endl;
                }
                cout << "wasting " << nBytes << " bytes" << endl;

                char *tmpBytes = new char[nBytes + 1];
                for (i = 0; i < nBytes; i++) {
                    tmpBytes[i] = ' ';
                }
                tmpBytes[i] = '\0';
                strVector->push_back(tmpBytes);
                nwb += nBytes;
                delete tmpBytes;
            } else if ((nArgs >= 1) && (cmdArgs[0].compare("pa") == 0)) {
                string argStr = DemoUtils::showvector<string > (cmdArgs);
                cout << "args = " << argStr << endl;
            } else if ((nArgs >= 1) && (cmdArgs[0].compare("fb") == 0)) {
                cout << "Freeing " << nwb << " bytes" << endl;
                delete strVector;
                strVector = new vector<string>;
                nwb = 0;
            } else if ((nArgs >= 2) && (cmdArgs[0].compare("nv") == 0)) {
                vector<double> dvector;
                cout << "Converting strings to double" << endl;
                for (i = 1; i < nArgs; i++) {
                    double val = std::atof(cmdArgs[i].c_str());
                    dvector.push_back(val);
                }
                cout << "vector<double> vals = " << DemoUtils::showvector<double>(dvector) << endl;
                cout << "Normalizing" << endl;
                DemoUtils::normalize(dvector);
                cout << DemoUtils::showvector<double>(dvector) << endl;
            } else if ((nArgs >= 1) && (cmdArgs[0].compare("nwb") == 0)) {
                long wasted_size = 0;
                vector<string>::iterator si;
                vector<string>::iterator beg = strVector->begin();
                vector<string>::iterator end = strVector->end();
                for (si = beg; si != end; si++) {
                    wasted_size += strlen(si->c_str());
                }
                cout << "bytes wasted so far is " << nwb << "= " << wasted_size << endl;

            } else if ((nArgs >= 1) && (cmdArgs[0].compare("help") == 0)) {
                cout << help() << endl;
            } else if (nArgs >= 1 && (cmdArgs[0].compare("clearmap") == 0)) {
                delete strMap;
                strMap = new unordered_map<string, string>;
            } else if (nArgs >= 3 && (cmdArgs[0].compare("put") == 0)) {
                string key = cmdArgs[1];
                string val = cmdArgs[2];
                cout << "setting map[" << key << "]=" << val << endl;
                //strMap->insert(make_pair(key, val));
                (*strMap)[key] = val;
            } else if ((nArgs >= 2) && (cmdArgs[0].compare("avg") == 0)) {
                vector<double> dvector;
                cout << "Converting strings to double" << endl;
                for (i = 1; i < nArgs; i++) {
                    double val = std::atof(cmdArgs[i].c_str());
                    dvector.push_back(val);
                }
                cout << "vector<double> vals = " << DemoUtils::showvector<double>(dvector) << endl;
                double avg = DemoUtils::avg(dvector);
                cout << "abg = " << avg << endl;

            } else if (nArgs >= 2 && (cmdArgs[0].compare("get") == 0)) {
                string key = cmdArgs[1];
                cout << "getting map[" << key << "]=";
                string val;
                try {
                    val = strMap->at(key);
                } catch (out_of_range& ex) {
                    val = "null";
                }
                cout << val << endl;
            } else if (nArgs >= 1 && (cmdArgs[0].compare("keys") == 0)) {
                cout << "{";
                unordered_map<string, string>::iterator ki;
                unordered_map<string, string>::iterator beg = strMap->begin();
                unordered_map<string, string>::iterator end = strMap->end();

                for (ki = beg; ki != end; ki++) {
                    string key = ki->first;
                    cout << "\"" << key << "\", ";
                }
                cout << "}" << endl;
            } else if (nArgs >= 1 && (cmdArgs[0].compare("allmap") == 0)) {
                unordered_map<string, string>::iterator mi;
                unordered_map<string, string>::iterator beg = strMap->begin();
                unordered_map<string, string>::iterator end = strMap->end();
                cout << "{";
                for (mi = beg; mi != end; mi++) {
                    string key = mi->first;
                    string val = mi->second;
                    cout << "(\"" << key << "\",\"" << val << "\"), ";
                }
                cout << "}" << endl;

            } else if ((nArgs >= 2) && (cmdArgs[0].compare("dkey") == 0)) {
                string key = cmdArgs[1];
                cout << "deleting key " << key << endl;
                strMap->erase(key);
            } else if ((nArgs >= 1) && (cmdArgs[0].compare("pid")) == 0) {
                cout << "pid = " << getpid() << endl;
            } else if ((nArgs >= 2) && (cmdArgs[0].compare("ln") == 0)) {
                int nTimes = atoi(cmdArgs[1].c_str());
                cout << "Looping " << nTimes << endl;
                double beforeTime = DemoUtils::gettimevalue();
                vector<string> tmpV;
                for (i = 0; i < nTimes; i++) {
                    tmpV.clear();
                    string testString("This is a xxxx  x  x xxx xxxx xxxx xxx xxx xxx    test");
                    int nVecs = DemoUtils::string2vector(testString, tmpV, ' ');
                }
                double afterTime = DemoUtils::gettimevalue();
                double timeDelta = afterTime - beforeTime;
                cout << "looped " << nTimes << " in " << timeDelta << " seconds" << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("st") == 0) {
                cout << "Showing running threads" << endl;
                cout << tm.to_string() << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("jt") == 0) {
                cout << "joining threads" << endl;
                tm.joinThreads();
                cout << "All threads joined" << endl;
            } else if ((nArgs >= 4) && cmdArgs[0].compare("nt") == 0) {
                int nThreads = std::atoi(cmdArgs[1].c_str());
                int nSecs = std::atoi(cmdArgs[2].c_str());
                int nTimes = std::atoi(cmdArgs[3].c_str());
                cout << "creating " << nThreads << " that will sleep for " << nSecs << " seconds " << nTimes << " times" << endl;
                for (i = 0; i < nThreads; i++) {
                    cout << "creating thread " << i << " of " << nThreads << endl;
                    tm.newThread(nSecs, nTimes);
                }
                cout << "All threads spawned" << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("dt") == 0) {
                cout << "Detatching threads" << endl;
                tm.detachThreads();
            } else if ((nArgs >= 1) && cmdArgs[0].compare("tc") == 0) {
                int nThreads = tm.getNThreads();
                cout << "Threads count is " << nThreads << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("sizeof") == 0) {
                cout << showsizeof() << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("mf") == 0) {
                cout << "Clearing " << matVector.size() << " entries from matrix vector" << endl;
                matVector.clear();
            } else if ((nArgs >= 7) && cmdArgs[0].compare("ap") == 0) {
                p1 = getPoint(cmdArgs, 1);
                p2 = getPoint(cmdArgs, 4);
                sum = p1 + p2;
                cout << "Sum of " << p1 << " + " << p2 << " = " << sum << endl;
            } else if ((nArgs >= 7) && cmdArgs[0].compare("dp") == 0) {
                p1 = getPoint(cmdArgs, 1);
                p2 = getPoint(cmdArgs, 4);
                double dp = dot(p1, p2);
                cout << "dot(" << p1 << "," << p2 << ") = " << dp << endl;
            } else if ((nArgs >= 2) && cmdArgs[0].compare("ma") == 0) {
                int nMatrixes = std::atoi(cmdArgs[1].c_str());
                cout << "Adding " << nMatrixes << " 100x100 matrixes" << endl;
                for (i = 0; i < nMatrixes; i++) {
                    matVector.push_back(Matrix(100, 100));
                }
            } else if ((nArgs >= 2) && cmdArgs[0].compare("ss") == 0) {
                strPtr = shared_ptr<string > (new string(cmdArgs[1]));
                cout << "*strPtr = " << *strPtr << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("as") == 0) {
                int nPtrs = 1;
                if (nArgs >= 2) nPtrs = std::atoi(cmdArgs[1].c_str());
                cout << "Attaching " << *strPtr << " " << nPtrs << " times" << endl;
                for (i = 0; i < nPtrs; i++) {
                    strPtrs.push_back(strPtr);
                }
            } else if ((nArgs >= 1) && cmdArgs[0].compare("sc") == 0) {
                cout << "*strPtr = " << *strPtr << endl;
                cout << "refCount(strPtr) = " << strPtr.use_count() << endl;
                for (i = 0; i < strPtrs.size(); i++) {
                    cout << "strPtrs[" << i << "] = " << *strPtrs[i] << " ref_cout=" << strPtrs[i].use_count() << endl;
                }

            } else if ((nArgs >= 1) && cmdArgs[0].compare("rs") == 0) {
                int nPtrs = 1;
                if (nArgs >= 2) nPtrs = std::atoi(cmdArgs[1].c_str());
                cout << "removing " << nPtrs << " strings from sharedPtrs" << endl;
                if (nPtrs > strPtrs.size()) {
                    nPtrs = strPtrs.size();
                }
                cout << "There are only " << strPtrs.size() << " strings to delete deleting those only" << endl;
                for (i = 0; i < nPtrs; i++) strPtrs.pop_back();
                cout << "strPtrs size = " << strPtrs.size() << endl;
            } else if ((nArgs >= 1) && cmdArgs[0].compare("cs") == 0) {
                cout << "Clearing " << strPtrs.size() << " entries from the strPtrs list" << endl;
                strPtrs.clear();

            } else if ((nArgs >= 1) && cmdArgs[0].compare("exit") == 0) {
                break;
            } else if ((nArgs >= 3) && cmdArgs[0].compare("nsl") == 0) {
                string & host(cmdArgs[1]);
                string & service(cmdArgs[2]);
                boost::system::error_code ec;
                shared_ptr<ip::tcp::resolver::iterator> res_it(resolveTCP(host, service, ios, ec));
                if (ec) {
                    cout << "Error looking up " << host << " " << service << ": " << ec.message() << endl;
                    continue;
                }
                ip::tcp::resolver::iterator end;
                int i = 0;
                while (*res_it != end) {
                    ip::tcp::endpoint ep((*res_it)->endpoint());
                    boost::asio::ip::address addr = ep.address();
                    cout << "endpoint[" << i << "]: " << addr.to_string() << " " << ep << endl;
                    (*res_it)++;
                    i++;
                }
            } else if (nArgs >= 2 && cmdArgs[0].compare("nrb") == 0) {
                int nSize = std::atoi(cmdArgs[1].c_str());
                cout << "Creating new ring buff of " << nSize << " chars" << endl;
                rb = ring_buffer(nSize);
                nop();
            } else if (nArgs >= 2 && cmdArgs[0].compare("nrbw") == 0) {
                string nrbString = DemoUtils::joinStrings(cmdArgs, " ", 1);
                cout << "Writing string " << nrbString << " to ring buffer" << endl;
                int nBytes = rb.write(nrbString + "\n");
                cout << "wrote " << nBytes << endl;
            } else if (nArgs >= 2 && cmdArgs[0].compare("nrbr") == 0) {
                int nBytes = std::atoi(cmdArgs[1].c_str());
                string readString(rb.read(nBytes));
                cout << "Read " << readString.length() << " from ring buffer: \"" << readString << "\"" << endl;
            } else if (nArgs >= 2 && cmdArgs[0].compare("nrbdec") == 0) {
                int nBytes = std::atoi(cmdArgs[1].c_str());
                cout << "Decrementing ring buffer by " << nBytes << endl;
                int nDeleted = rb.dec(nBytes);
                cout << "deleted " << nDeleted << " chars from ring_buffer" << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("nrbs") == 0) {
                bool showBuffer = false;
                if (nArgs > 1) {
                    showBuffer = (cmdArgs[1].compare("true") == 0) ? true : false;
                }
                cout << "calling rb.debug_str(" << boolalpha << showBuffer << "):" << endl;
                cout << rb.debug_str(showBuffer) << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("nrblc") == 0) {
                cout << "number of lines in ring_buffer is ";
                cout << rb.linesAvailable() << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("nrblp") == 0) {
                string line = rb.readLine();
                cout << "poped line \"" << line << "\" off the ring_buffer" << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("nrbdc") == 0) {
                cout << "doubling capacity of ring_char buff" << endl;
                cout << "before: " << rb.debug_str(false) << endl;
                rb.double_capacity();
                cout << "after: " << rb.debug_str(false) << endl;
            } else if (nArgs >= 2 && cmdArgs[0].compare("nrbws") == 0) {
                int nBytes = std::atoi(cmdArgs[1].c_str());
                cout << "Writing " << nBytes << " bytes into ring buffer" << endl;
                for (i = 0; i < nBytes; i++) {
                    rb.write("X");
                }
            } else if (nArgs >= 2 && cmdArgs[0].compare("reduce") == 0) {
                vector<int> weights;
                vector<int> reduced;
                cout << "reducing: ";
                int itercount = 0;
                int as = cmdArgs.size();
                for (i = 1; i < as; i++) {
                    int weight = std::atoi(cmdArgs[i].c_str());
                    cout << weight << " ";
                    weights.push_back(weight);
                }
                cout << endl;
                int r = gcdreduce(reduced, weights, itercount);
                cout << "weights.size() = " << weights.size() << endl;
                cout << "reduced.size() = " << reduced.size() << endl;

                cout << "reduced in " << itercount << " gcd loop iterations. gcd= " << r << endl;
                cout << "Reduction: ";
                as = reduced.size();
                for (i = 0; i < as; i++) {
                    cout << reduced[i] << " ";
                }
                cout << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("dqs") == 0) {
                cout << "dqsize = " << dq.size() << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("dqc") == 0) {
                cout << "Clearing dq" << endl;
                dq.clear();
            } else if (nArgs >= 2 && cmdArgs[0].compare("dqw") == 0) {
                string val(cmdArgs[1]);
                cout << "adding " << val << " onto dq" << endl;
                dq.push_back(val);
            } else if (nArgs >= 1 && cmdArgs[0].compare("dqr") == 0) {
                string val(*dq.begin());
                cout << "read value " << val << endl;
                cout << "popping value off front" << endl;
                dq.pop_front();
            } else if (nArgs >= 2 && cmdArgs[0].compare("ba") == 0) {
                int nBits = std::atoi(cmdArgs[1].c_str());
                cout << "Allocating bit array to size " << nBits << endl;
                ba = BitArray(nBits);
            } else if (nArgs >= 1 && cmdArgs[0].compare("bah") == 0) {
                cout << "bitArray in hex: " << ba.to_hex() << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("bab") == 0) {
                cout << "bitArray in bin: " << ba.to_bin() << endl;
            } else if (nArgs >= 3 && cmdArgs[0].compare("bas") == 0) {
                int bit_idx = std::atoi(cmdArgs[1].c_str());
                int bit_val = std::atoi(cmdArgs[2].c_str());
                cout << "setting bit[" << bit_idx << "] = " << bit_val << endl;
                ba.setBit(bit_idx, bit_val);
            } else if (nArgs >= 2 && cmdArgs[0].compare("bag") == 0) {
                int bit_idx = std::atoi(cmdArgs[1].c_str());
                cout << "getting bit[" << bit_idx << "] = " << ba.getBit(bit_idx) << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("splitstr") == 0) {
                string striplr(cmd);
                boost::algorithm::replace_all(striplr, "\r", "");
                boost::algorithm::replace_all(striplr, "\n", "");
                vector<string>splitStr;
                boost::algorithm::split(splitStr, striplr, boost::algorithm::is_any_of(" "), boost::token_compress_on);
                int splitSize = splitStr.size();
                for (i = 0; i < splitSize; i++) {
                    cout << "splitStr[" << i << "]=\"" << splitStr[i] << "\"" << endl;
                }
            } else if (nArgs >= 2 && cmdArgs[0].compare("rndsh") == 0) {
                vector<int> nums;
                int nVals = std::atoi(cmdArgs[1].c_str());
                for (i = 0; i < nVals; i++) {
                    nums.push_back(i);
                }

                clock_t startTime = clock();
                random_shuffle(nums.begin(), nums.end(), rn);
                clock_t endTime = clock();
                double secs = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
                cout << "Took " << secs << " seconds to shuffle " << nVals << " elements: " << endl;
                cout << "Vals = ";
            } else if (nArgs >= 1 && cmdArgs[0].compare("resettime") == 0) {
                tmr.reset();
                cout << "Reset timer" << endl;
            } else if (nArgs >= 1 && cmdArgs[0].compare("readtime") == 0) {
                cout << "Timer reads " << tmr.read() << endl;
                clock_t startTime = clock();
                for (int i = 0; i < 2000000000; i++);
                clock_t endTime = clock();
                double secs = static_cast<double> (endTime - startTime) / CLOCKS_PER_SEC;
                cout << "Took " << secs << " seconds to shuffle " << endl;
            } else if (nArgs >= 3 && cmdArgs[0].compare("write_json") == 0) {
                string val = cmdArgs[1];
                int n = std::atoi(cmdArgs[2].c_str());
                int nl = 1;
                if (nArgs > 3) {
                    nl = std::atoi(cmdArgs[3].c_str());
                }
                cout << "Writing json with params " << val << " " << n << " " << nl << " times" << endl;
                string json("");
                for (i = 0; i < nl; i++) {
                    json = buildJson(val, n);
                }
                cout << json << endl;
            } else {
                cout << "Unknown command" << cmd << endl;
                cout << help() << endl;
            }
        } catch (std::exception& ex) {
            cout << "exception:" << ex.what() << endl;
        }
    } while (!cin.eof());
    delete strVector;
    delete strMap;
    delete cmd;
    return 0;
}

Point getPoint(vector<string> args, int offset) {
    double x = std::atof(args[offset + 0].c_str());
    double y = std::atof(args[offset + 1].c_str());
    double z = std::atof(args[offset + 2].c_str());
    Point p = Point(x, y, z);
    return p;
}

string help() {
    ostringstream os;
    os << "help #Displays this help menu" << endl
            << "pid #show pid" << endl
            << "pa <args..> #Echo args" << endl
            << "wb <nBytes> #add nBytes to the vector<string> wb object" << endl
            << "fb #Free all bytes in vector<string> wb object" << endl
            << "nwb #Display number of bytes wasted so far" << endl
            << "clearmap #clear map<string,string> strMap object" << endl
            << "put <key> <val> #put the key value in the map<string,string> strMap object" << endl
            << "get <key> #Get value in map map<string,string> strMap object" << endl
            << "keys #List keys in map<string,string> strMap object" << endl
            << "dkey #Delete the key from the map<string,string> strMap object" << endl
            << "ln <nTimes> #Execute loop nTimes" << endl
            << "allmap #Display all key values in map<string,string> strMap object" << endl
            << "nv <double1...double> #Normalize vector" << endl
            << "avg <double1...double> #compute Average vector" << endl
            << "nt <nThreads> <nSecs> <nTimes> #Spawn nThreads with each one sleeping for nSecs nTimes" << endl
            << "tc #Show thread count" << endl
            << "st #Show running threads" << endl
            << "jt #Join running threads" << endl
            << "dt #Detach threads" << endl
            << "sizeof #Get size of varias data types" << endl
            << "ma <nEntries> # Add n 100x100 matrixes into memory" << endl
            << "mf #Free matrixes from memory" << endl
            << "ap <x1> <y1> <z1> <x2> <y2> <z2> #Add the two points together" << endl
            << "dp <x1> <y1> <z1> <x2> <y2> <z2> #Dot the two points together" << endl
            << "ss <strVal> #Set the string pointer to the value of the given string" << endl
            << "as [nStrs] # push nStrs onto the strPtrs list or just one if nStrs isn't specified" << endl
            << "sc #Display reference count on stringPtr" << endl
            << "cs #clear the smart_ptr<string> vector list" << endl
            << "nsl <host> <port> #Do a boost asio resolver call on the specified host and port" << endl
            << "nrb <size> #Create new ring buffer" << endl
            << "nrbw <str> #Add string to ring bufer" << endl
            << "nrbr <size> #read nBytes from ringBuffer" << endl
            << "nrbwb <size> #write bogus bytes into buffer"
            << "nrbdec <size> # remve nBytes from the ringBuffer" << endl
            << "nrbs [true|false]#Show entire ring buffer the true or false option sepcifies wether the characters should be displayed" << endl
            << "nrblc #Count lines in ring_buffer" << endl
            << "nrblp #pop a line from the ring_buffer" << endl
            << "nrbdc #Double the capacity of the ring buffer" << endl
            << "nrbws <nBytes> # Write nBytes into the ring buffer" << endl
            << "nrbdec <nBytes> #Decrement the number of bytes in the buffer simulating a read" << endl
            << "reduce <int1> <int2> <int..n> #use gcd to reduce the following vector simulating the weighted algo" << endl
            << "expandreduce <int1> <int2> <int..n? #reduce the weights then expand to vector list" << endl
            << "dqs #show dq size" << endl
            << "dqc #clear dq" << endl
            << "dqw <str> #Write str onto deque" << endl
            << "dqr <str> #read string off front of deque" << endl
            << "ba <nBits> #create BitArray of size nBits " << endl
            << "bah #display the bit array in hex" << endl
            << "bab #display the bit array in binar" << endl
            << "bas <bitNum> <bitVal> #set the bitNumber to the bitval" << endl
            << "bag <bitNum> #Read the bit at the specified address" << endl
            << "splitstr <str> #split str into string vector" << endl
            << "rndsh <nArgs> #Randomly shuffle the values" << endl
            << "readtime #Read the timer" << endl
            << "resettime #Reset the timer" << endl
            << "write_json <val> <n> [ln]#write json value" << endl
            << "exit #Exit program" << endl;

    return os.str();
}

string showsizeof() {
    ostringstream os;
    os << "Sizeof:" << endl
            << "sizeof(bool):                " << setw(4) << sizeof (bool) << endl
            << "sizeof(char):                " << setw(4) << sizeof (char) << endl
            << "sizeof(int):                 " << setw(4) << sizeof (int) << endl
            << "sizeof(long):                " << setw(4) << sizeof (long) << endl
            << "sizeof(long long):           " << setw(4) << sizeof (long) << endl
            << "sizeof(float):               " << setw(4) << sizeof (float) << endl
            << "sizeof(double):              " << setw(4) << sizeof (double) << endl
            << "sizeof(long double):         " << setw(4) << sizeof (long double) << endl
            << "sizeof(void *):              " << setw(4) << sizeof (void *) << endl
            << "sizeof(string):              " << setw(4) << sizeof (string) << endl
            << "sizeof(boost::thread):       " << setw(4) << sizeof (boost::thread) << endl
            << "sizeof(boost::mutex):        " << setw(4) << sizeof (boost::mutex) << endl
            << "sizeof(boost::shared_mutex): " << setw(4) << sizeof (boost::shared_mutex) << endl
            << "sizeof(ThreadManager):       " << setw(4) << sizeof (ThreadManager) << endl
            << "sizeof(Matrix):              " << setw(4) << sizeof (Matrix) << endl
            << "sizeof(bool):                " << setw(4) << sizeof (bool) << endl
            << "sizeof(vector<double>):      " << setw(4) << sizeof (vector<double>) << endl
            << "sizeof(vector<double *>):    " << setw(4) << sizeof (vector<double *>) << endl
            << "sizeof(shared_ptr<string>):  " << setw(4) << sizeof (shared_ptr<string>) << endl
            << "sizeof(shared_ptr<double>):  " << setw(4) << sizeof (shared_ptr<double>) << endl
            << "sizeof(unordered_map):       " << setw(4) << sizeof (unordered_map<string, string>) << endl
            << "sizeof(ring_buffer):         " << setw(4) << sizeof (ring_buffer) << endl
            << "sizeof(boost::ip::tcp::resolver " << setw(4) << sizeof (boost::asio::ip::tcp::resolver) << endl
            << "sizeof(boost::ip::tcp::resolver::iterator) " << setw(4) << sizeof (boost::asio::ip::tcp::resolver::iterator) << endl
            << "sizeof(boost::condition_variable) " << setw(4) << sizeof (boost::condition_variable) << endl
            << "sizeof(MyTimer)                   " << setw(4) << sizeof (MyTimer) << endl
            << endl
            << "boost::thread::hardware_concurrency(): " << setw(4) << boost::thread::hardware_concurrency() << endl
            << "mapped_region::get_page_size():        " << setw(4) << boost::interprocess::mapped_region::get_page_size() << endl
            << "time: " << std::time(0) << endl;


    return os.str();
}

string address_to_string(ip::address& addr) {
    ostringstream os;
    os << "{"
            << " is_loopback=" << boolalpha << addr.is_loopback() << ", "
            << "is_multicase: " << boolalpha << addr.is_multicast() << ", "
            << "is_unspecified: " << boolalpha << addr.is_unspecified() << ", "
            << "is_v4: " << boolalpha << addr.is_v4() << ", "
            << "is_v6: " << boolalpha << addr.is_v6() << ", "
            << "addr: " << addr.to_string() << "}";
    return os.str();
}

boost::shared_ptr<ip::tcp::resolver::iterator> resolveTCP(string& host, string& service, io_service& ios, boost::system::error_code& ec) {
    ip::tcp::resolver r(ios);
    ip::tcp::resolver::query q(host, service, boost::asio::ip::resolver_query_base::all_matching);
    boost::shared_ptr<ip::tcp::resolver::iterator> it(new ip::tcp::resolver::iterator(r.resolve(q, ec)));
    if (ec) {
        boost::shared_ptr<boost::asio::ip::tcp::resolver::iterator> null;
        return null;
    }
    //ip::tcp::resolver::iterator it = r.resolve(q, ec);
    cout << "it @" << it.get() << endl;
    return it;
}

void expandweights(vector<int> &expanded, vector<int>&weights) {
    int ws = weights.size();
    expanded.clear();
    for (int i = 0; i < ws; i++) {
        int weight = weights[i];
        for (int j = 0; j < weight; j++) {
            weights.push_back(i);
        }
    }
}

// Besure you don't have all vectors weighted at zero as this triggers division by zero

int gcdreduce(vector<int> &reduced_weights, vector<int> &weights, int& itercount) {
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

int nop() {
    return 0;
}
