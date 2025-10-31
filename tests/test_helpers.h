/*
 * test_helpers.h
 *
 *  Created on: 2025-01-31
 *      Author: Test Infrastructure
 *
 *  Test helper macros and utilities for Google Test
 *  Provides comparison functions for Vector, Matrix3x3, and Quaternion classes
 */

#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#include <gtest/gtest.h>
#include <cmath>
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"

// Epsilon for floating point comparisons
constexpr double TEST_EPSILON = 1e-6;

// Custom assertion macros for Vector comparison
// Compares two Vector objects component-wise with epsilon tolerance
#define EXPECT_VECTOR_NEAR(v1, v2, epsilon) \
    do { \
        EXPECT_NEAR((v1).getX(), (v2).getX(), (epsilon)) << "X components differ"; \
        EXPECT_NEAR((v1).getY(), (v2).getY(), (epsilon)) << "Y components differ"; \
        EXPECT_NEAR((v1).getZ(), (v2).getZ(), (epsilon)) << "Z components differ"; \
    } while(0)

// Convenience macro using default epsilon
#define EXPECT_VECTOR_EQ(v1, v2) EXPECT_VECTOR_NEAR(v1, v2, TEST_EPSILON)

// Custom assertion macros for Matrix3x3 comparison
// Compares two Matrix3x3 objects element-wise with epsilon tolerance
#define EXPECT_MATRIX_NEAR(m1, m2, epsilon) \
    do { \
        for (int row = 0; row < 3; ++row) { \
            for (int col = 0; col < 3; ++col) { \
                EXPECT_NEAR((m1).get(row, col), (m2).get(row, col), (epsilon)) \
                    << "Matrix element [" << row << "][" << col << "] differs"; \
            } \
        } \
    } while(0)

// Convenience macro using default epsilon
#define EXPECT_MATRIX_EQ(m1, m2) EXPECT_MATRIX_NEAR(m1, m2, TEST_EPSILON)

// Custom assertion macros for Quaternion comparison
// Compares two Quaternion objects component-wise with epsilon tolerance
#define EXPECT_QUATERNION_NEAR(q1, q2, epsilon) \
    do { \
        EXPECT_NEAR((q1).getW(), (q2).getW(), (epsilon)) << "W components differ"; \
        EXPECT_NEAR((q1).getX(), (q2).getX(), (epsilon)) << "X components differ"; \
        EXPECT_NEAR((q1).getY(), (q2).getY(), (epsilon)) << "Y components differ"; \
        EXPECT_NEAR((q1).getZ(), (q2).getZ(), (epsilon)) << "Z components differ"; \
    } while(0)

// Convenience macro using default epsilon
#define EXPECT_QUATERNION_EQ(q1, q2) EXPECT_QUATERNION_NEAR(q1, q2, TEST_EPSILON)

// Helper function to check if two doubles are approximately equal
inline bool approximatelyEqual(double a, double b, double epsilon = TEST_EPSILON) {
    return std::abs(a - b) < epsilon;
}

// Helper function to check if a double is approximately zero
inline bool approximatelyZero(double value, double epsilon = TEST_EPSILON) {
    return std::abs(value) < epsilon;
}

// Helper function to create a normalized vector for testing
inline Vector makeNormalizedVector(double x, double y, double z) {
    Vector v(x, y, z);
    double length = v.getLengthVector();
    if (length > TEST_EPSILON) {
        return v / length;
    }
    return Vector(1, 0, 0); // Default if zero vector
}

// Helper function to check if vectors are parallel (same or opposite direction)
inline bool areVectorsParallel(const Vector& v1, const Vector& v2, double epsilon = TEST_EPSILON) {
    Vector cross = v1.crossProduct(v2);
    return approximatelyZero(cross.getLengthVector(), epsilon);
}

// Helper function to check if vectors are perpendicular
inline bool areVectorsPerpendicular(const Vector& v1, const Vector& v2, double epsilon = TEST_EPSILON) {
    return approximatelyZero(v1.dotProduct(v2), epsilon);
}

// Helper function to check if a matrix is identity
inline bool isIdentityMatrix(const std::Matrix3x3& m, double epsilon = TEST_EPSILON) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            double expected = (i == j) ? 1.0 : 0.0;
            if (!approximatelyEqual(m.get(i, j), expected, epsilon)) {
                return false;
            }
        }
    }
    return true;
}

// Helper function to check if a quaternion is normalized
inline bool isQuaternionNormalized(const std::Quaternion& q, double epsilon = TEST_EPSILON) {
    return approximatelyEqual(q.norm(), 1.0, epsilon);
}

// Helper function to print vector for debugging
inline std::string vectorToString(const Vector& v) {
    return "Vector(" + std::to_string(v.getX()) + ", " +
           std::to_string(v.getY()) + ", " +
           std::to_string(v.getZ()) + ")";
}

// Helper function to print quaternion for debugging
inline std::string quaternionToString(const std::Quaternion& q) {
    return "Quaternion(w=" + std::to_string(q.getW()) +
           ", x=" + std::to_string(q.getX()) +
           ", y=" + std::to_string(q.getY()) +
           ", z=" + std::to_string(q.getZ()) + ")";
}

#endif /* TEST_HELPERS_H_ */
