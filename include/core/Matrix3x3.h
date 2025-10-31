/*
 * Matrix3x3.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  3x3 Matrix class for inertia tensor calculations and rotations
 */

#ifndef MATRIX3X3_H_
#define MATRIX3X3_H_

#include "Vector.h"

namespace std {

class Matrix3x3 {
private:
    // Matrix stored in row-major order: m[row][col]
    double m[3][3];

public:
    // Constructors
    Matrix3x3();
    Matrix3x3(double m00, double m01, double m02,
              double m10, double m11, double m12,
              double m20, double m21, double m22);

    // Factory methods
    static Matrix3x3 identity();
    static Matrix3x3 diagonal(double d0, double d1, double d2);
    static Matrix3x3 zero();

    // Element access
    double get(int row, int col) const;
    void set(int row, int col, double value);

    // Matrix operations
    Matrix3x3 operator+(const Matrix3x3 &other) const;
    Matrix3x3 operator-(const Matrix3x3 &other) const;
    Matrix3x3 operator*(double scalar) const;
    Matrix3x3 operator*(const Matrix3x3 &other) const;
    Vector operator*(const Vector &v) const;  // Matrix-vector multiplication

    Matrix3x3 transpose() const;
    double determinant() const;
    Matrix3x3 inverse() const;

    // Utilities
    void print() const;  // Debug output

    ~Matrix3x3() {}
};

} /* namespace std */

#endif /* MATRIX3X3_H_ */
