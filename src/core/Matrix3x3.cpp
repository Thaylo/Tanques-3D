/*
 * Matrix3x3.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "Matrix3x3.h"
#include <iostream>
#include <cmath>

namespace std {

// Constructors
Matrix3x3::Matrix3x3() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m[i][j] = 0.0;
        }
    }
}

Matrix3x3::Matrix3x3(double m00, double m01, double m02,
                     double m10, double m11, double m12,
                     double m20, double m21, double m22) {
    m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
    m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
    m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
}

// Factory methods
Matrix3x3 Matrix3x3::identity() {
    return Matrix3x3(1, 0, 0,
                     0, 1, 0,
                     0, 0, 1);
}

Matrix3x3 Matrix3x3::diagonal(double d0, double d1, double d2) {
    return Matrix3x3(d0, 0, 0,
                     0, d1, 0,
                     0, 0, d2);
}

Matrix3x3 Matrix3x3::zero() {
    return Matrix3x3(0, 0, 0,
                     0, 0, 0,
                     0, 0, 0);
}

// Element access
double Matrix3x3::get(int row, int col) const {
    return m[row][col];
}

void Matrix3x3::set(int row, int col, double value) {
    m[row][col] = value;
}

// Matrix operations
Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &other) const {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = m[i][j] + other.m[i][j];
        }
    }
    return result;
}

Matrix3x3 Matrix3x3::operator-(const Matrix3x3 &other) const {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = m[i][j] - other.m[i][j];
        }
    }
    return result;
}

Matrix3x3 Matrix3x3::operator*(double scalar) const {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = m[i][j] * scalar;
        }
    }
    return result;
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &other) const {
    Matrix3x3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0.0;
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }
    return result;
}

Vector Matrix3x3::operator*(const Vector &v) const {
    return Vector(
        m[0][0] * v.getX() + m[0][1] * v.getY() + m[0][2] * v.getZ(),
        m[1][0] * v.getX() + m[1][1] * v.getY() + m[1][2] * v.getZ(),
        m[2][0] * v.getX() + m[2][1] * v.getY() + m[2][2] * v.getZ()
    );
}

Matrix3x3 Matrix3x3::transpose() const {
    return Matrix3x3(m[0][0], m[1][0], m[2][0],
                     m[0][1], m[1][1], m[2][1],
                     m[0][2], m[1][2], m[2][2]);
}

double Matrix3x3::determinant() const {
    // Using the rule of Sarrus for 3x3 determinant
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
         - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
         + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

Matrix3x3 Matrix3x3::inverse() const {
    double det = determinant();

    // Check for singular matrix
    if (fabs(det) < 1e-10) {
        // Return identity matrix if singular (should not happen with valid inertia tensors)
        return Matrix3x3::identity();
    }

    // Calculate adjugate matrix (transpose of cofactor matrix)
    Matrix3x3 adj;

    adj.m[0][0] = (m[1][1] * m[2][2] - m[1][2] * m[2][1]);
    adj.m[0][1] = -(m[0][1] * m[2][2] - m[0][2] * m[2][1]);
    adj.m[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]);

    adj.m[1][0] = -(m[1][0] * m[2][2] - m[1][2] * m[2][0]);
    adj.m[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]);
    adj.m[1][2] = -(m[0][0] * m[1][2] - m[0][2] * m[1][0]);

    adj.m[2][0] = (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
    adj.m[2][1] = -(m[0][0] * m[2][1] - m[0][1] * m[2][0]);
    adj.m[2][2] = (m[0][0] * m[1][1] - m[0][1] * m[1][0]);

    // Transpose and divide by determinant
    return adj.transpose() * (1.0 / det);
}

void Matrix3x3::print() const {
    cout << "[" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << "]" << endl;
    cout << "[" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << "]" << endl;
    cout << "[" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << "]" << endl;
}

} /* namespace std */
