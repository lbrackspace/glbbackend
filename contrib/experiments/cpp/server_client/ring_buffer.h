#ifndef RING_BUFFER_H
#define	RING_BUFFER_H

#include<boost/shared_array.hpp>
#include<string>
#include<vector>
#include<sstream>

class ring_buffer {
private:
    boost::shared_array<char> data;
    int data_size;
    int h_idx;
    int used;
public:
    ring_buffer(int ds);
    ring_buffer(const ring_buffer& orig);
    ring_buffer & operator=(const ring_buffer& other);
    int read(char *str, int max_size);
    std::string readLine();
    std::string read(int max_size);
    int linesAvailable() const;
    int write(const char *str, int write_size);
    int write(std::string str);
    int dec(int dec_size);
    int used_size() const;
    int free_size() const;
    virtual ~ring_buffer();
    std::string debug_str(bool showBuffer) const;
    static std::string vectorToString(const std::vector<std::string>& strVector,char delim);
    static int stringToVector(const std::string& strIn,std::vector<std::string> &strVersion,char delim,bool skipLF);
};

#endif	/* RING_BUFFER_H */

