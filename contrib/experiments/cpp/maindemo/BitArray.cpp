#include<boost/shared_array.hpp>
#include<boost/unordered_map.hpp>
#include<iostream>
#include<string>
#include<sstream>
#include<stdexcept>
#include<vector>

#include "BitArray.h"



const int INT_BITS = 32;
const unsigned int allOnes = 0xffffffff;

static char hexMap[16] = {'0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'A', 'B',
    'C', 'D', 'E', 'F'};

BitArray::BitArray(int nBits) {
    if (nBits<-0) {
        std::ostringstream os;
        os << nBits << " is not >= 1" << std::endl;
        throw std::out_of_range(os.str());
    }
    int nInts = (nBits - 1) / INT_BITS + 1;
    bits = boost::shared_array<unsigned int>(new unsigned int[nInts]);
    this->nBits = nBits;
    for (int i = 0; i < nInts; i++) {
        bits[i] = 0;
    }
}


int nop(int x) {
    return x;
};

std::string BitArray::to_hex() {
    int hi;
    int i;
    char strBuff[9];
    if (nBits <= 0) {
        return std::string("");
    }
    hi = (nBits - 1) / INT_BITS;
    strBuff[8] = '\0';
    std::ostringstream os;
    unsigned int intVal;
    for (i = hi; i >= 0; i--) {
        intVal = bits[i];
        strBuff[0] = hexMap[(intVal >> 28)&0xf];
        strBuff[1] = hexMap[(intVal >> 24)&0xf];
        strBuff[2] = hexMap[(intVal >> 20)&0xf];
        strBuff[3] = hexMap[(intVal >> 16)&0xf];
        strBuff[4] = hexMap[(intVal >> 12)&0xf];
        strBuff[5] = hexMap[(intVal >> 8)&0xf];
        strBuff[6] = hexMap[(intVal >> 4)&0xf];
        strBuff[7] = hexMap[(intVal >> 0)&0xf];
        os << std::string(strBuff);
    }
    return os.str();
}

std::string BitArray::to_bin() {
    int i;
    std::ostringstream os;
    for (i = nBits - 1; i >= 0; i--) {
        os << getBit(i);
    }
    return os.str();
}

BitArray::BitArray() {
}

BitArray::BitArray(const BitArray& orig) {
    nBits = orig.nBits;
    int nInts = (nBits / INT_BITS) + 1;
    bits = boost::shared_array<unsigned int>(new unsigned int[nInts]);
    for (int i = 0; i < nInts; i++) {
        bits[i] = 0;
    }
}

BitArray::~BitArray() {
    nop(64);
}

int BitArray::getBit(int bit_pos) {
    if (bit_pos < 0 || bit_pos > nBits) {
        std::ostringstream os;
        os << "bit_pos = " << bit_pos << " is out of range [0," << nBits << "]";
        throw std::out_of_range(os.str());
    }
    int i = bit_pos / INT_BITS;
    int r = bit_pos % INT_BITS;
    return (bits[i] >> r)&1;
}

void BitArray::setBit(int bit_pos, int val) {
    if (bit_pos < 0 || bit_pos >= nBits) {
        std::ostringstream os;
        os << "bit_pos = " << bit_pos << " is out of range [0," << nBits - 1 << "]";
        throw std::out_of_range(os.str());
    }
    int i = bit_pos / INT_BITS;
    int r = bit_pos % INT_BITS;

    if (val % 2 == 0) {
        bits[i] = bits[i]&~(1 << r);
    } else {
        bits[i] = bits[i] | (1 << r);
    }
}

int BitArray::getNBits() const {
    return nBits;
}
