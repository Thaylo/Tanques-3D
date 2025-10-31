/*
 * test_example.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Test Infrastructure
 *
 *  Example test file demonstrating Google Test usage with test helpers
 *  This file shows how to write tests for Vector, Matrix3x3, and Quaternion classes
 */

#include <gtest/gtest.h>
#include "test_helpers.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"

// ============================================================================
// Vector Tests
// ============================================================================

// Test fixture for Vector tests (shared setup/teardown)
class VectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup - runs before each test
        v1 = Vector(1.0, 2.0, 3.0);
        v2 = Vector(4.0, 5.0, 6.0);
        zero = Vector(0.0, 0.0, 0.0);
    }

    // Shared test data
    Vector v1;
    Vector v2;
    Vector zero;
};

// Example: Basic construction test
TEST_F(VectorTest, Construction) {
    Vector v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}

// Example: Vector addition test
TEST_F(VectorTest, Addition) {
    Vector result = v1 + v2;
    EXPECT_VECTOR_EQ(result, Vector(5.0, 7.0, 9.0));
}

// Example: Vector subtraction test
TEST_F(VectorTest, Subtraction) {
    Vector result = v2 - v1;
    EXPECT_VECTOR_EQ(result, Vector(3.0, 3.0, 3.0));
}

// Example: Scalar multiplication test
TEST_F(VectorTest, ScalarMultiplication) {
    Vector result = v1 * 2.0;
    EXPECT_VECTOR_EQ(result, Vector(2.0, 4.0, 6.0));
}

// Example: Dot product test
TEST_F(VectorTest, DotProduct) {
    double result = v1.dotProduct(v2);
    // (1*4) + (2*5) + (3*6) = 4 + 10 + 18 = 32
    EXPECT_NEAR(result, 32.0, TEST_EPSILON);
}

// Example: Cross product test
TEST_F(VectorTest, CrossProduct) {
    Vector x_axis(1, 0, 0);
    Vector y_axis(0, 1, 0);
    Vector result = x_axis.crossProduct(y_axis);

    // x cross y = z
    EXPECT_VECTOR_EQ(result, Vector(0, 0, 1));
}

// Example: Vector length test
TEST_F(VectorTest, Length) {
    Vector v(3.0, 4.0, 0.0);  // 3-4-5 triangle
    double length = v.getLengthVector();
    EXPECT_NEAR(length, 5.0, TEST_EPSILON);
}

// Example: Normalization test
TEST_F(VectorTest, Normalization) {
    Vector v(3.0, 4.0, 0.0);
    Vector normalized = v.getNormalVector();

    // Check length is 1
    EXPECT_NEAR(normalized.getLengthVector(), 1.0, TEST_EPSILON);

    // Check direction is preserved
    EXPECT_VECTOR_NEAR(normalized, Vector(0.6, 0.8, 0.0), TEST_EPSILON);
}

// Example: Test using helper function
TEST_F(VectorTest, PerpendicularVectors) {
    Vector x_axis(1, 0, 0);
    Vector y_axis(0, 1, 0);

    EXPECT_TRUE(areVectorsPerpendicular(x_axis, y_axis));
}

// ============================================================================
// Matrix Tests
// ============================================================================

class MatrixTest : public ::testing::Test {
protected:
    void SetUp() override {
        identity = std::Matrix3x3::identity();
        zero = std::Matrix3x3::zero();
    }

    std::Matrix3x3 identity;
    std::Matrix3x3 zero;
};

// Example: Identity matrix test
TEST_F(MatrixTest, IdentityMatrix) {
    EXPECT_TRUE(isIdentityMatrix(identity));

    // Verify specific elements
    EXPECT_DOUBLE_EQ(identity.get(0, 0), 1.0);
    EXPECT_DOUBLE_EQ(identity.get(1, 1), 1.0);
    EXPECT_DOUBLE_EQ(identity.get(2, 2), 1.0);
    EXPECT_DOUBLE_EQ(identity.get(0, 1), 0.0);
}

