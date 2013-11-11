#include<Point.h>
#include<string>
#include<iostream>
#include<sstream>

using namespace std;

Point::Point() {
    cout << "Point() called" << endl;
}

Point::Point(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
    cout << "Point(" << x << "," << y << "," << z << ") called" << endl;
}

Point::~Point() {
    cout << "~Point(" << x << "," << y << "," << z << ") called" << endl;
}

Point Point::operator +(Point& oth) {
    Point out;
    out.x = this->x + oth.x;
    out.y = this->y + oth.y;
    out.z = this->z + oth.z;
    return out;
}

string Point::str() {
    ostringstream os;
    os << "(" << x << "," << y << "," << z << ")";
    return os.str();
}

ostream & operator<<(ostream &os, const Point&p) {
    os << "(" << p.x << "," << p.y << "," << p.z << ")";
    return os;
}

double dot(const Point &p1, const Point &p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}