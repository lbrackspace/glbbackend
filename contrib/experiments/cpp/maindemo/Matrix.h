#ifndef MATRIX_H
#define	MATRIX_H

#include<iostream>
#include<sstream>
#include<string>
#include<ostream>

class Matrix {
public:
    Matrix(std::string strIn);
    Matrix(int rows, int cols);
    Matrix(const Matrix& orig);

    double get(int row, int col) const {
        return values[(row - 1) * nCols + (col - 1)];
    }

    void set(int row, int col, double val) {
        values[(row - 1) * nCols + (col - 1)] = val;
    }

    int getNRows() const {
        return nRows;
    };

    int getNCols() const {
        return nCols;
    };
    virtual ~Matrix();
    void rowSwap(int r1, int r2);
    void rowMul(int r, double scalerVal);
    void rowAddMul(int dstRow, int srcRow, double scalerValue);
    void rref();
    static Matrix *identity(int n);
    static void copyMatrix(Matrix *dst, const Matrix *src);
    static Matrix *incremental(int m, int n);
    std::string to_string();
    static void mulMatMat(Matrix* r, Matrix* a, Matrix* b);
    friend std::ostream & operator<<(std::ostream &os, const Matrix &m);
private:
    int nRows;
    int nCols;
    double *values;
    bool dequ(double v1, double v2, double tol);
    int maxColForRow(int c);

};

#endif	/* MATRIX_H */

