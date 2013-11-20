#ifndef BITARRAY_H
#define	BITARRAY_H

#include<boost/shared_array.hpp>
#include<string>

class BitArray {
private:
    int nBits;
    boost::shared_array<unsigned int> bits;
public:
    BitArray();
    BitArray(int nBits);
    BitArray(const BitArray& orig);
    int getBit(int bit_pos);
    void setBit(int bit_pos, int val);
    int getNBits()const;
    std::string to_hex();
    std::string to_bin();
    virtual ~BitArray();
};

extern const int INT_NBITS;
extern const unsigned int allOnes;
#endif	/* BITARRAY_H */

