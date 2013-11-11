#include "Matrix.h"

#include<iostream>
#include<stdexcept>
#include<string>
#include<sstream>
#include<istream>
#include<math.h>

Matrix::Matrix(std::string strIn) {
    double val;
    std::istringstream is(strIn);
    is >> nRows;
    is >> nCols;
    values = new double[nRows * nCols];
    for (int r = 1; r <= nRows; r++) {
        for (int c = 1; c <= nCols; c++) {
            is >> val;
            set(r, c, val);
        }
    }
}

Matrix::Matrix(const Matrix& orig) {
    nRows = orig.getNRows();
    nCols = orig.getNCols();
    values = new double[nRows * nCols];
    copyMatrix(this, &orig);
}

Matrix::Matrix(int rows, int cols) {
    this->nRows = rows;
    this->nCols = cols;
    values = new double[rows * cols];
}

Matrix::~Matrix() {
    if (values != NULL) {
        delete[] values;
    }

    values = NULL;
    nRows = 0;
    nCols = 0;
}

void Matrix::rowMul(int r, double scaler) {
    double value;
    for (int i = 1; i <= nCols; i++) {
        value = get(r, i);
        value *= scaler;
        set(r, i, value);
    }
}

void Matrix::rowAddMul(int dstRow, int srcRow, double scalerValue) {
    double tmpValue;
    for (int c = 1; c <= nCols; c++) {
        tmpValue = get(srcRow, c) * scalerValue + get(dstRow, c);
        set(dstRow, c, tmpValue);
    }
}

Matrix *Matrix::incremental(int rows, int cols) {
    Matrix *mat = new Matrix(rows, cols);
    double curr = 1.0;
    for (int r = 1; r <= rows; r++) {
        for (int j = 1; j <= cols; j++) {
            mat->set(r, j, curr);
            curr += 1.0;
        }
    }
    return mat;
}

void Matrix::rref() {
    int l = 0;
    int r = 0;
    int i = 0;
    double v = 0.0;
    for (r = 0; r < nRows; r++) {
        if (nCols <= l) {
            break;
        }
        i = r;
        while (get(i + 1, l + 1) == 0.0) {
            i++;
            if (nRows == i) {
                i = r;
                l++;
                if (nCols == l) {
                    break;
                }
            }
        }
        rowSwap(i + 1, r + 1);
        v = get(r + 1, l + 1);
        if (v != 0.0) {
            rowMul(r + 1, 1 / v);
        }
        for (i = 0; i < nRows; i++) {
            if (i != r) {
                v = get(i + 1, l + 1);
                rowAddMul(i + 1, r + 1, 0 - v);
            }
        }
        l++;
    }
}

Matrix *Matrix::identity(int n) {
    Matrix *m = new Matrix(n, n);
    for (int r = 1; r <= n; r++) {
        for (int c = 1; c <= n; c++) {
            if (r == c)
                m->set(r, c, 1.0);
            else
                m->set(r, c, 0.0);

        }
    }
    return m;
}

void Matrix::rowSwap(int r1, int r2) {
    double swp;
    for (int i = 1; i <= nCols; i++) {
        swp = get(r1, i);
        set(r1, i, get(r2, i));
        set(r2, i, swp);
    }
}

std::ostream & operator<<(std::ostream &os, const Matrix &m) {
    os << "Matrix " << m.nRows << " x " << m.nCols << " @" << &m << std::endl;
    for (int r = 1; r <= m.nRows; r++) {
        for (int c = 1; c <= m.nCols; c++) {
            os << m.get(r, c);
            if (c < m.nCols) {
                os << " ";
            }
        }
        os << ";" << std::endl;
    }
    return os;
}

std::string Matrix::to_string() {
    std::ostringstream os;
    os << "Matrix " << nRows << " x " << nCols << " @" << this << std::endl;
    for (int r = 1; r <= nRows; r++) {
        for (int c = 1; c <= nCols; c++) {
            os << get(r, c);
            if (c < nCols) {
                os << " ";
            }
        }
        os << ";" << std::endl;
    }
    std::string str = os.str();
    return str;
}

void Matrix::copyMatrix(Matrix* dst, const Matrix* src) {
    double val;
    int nRows = src->getNRows();
    int nCols = src->getNCols();
    for (int r = 1; r <= nRows; r++) {
        for (int c = 1; c <= nCols; c++) {
            val = src->get(r, c);
            dst->set(r, c, val);
        }
    }
}

bool Matrix::dequ(double v1, double v2, double tol) {
    double v = v1 - v2;
    if (v < 0.0) {
        v = 0 - v;
    }
    return v < tol;
}

int Matrix::maxColForRow(int c) {
    double maxVal = 0.0;
    int maxRow = c;
    for (int r = c; r <= nRows; r++) {
        double val = fabs(get(r, c));
        if (val > maxVal) {
            maxVal = val;
            maxRow = r;
        }
    }
    return maxRow;
}

void Matrix::mulMatMat(Matrix *r, Matrix* a, Matrix* b) {
    int nRows = a->getNRows();
    int nCols = b->getNCols();
    int nInner = a->getNCols();
    int i;
    int j;
    int k;
    double v;
    if (nInner != b->nRows) {
        throw std::invalid_argument("first matrix parameter colum count does not match second matrix parameters row count");
    }
    if (r->values != NULL) {
        delete r->values;
        r->values = NULL;
    }
    r->nRows = nRows;
    r->nCols = nCols;
    r->values = new double[nRows * nCols];
    for (i = 1; i <= nRows; i++) {
        for (j = 1; j <= nCols; j++) {
            v = 0.0;
            for (k = 1; k <= nInner; k++) {
                v += a->get(i, k) * b->get(k, j);
            }
            r->set(i, j, v);
        }
    }
}