// Example: Matrix multiplication by identity
TEST_F(MatrixTest, MultiplicationByIdentity) {
    std::Matrix3x3 m(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 result = m * identity;
    EXPECT_MATRIX_EQ(result, m);
}

// Example: Matrix-vector multiplication
TEST_F(MatrixTest, MatrixVectorMultiplication) {
    std::Matrix3x3 m(1, 0, 0,
                     0, 2, 0,
                     0, 0, 3);

    Vector v(1, 1, 1);
    Vector result = m * v;

    EXPECT_VECTOR_EQ(result, Vector(1, 2, 3));
}

// Example: Matrix transpose test
TEST_F(MatrixTest, Transpose) {
    std::Matrix3x3 m(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 transposed = m.transpose();
    std::Matrix3x3 expected(1, 4, 7,
                            2, 5, 8,
                            3, 6, 9);

    EXPECT_MATRIX_EQ(transposed, expected);
}

// ============================================================================
// Quaternion Tests
// ============================================================================

class QuaternionTest : public ::testing::Test {
protected:
    void SetUp() override {
        identity = std::Quaternion::identity();
    }

    std::Quaternion identity;
};

// Example: Identity quaternion test
TEST_F(QuaternionTest, IdentityQuaternion) {
    EXPECT_DOUBLE_EQ(identity.getW(), 1.0);
    EXPECT_DOUBLE_EQ(identity.getX(), 0.0);
    EXPECT_DOUBLE_EQ(identity.getY(), 0.0);
    EXPECT_DOUBLE_EQ(identity.getZ(), 0.0);

    EXPECT_TRUE(isQuaternionNormalized(identity));
}

// Example: Quaternion normalization test
TEST_F(QuaternionTest, Normalization) {
    std::Quaternion q(1.0, 2.0, 3.0, 4.0);
    std::Quaternion normalized = q.normalize();

    EXPECT_TRUE(isQuaternionNormalized(normalized));
    EXPECT_NEAR(normalized.norm(), 1.0, TEST_EPSILON);
}

// Example: Rotation by identity quaternion
TEST_F(QuaternionTest, RotationByIdentity) {
    Vector v(1, 2, 3);
    Vector rotated = identity.rotate(v);

    // Identity rotation should not change the vector
    EXPECT_VECTOR_EQ(rotated, v);
}

// Example: 90-degree rotation around Z-axis
TEST_F(QuaternionTest, RotationAroundZAxis) {
    // Create quaternion for 90-degree rotation around Z-axis
    Vector z_axis(0, 0, 1);
    double angle = M_PI / 2.0;  // 90 degrees in radians

    std::Quaternion q = std::Quaternion::fromAxisAngle(z_axis, angle);

    // Rotate X-axis vector (1,0,0) around Z by 90 degrees -> should give Y-axis (0,1,0)
    Vector x_axis(1, 0, 0);
    Vector rotated = q.rotate(x_axis);

    EXPECT_VECTOR_NEAR(rotated, Vector(0, 1, 0), TEST_EPSILON);
}

// ============================================================================
// Integration Tests (combining multiple classes)
// ============================================================================

TEST(IntegrationTest, QuaternionToMatrixConversion) {
    // Create a quaternion for 90-degree rotation around Z-axis
    Vector z_axis(0, 0, 1);
    double angle = M_PI / 2.0;

    std::Quaternion q = std::Quaternion::fromAxisAngle(z_axis, angle);
    std::Matrix3x3 rotation_matrix = q.toRotationMatrix();

    // Rotate vector using quaternion
    Vector v(1, 0, 0);
    Vector rotated_by_quat = q.rotate(v);

    // Rotate same vector using matrix
    Vector rotated_by_matrix = rotation_matrix * v;

    // Results should be identical
    EXPECT_VECTOR_NEAR(rotated_by_quat, rotated_by_matrix, TEST_EPSILON);
}

// Example: Edge case - zero vector
TEST(EdgeCaseTest, ZeroVectorLength) {
    Vector zero(0, 0, 0);
    EXPECT_DOUBLE_EQ(zero.getLengthVector(), 0.0);
}
