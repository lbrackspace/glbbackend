#include<string>
#include<iostream>
#include<iomanip>
#include<sstream>
#include<boost/shared_array.hpp>
#include "ring_buffer.h"

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
    int u = used;
    int ds = data_size;
    int di = 0;
    int ri = h_idx;
    if (ds <= 0) {
        return 0;
    }
    while (di < max_size && u > 0) {
        strPtr[di] = data[ri];
        ri = (ri + 1) % ds;
        di++;
        u--;
    }
    used = u;
    return di;
}

int ring_buffer::free_size() const {
    return data_size - used;
}

std::string ring_buffer::debug_str(bool showBuffer) const {
    std::ostringstream os;
    int ds = data_size;
    int h = h_idx;
    int u = used;
    if (showBuffer) {
        for (int i = 0; i < ds; i++) {
            const char ch = data[i];
            std::string strRepr = std::string(1, data[i]);
            if (ch == '\n') {
                strRepr = "\\n";
            }
            os << "rb[" << std::setw(4) << i << "]= '" << strRepr << "'";
            if ((h + u >= ds) && (i <= (h + u) % ds || (i > h))) {
                os << "*" << std::endl;
            } else if ((i >= h) && (i < h + u)) {
                os << "*" << std::endl;
            } else {
                os << std::endl;
            }

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