#include<string>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<boost/shared_array.hpp>
#include<string>
#include<vector>
#include<sstream>

#include "ring_buffer.h"
#include"BitArray.h"

const int STRBUFFSIZE = 4096;

ring_buffer::ring_buffer(int ds) {
    std::cout << "ring_buffer(" << ds << ") construct @" << this << std::endl;
    data_size = ds;
    data = boost::shared_array<char>(new char[ds]);
    used = 0;
    h_idx = 0;
}

ring_buffer& ring_buffer::operator =(const ring_buffer& orig) {
    const ring_buffer *origPtr = &orig;
    std::cout << "copy assignment operator " << this << " ring_buffer(" << origPtr << ") called" << std::endl;
    int ds = orig.data_size;
    data_size = ds;
    char *origcharPtr = orig.data.get();
    data = boost::shared_array<char>(new char[ds]);
    char *charPtr = data.get();
    h_idx = orig.h_idx;
    used = orig.used;
    int u = used;
    int h = h_idx;
    int abs_i;
    for (int i = 0; i < u; i++) {
        abs_i = (h + i) % ds;
        charPtr[abs_i] = origcharPtr[abs_i];
    }
    return *this;
}

ring_buffer::ring_buffer(const ring_buffer& orig) {
    const ring_buffer *origPtr = &orig;
    std::cout << "copy constructer " << this << " ring_buffer(" << origPtr << ") called" << std::endl;
    int ds = orig.data_size;
    data_size = ds;
    char *origcharPtr = orig.data.get();
    data = boost::shared_array<char>(new char[ds]);
    char *charPtr = data.get();
    h_idx = orig.h_idx;
    used = orig.used;
    int h = h_idx;
    int abs_i;
    for (int i = 0; i < ds; i++) {
        abs_i = (h + i) % ds;
        charPtr[abs_i] = origcharPtr[abs_i];
    }
}

ring_buffer::~ring_buffer() {
    std::cout << "~ring_buffer() destruct @" << this << std::endl;
}

int ring_buffer::used_size() const {
    return used;
}

int ring_buffer::dec(int dec_size) {
    int ds = data_size;
    int n = (dec_size < used) ? dec_size : used;
    if (ds == 0) {
        return 0;
    }
    h_idx = (h_idx + n) % ds;
    used -= n;
    return n;
}

int ring_buffer::write(std::string str) {
    int nBytes = str.length();
    const char *strPtr = str.c_str();
    return write(strPtr, nBytes);
}

int ring_buffer::write(const char *strPtr, int write_size) {
    int f = data_size - used;
    int ds = data_size;
    int di = 0;
    int ri;
    if (ds == 0) {
        return 0;
    }
    ri = (h_idx + used) % ds;
    if (ds <= 0) {
        return 0;
    }
    while (di < write_size && f > 0) {
        data[ri] = strPtr[di];
        ri = (ri + 1) % ds;
        di++;
        f--;
    }
    used += di;
    return di;
}

std::string ring_buffer::readLine() {
    int nBytes = 0;
    int ri = h_idx;
    int u = used;
    int ds = data_size;
    int i;

    // Compute length of line
    for (i = 0; i < u; i++) {
        if (data[ri] == '\n') {
            nBytes++;
            break;
        }
        ri = (ri + 1) % ds;
        nBytes++;
    }

    std::string popped_line(nBytes + 1, '\0');
    ri = h_idx;
    for (i = 0; i < nBytes; i++) {
        popped_line[i] = data[ri];
        ri = (ri + 1) % ds;
    }
    dec(nBytes);
    return popped_line;
}

int ring_buffer::linesAvailable() const {
    int nLines = 0;
    int ds = data_size;
    int u = used;
    int ri = h_idx;
    int i;
    for (i = 0; i < u; i++) {
        if (data[ri] == '\n') {
            nLines++;
        }
        ri = (ri + 1) % ds;
    }
    return nLines;
}

std::string ring_buffer::read(int max_size) {
    int u = used;
    int ds = data_size;
    int nBytes = (u < max_size) ? u : max_size;
    std::string val(nBytes + 1, '\0');
    for (int i = 0; i < u; i++) {
        val[i] = data[i % ds];
    }
    return val;
}

int ring_buffer::read(char *strPtr, int max_size) {
    int ds = data_size;
    int u = used;
    int di = 0;
    int ri = h_idx;
    while (di < max_size && u > 0) {
        strPtr[di] = strPtr[ri];
        ri = (ri + 1) % ds;
        di++;
        u--;
    }

    dec(di);
}

int ring_buffer::free_size() const {
    return data_size - used;
}

int ring_buffer::getDataSize() const {
    return data_size;
}

std::string ring_buffer::debug_str(bool showBuffer) const {
    std::ostringstream os;
    int ds = data_size;
    int i;
    int h = h_idx;
    int u = used;

    u = used;
    h = h_idx;
    if (showBuffer) {
        BitArray ba(ds);
        int th = h_idx;
        for (int tu = used; tu > 0; tu--) {
            ba.setBit(th, 1);
            th = (th + 1) % ds;
        }
        for (i = 0; i < ds; i++) {
            const char ch = data[i];
            std::string strRepr = std::string(1, data[i]);
            if (ch == '\n') {
                strRepr = "\\n";
            }
            os << "rb[" << std::setw(4) << i << "]= ";
            if (ba.getBit(i) == 1) {
                os << strRepr << " *";
            }
            os << std::endl;
        }
    }
    os << "{"
            << "{used=" << used
            << " free=" << free_size()
            << " h_idx=" << h_idx
            << " data@" << (void *) (data.get())
            << " data usecount=" << data.use_count()
            << " this=" << this
            << "}"
            << std::endl;
    return os.str();
}

int ring_buffer::stringToVector(const std::string& strIn, std::vector<std::string>& strVector, char delim, bool skipLF) {
    int nStrings = 0;
    char buff[STRBUFFSIZE + 1];
    buff[0] = '\0';
    int ci = 0;
    int cb = 0;
    int cl;
    int li = strIn.size();
    for (ci = 0; ci <= li; ci++) {
        cl = ci - cb;
        char ch = strIn[ci];
        if (ch == delim || ch == '\0' || cl >= STRBUFFSIZE || (skipLF && (ch == '\n' || ch == '\r'))) {
            if (cl <= 0) {
                cb = ci + 1;
                buff[0] = '\0';
                continue;
            }
            buff[ci - cb] = '\0';
            cb = ci + 1;
            strVector.push_back(std::string(buff));
            nStrings++;
            buff[0] = '\0';
            continue;
        }
        buff[ci - cb] = strIn[ci];
    }
    return nStrings;
}

// Implement growable char array

int ring_buffer::double_capacity() {
    int ds = data_size;

    int ns = ds * 2;
    boost::shared_array<char> tmp(new char[ns]);
    int ci = h_idx;
    int ni = 0;
    for (int i = 0; i < used; i++) {
        tmp[ni] = data[ci];
        ci = (ci + 1) % ds;
        ni++;
    }

    data = boost::shared_array<char>(new char[ns]);
    for (int i = 0; i < used; i++) {
        data[i] = tmp[i];
    }
    h_idx = 0;
    data_size = ns;
    return data_size;
}

std::string ring_buffer::vectorToString(const std::vector<std::string> &strVector, char delim) {
    int nVecs = strVector.size();
    int i;
    if (nVecs == 0) {
        return std::string("");
    }
    std::ostringstream os;
    for (i = 0; i < nVecs - 1; i++) {
        os << strVector[i] << delim;
    }
    os << strVector[i];
    return os.str();
}
