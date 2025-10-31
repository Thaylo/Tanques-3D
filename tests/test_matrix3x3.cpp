/*
 * test_matrix3x3.cpp
 *
 * Comprehensive Google Test unit tests for Matrix3x3 class
 * Tests construction, operations, properties, inverse, and edge cases
 */

#include <gtest/gtest.h>
#include "Matrix3x3.h"
#include "Vector.h"
#include <cmath>

using namespace std;

// Test epsilon for floating-point comparisons
const double EPSILON = 1e-6;

// Helper function to compare matrices element-wise
void ExpectMatrixNear(const Matrix3x3& actual, const Matrix3x3& expected, double epsilon = EPSILON) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(actual.get(i, j), expected.get(i, j), epsilon)
                << "Mismatch at position [" << i << "][" << j << "]";
        }
    }
}

// ============================================================================
// Matrix3x3Construction Tests
// ============================================================================

TEST(Matrix3x3Construction, DefaultConstructor) {
    // Default constructor should create zero matrix
    Matrix3x3 m;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(m.get(i, j), 0.0, EPSILON);
        }
    }
}

TEST(Matrix3x3Construction, ParameterizedConstructor) {
    // Test parameterized constructor with known values
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    EXPECT_NEAR(m.get(0, 0), 1.0, EPSILON);
    EXPECT_NEAR(m.get(0, 1), 2.0, EPSILON);
    EXPECT_NEAR(m.get(0, 2), 3.0, EPSILON);
    EXPECT_NEAR(m.get(1, 0), 4.0, EPSILON);
    EXPECT_NEAR(m.get(1, 1), 5.0, EPSILON);
    EXPECT_NEAR(m.get(1, 2), 6.0, EPSILON);
    EXPECT_NEAR(m.get(2, 0), 7.0, EPSILON);
    EXPECT_NEAR(m.get(2, 1), 8.0, EPSILON);
    EXPECT_NEAR(m.get(2, 2), 9.0, EPSILON);
}

