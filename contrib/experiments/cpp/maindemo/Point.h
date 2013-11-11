#ifndef POINT_H
#define	POINT_H

#include<string>

using namespace std;

class Point {
public:
    Point();
    Point(double _x, double _y, double _z);
    string str();
    Point operator+(Point &oth);
    virtual ~Point();

    double getX()const {
        return x;
    }

    double getY()const {
        return y;
    }

    double getZ()const {
        return z;
    }
    friend ostream & operator<<(ostream& os, const Point &p);
    friend double dot(const Point &p1, const Point &p2);
private:
    double x;
    double y;
    double z;
};

#endif	/* POINT_H */

