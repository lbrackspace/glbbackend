
#include<DemoUtils.h>
#include<string.h>
#include<string>
#include<sstream>
#include<sys/time.h>
#include<stdio.h>
#include<cmath>

const int STRBUFFSIZE = 1024;

using namespace std;

DemoUtils::DemoUtils() {
}

DemoUtils::DemoUtils(const DemoUtils& orig) {
}

DemoUtils::~DemoUtils() {
}

string DemoUtils::args2String(int argc, char** argv) {
    ostringstream os;
    for (int i = 0; i < argc; i++) {
        os << "argv[" << i << "]=" << argv[i] << endl;
    }
    return os.str();
}

string DemoUtils::showstringchars(const string& strIn) {
    ostringstream os;
    const string::const_iterator beg = strIn.begin();
    const string::const_iterator end = strIn.end();
    for (string::const_iterator si = beg; si != end; si++) {
        int i = si - beg;
        os << "str[" << i << "]=\"" << *si << "\"" << endl;
    }
    return os.str();
}

string DemoUtils::showstringvector(const vector<string>& strVector) {
    ostringstream os;
    const vector<string>::const_iterator beg = strVector.begin();
    const vector<string>::const_iterator end = strVector.end();
    os << "{ ";
    for (vector<string>::const_iterator si = beg; si != end; si++) {
        os << "\"" << *si << "\"";
        if (end - 1 > si) {
            os << ", ";
        }
    }
    os << "}";
    return os.str();
}

int DemoUtils::string2vector(const string& strIn, vector<string>& strVector, char delim) {
    int nStrings = 0;
    char buff[STRBUFFSIZE + 1];
    buff[0] = '\0';
    int ci = 0;
    int cb = 0;
    int cl;
    int li = strIn.size();
    for (ci = 0; ci <= li; ci++) {
        cl = ci - cb;
        if (strIn[ci] == delim || strIn[ci] == '\0' || cl >= STRBUFFSIZE) {
            if (cl <= 0) {
                cb = ci + 1;
                buff[0] = '\0';
                continue;
            }
            buff[ci - cb] = '\0';
            cb = ci + 1;
            strVector.push_back(string(buff));
            nStrings++;
            buff[0] = '\0';
            continue;
        }
        buff[ci - cb] = strIn[ci];
    }
    return nStrings;
}

string DemoUtils::joinStrings(const vector<string> strVector, const string delim, int start_idx) {
    ostringstream os;
    int i;
    int last_i = strVector.size() - 1;
    for (i = start_idx; i < last_i; i++) {
        os << strVector[i] << delim;
    }
    os << strVector[i];
    return os.str();
}

string DemoUtils::vector2string(const vector<string> &strVector) {
    ostringstream os;
    const vector<string>::const_iterator beg = strVector.begin();
    const vector<string>::const_iterator end = strVector.end();

    os << "Iterating through " << strVector.size() << " strings" << endl;
    for (vector<string>::const_iterator si = beg; si != end; si++) {
        int entry_i = si - beg;
        os << "strVector[" << entry_i << "]=\"" << *si << "\"" << endl;
    }
    return os.str();
}

void DemoUtils::normalize(vector<double> &v) {
    double squaredSum = 0.0;
    int i;
    if (v.size() == 0) {
        return; // Can't normalize 0 entry vector so do nothing
    }
    for (i = 0; i < v.size(); i++) {
        squaredSum += v[i] * v[i];
    }

    if (squaredSum == 0.0) {
        return; // Can't normalize a 0 length vector either so do nothing
    }
    double divisor = 1.0 / sqrt(squaredSum);
    for (i = 0; i < v.size(); i++) {
        v[i] *= divisor;
    }
}

double DemoUtils::gettimevalue() {
    double out;
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return NAN;
    }
    out = (double) tv.tv_sec + (double) tv.tv_usec * 0.000001;
    return out;
}

double DemoUtils::avg(const vector<double> &vals) {
    int n = 0;
    double sum = 0.0;
    if (vals.size() == 0) {
        return NAN;
    }
    vector<double>::const_iterator di;
    vector<double>::const_iterator beg = vals.begin();
    vector<double>::const_iterator end = vals.end();
    for (di = beg; di != end; di++) {
        n++;
        sum += *di;
    }
    return sum / double(n);
}