TEST(Matrix3x3Construction, IdentityMatrix) {
    // Test identity matrix factory method
    Matrix3x3 identity = Matrix3x3::identity();

    // Diagonal elements should be 1
    EXPECT_NEAR(identity.get(0, 0), 1.0, EPSILON);
    EXPECT_NEAR(identity.get(1, 1), 1.0, EPSILON);
    EXPECT_NEAR(identity.get(2, 2), 1.0, EPSILON);

    // Off-diagonal elements should be 0
    EXPECT_NEAR(identity.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(identity.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(identity.get(1, 0), 0.0, EPSILON);
    EXPECT_NEAR(identity.get(1, 2), 0.0, EPSILON);
    EXPECT_NEAR(identity.get(2, 0), 0.0, EPSILON);
    EXPECT_NEAR(identity.get(2, 1), 0.0, EPSILON);
}

TEST(Matrix3x3Construction, DiagonalMatrix) {
    // Test diagonal matrix factory method
    Matrix3x3 diag = Matrix3x3::diagonal(2.0, 3.0, 4.0);

    // Diagonal elements should match input
    EXPECT_NEAR(diag.get(0, 0), 2.0, EPSILON);
    EXPECT_NEAR(diag.get(1, 1), 3.0, EPSILON);
    EXPECT_NEAR(diag.get(2, 2), 4.0, EPSILON);

    // Off-diagonal elements should be 0
    EXPECT_NEAR(diag.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(diag.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(diag.get(1, 0), 0.0, EPSILON);
    EXPECT_NEAR(diag.get(1, 2), 0.0, EPSILON);
    EXPECT_NEAR(diag.get(2, 0), 0.0, EPSILON);
    EXPECT_NEAR(diag.get(2, 1), 0.0, EPSILON);
}

TEST(Matrix3x3Construction, ZeroMatrix) {
    // Test zero matrix factory method
    Matrix3x3 zero = Matrix3x3::zero();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(zero.get(i, j), 0.0, EPSILON);
        }
    }
}

// ============================================================================
// Matrix3x3Access Tests
// ============================================================================

TEST(Matrix3x3Access, GetMethod) {
    // Test get method for element access
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    EXPECT_NEAR(m.get(0, 0), 1.0, EPSILON);
    EXPECT_NEAR(m.get(1, 1), 5.0, EPSILON);
    EXPECT_NEAR(m.get(2, 2), 9.0, EPSILON);
    EXPECT_NEAR(m.get(0, 2), 3.0, EPSILON);
    EXPECT_NEAR(m.get(2, 0), 7.0, EPSILON);
}

TEST(Matrix3x3Access, SetMethod) {
    // Test set method for element modification
    Matrix3x3 m;

    m.set(0, 0, 1.5);
    m.set(1, 1, 2.5);
    m.set(2, 2, 3.5);
    m.set(0, 2, 4.5);

    EXPECT_NEAR(m.get(0, 0), 1.5, EPSILON);
    EXPECT_NEAR(m.get(1, 1), 2.5, EPSILON);
    EXPECT_NEAR(m.get(2, 2), 3.5, EPSILON);
    EXPECT_NEAR(m.get(0, 2), 4.5, EPSILON);
}

// ============================================================================
// Matrix3x3Arithmetic Tests
// ============================================================================

TEST(Matrix3x3Arithmetic, Addition) {
    // Test matrix addition
    Matrix3x3 m1(1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0);

    Matrix3x3 m2(9.0, 8.0, 7.0,
                 6.0, 5.0, 4.0,
                 3.0, 2.0, 1.0);

    Matrix3x3 result = m1 + m2;
    Matrix3x3 expected(10.0, 10.0, 10.0,
                       10.0, 10.0, 10.0,
                       10.0, 10.0, 10.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3Arithmetic, Subtraction) {
    // Test matrix subtraction
    Matrix3x3 m1(10.0, 9.0, 8.0,
                 7.0, 6.0, 5.0,
                 4.0, 3.0, 2.0);

    Matrix3x3 m2(1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0);

    Matrix3x3 result = m1 - m2;
    Matrix3x3 expected(9.0, 7.0, 5.0,
                       3.0, 1.0, -1.0,
                       -3.0, -5.0, -7.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3Arithmetic, ScalarMultiplication) {
    // Test scalar multiplication
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Matrix3x3 result = m * 2.0;
    Matrix3x3 expected(2.0, 4.0, 6.0,
                       8.0, 10.0, 12.0,
                       14.0, 16.0, 18.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3Arithmetic, MatrixMultiplication) {
    // Test matrix multiplication with known result
    Matrix3x3 m1(1.0, 2.0, 3.0,
                 4.0, 5.0, 6.0,
                 7.0, 8.0, 9.0);

    Matrix3x3 m2(9.0, 8.0, 7.0,
                 6.0, 5.0, 4.0,
                 3.0, 2.0, 1.0);

    Matrix3x3 result = m1 * m2;

    // Expected result calculated manually:
    // [1*9+2*6+3*3, 1*8+2*5+3*2, 1*7+2*4+3*1] = [30, 24, 18]
    // [4*9+5*6+6*3, 4*8+5*5+6*2, 4*7+5*4+6*1] = [84, 69, 54]
    // [7*9+8*6+9*3, 7*8+8*5+9*2, 7*7+8*4+9*1] = [138, 114, 90]
    Matrix3x3 expected(30.0, 24.0, 18.0,
                       84.0, 69.0, 54.0,
                       138.0, 114.0, 90.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3Arithmetic, IdentityMultiplication) {
    // Matrix multiplied by identity should equal itself
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Matrix3x3 identity = Matrix3x3::identity();
    Matrix3x3 result1 = m * identity;
    Matrix3x3 result2 = identity * m;

    ExpectMatrixNear(result1, m);
    ExpectMatrixNear(result2, m);
}

// ============================================================================
// Matrix3x3VectorOps Tests
// ============================================================================

TEST(Matrix3x3VectorOps, MatrixVectorMultiplication) {
    // Test matrix-vector multiplication
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Vector v(1.0, 2.0, 3.0);
    Vector result = m * v;

    // Expected: [1*1+2*2+3*3, 4*1+5*2+6*3, 7*1+8*2+9*3] = [14, 32, 50]
    EXPECT_NEAR(result.getX(), 14.0, EPSILON);
    EXPECT_NEAR(result.getY(), 32.0, EPSILON);
    EXPECT_NEAR(result.getZ(), 50.0, EPSILON);
}

TEST(Matrix3x3VectorOps, IdentityMatrixVector) {
    // Identity matrix times vector should equal the vector
    Matrix3x3 identity = Matrix3x3::identity();
    Vector v(5.0, -3.0, 7.0);
    Vector result = identity * v;

    EXPECT_NEAR(result.getX(), v.getX(), EPSILON);
    EXPECT_NEAR(result.getY(), v.getY(), EPSILON);
    EXPECT_NEAR(result.getZ(), v.getZ(), EPSILON);
}

TEST(Matrix3x3VectorOps, DiagonalMatrixVector) {
    // Diagonal matrix should scale each component
    Matrix3x3 diag = Matrix3x3::diagonal(2.0, 3.0, 4.0);
    Vector v(1.0, 2.0, 3.0);
    Vector result = diag * v;

    EXPECT_NEAR(result.getX(), 2.0, EPSILON);
    EXPECT_NEAR(result.getY(), 6.0, EPSILON);
    EXPECT_NEAR(result.getZ(), 12.0, EPSILON);
}

// ============================================================================
// Matrix3x3Properties Tests
// ============================================================================

TEST(Matrix3x3Properties, Transpose) {
    // Test transpose operation
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Matrix3x3 result = m.transpose();
    Matrix3x3 expected(1.0, 4.0, 7.0,
                       2.0, 5.0, 8.0,
                       3.0, 6.0, 9.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3Properties, DoubleTranspose) {
    // Transpose of transpose should equal original
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Matrix3x3 result = m.transpose().transpose();
    ExpectMatrixNear(result, m);
}

TEST(Matrix3x3Properties, DeterminantIdentity) {
    // Determinant of identity matrix should be 1
    Matrix3x3 identity = Matrix3x3::identity();
    double det = identity.determinant();

    EXPECT_NEAR(det, 1.0, EPSILON);
}

TEST(Matrix3x3Properties, DeterminantDiagonal) {
    // Determinant of diagonal matrix is product of diagonal elements
    Matrix3x3 diag = Matrix3x3::diagonal(2.0, 3.0, 4.0);
    double det = diag.determinant();

    EXPECT_NEAR(det, 24.0, EPSILON);  // 2 * 3 * 4 = 24
}

TEST(Matrix3x3Properties, DeterminantGeneral) {
    // Test determinant with known value
    Matrix3x3 m(1.0, 2.0, 3.0,
                0.0, 1.0, 4.0,
                5.0, 6.0, 0.0);

    // det = 1*(1*0 - 4*6) - 2*(0*0 - 4*5) + 3*(0*6 - 1*5)
    // det = 1*(-24) - 2*(-20) + 3*(-5)
    // det = -24 + 40 - 15 = 1
    double det = m.determinant();
    EXPECT_NEAR(det, 1.0, EPSILON);
}

TEST(Matrix3x3Properties, DeterminantZero) {
    // Determinant of zero matrix should be 0
    Matrix3x3 zero = Matrix3x3::zero();
    double det = zero.determinant();

    EXPECT_NEAR(det, 0.0, EPSILON);
}

// ============================================================================
// Matrix3x3Inverse Tests
// ============================================================================

TEST(Matrix3x3Inverse, InverseIdentity) {
    // Inverse of identity matrix should be identity
    Matrix3x3 identity = Matrix3x3::identity();
    Matrix3x3 inverse = identity.inverse();

    ExpectMatrixNear(inverse, identity);
}

TEST(Matrix3x3Inverse, InverseDiagonal) {
    // Inverse of diagonal matrix has reciprocal diagonal elements
    Matrix3x3 diag = Matrix3x3::diagonal(2.0, 4.0, 5.0);
    Matrix3x3 inverse = diag.inverse();
    Matrix3x3 expected = Matrix3x3::diagonal(0.5, 0.25, 0.2);

    ExpectMatrixNear(inverse, expected);
}

TEST(Matrix3x3Inverse, InverseGeneral) {
    // Test inverse with known invertible matrix
    Matrix3x3 m(1.0, 2.0, 3.0,
                0.0, 1.0, 4.0,
                5.0, 6.0, 0.0);

    Matrix3x3 inverse = m.inverse();

    // Verify M * M^-1 = I
    Matrix3x3 product = m * inverse;
    Matrix3x3 identity = Matrix3x3::identity();

    ExpectMatrixNear(product, identity);
}

TEST(Matrix3x3Inverse, InverseProperty) {
    // Test (M^-1)^-1 = M
    Matrix3x3 m(2.0, 1.0, 1.0,
                1.0, 3.0, 2.0,
                1.0, 2.0, 3.0);

    Matrix3x3 inverse = m.inverse();
    Matrix3x3 doubleInverse = inverse.inverse();

    ExpectMatrixNear(doubleInverse, m);
}

TEST(Matrix3x3Inverse, SingularMatrix) {
    // Singular matrix (det = 0) should return identity as fallback
    Matrix3x3 singular(1.0, 2.0, 3.0,
                       2.0, 4.0, 6.0,
                       3.0, 6.0, 9.0);

    double det = singular.determinant();
    EXPECT_NEAR(det, 0.0, EPSILON);

    Matrix3x3 inverse = singular.inverse();
    Matrix3x3 identity = Matrix3x3::identity();

    // Implementation returns identity for singular matrices
    ExpectMatrixNear(inverse, identity);
}

// ============================================================================
// Matrix3x3EdgeCases Tests
// ============================================================================

TEST(Matrix3x3EdgeCases, ZeroMatrixOperations) {
    // Test operations with zero matrix
    Matrix3x3 zero = Matrix3x3::zero();
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    // Zero + M = M
    Matrix3x3 result1 = zero + m;
    ExpectMatrixNear(result1, m);

    // M - M = Zero
    Matrix3x3 result2 = m - m;
    ExpectMatrixNear(result2, zero);

    // Zero * M = Zero
    Matrix3x3 result3 = zero * m;
    ExpectMatrixNear(result3, zero);
}

TEST(Matrix3x3EdgeCases, NegativeValues) {
    // Test matrix with negative values
    Matrix3x3 m(-1.0, -2.0, -3.0,
                -4.0, -5.0, -6.0,
                -7.0, -8.0, -9.0);

    Matrix3x3 result = m * (-1.0);
    Matrix3x3 expected(1.0, 2.0, 3.0,
                       4.0, 5.0, 6.0,
                       7.0, 8.0, 9.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3EdgeCases, LargeValues) {
    // Test matrix with large values
    Matrix3x3 m(1e6, 2e6, 3e6,
                4e6, 5e6, 6e6,
                7e6, 8e6, 9e6);

    double det = m.determinant();
    EXPECT_NEAR(det, 0.0, 1e-3);  // This matrix is singular
}

TEST(Matrix3x3EdgeCases, SmallValues) {
    // Test matrix with very small values
    Matrix3x3 m(1e-8, 2e-8, 3e-8,
                4e-8, 5e-8, 6e-8,
                7e-8, 8e-8, 9e-8);

    Matrix3x3 result = m * 1e8;
    Matrix3x3 expected(1.0, 2.0, 3.0,
                       4.0, 5.0, 6.0,
                       7.0, 8.0, 9.0);

    ExpectMatrixNear(result, expected);
}

TEST(Matrix3x3EdgeCases, RotationMatrixProperties) {
    // Test properties of rotation matrices (orthogonal matrices)
    // Create a rotation matrix (90 degrees around Z-axis)
    double angle = M_PI / 2.0;
    Matrix3x3 rot(cos(angle), -sin(angle), 0.0,
                  sin(angle), cos(angle), 0.0,
                  0.0, 0.0, 1.0);

    // For rotation matrices: R^T * R = I
    Matrix3x3 product = rot.transpose() * rot;
    Matrix3x3 identity = Matrix3x3::identity();

    ExpectMatrixNear(product, identity);

    // Determinant of rotation matrix should be 1
    double det = rot.determinant();
    EXPECT_NEAR(det, 1.0, EPSILON);
}

TEST(Matrix3x3EdgeCases, SymmetricMatrix) {
    // Test symmetric matrix (M = M^T)
    Matrix3x3 m(1.0, 2.0, 3.0,
                2.0, 4.0, 5.0,
                3.0, 5.0, 6.0);

    Matrix3x3 transpose = m.transpose();
    ExpectMatrixNear(m, transpose);
}

// ============================================================================
// Additional Mathematical Properties Tests
// ============================================================================

TEST(Matrix3x3Properties, TransposeProductProperty) {
    // Test (AB)^T = B^T A^T
    Matrix3x3 A(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 10.0);

    Matrix3x3 B(2.0, 0.0, 1.0,
                1.0, 3.0, 2.0,
                0.0, 1.0, 4.0);

    Matrix3x3 lhs = (A * B).transpose();
    Matrix3x3 rhs = B.transpose() * A.transpose();

    ExpectMatrixNear(lhs, rhs);
}

TEST(Matrix3x3Properties, ScalarDistributivity) {
    // Test scalar distributivity: k(A + B) = kA + kB
    Matrix3x3 A(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    Matrix3x3 B(9.0, 8.0, 7.0,
                6.0, 5.0, 4.0,
                3.0, 2.0, 1.0);

    double k = 3.5;

    Matrix3x3 lhs = (A + B) * k;
    Matrix3x3 rhs = (A * k) + (B * k);

    ExpectMatrixNear(lhs, rhs);
}
