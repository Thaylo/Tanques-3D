/*
 * test_quaternion.cpp
 *
 * Comprehensive Google Test unit tests for Quaternion class
 * Tests construction, factory methods, arithmetic, rotation, conversion, and edge cases
 */

#include <gtest/gtest.h>
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include <cmath>

using namespace std;

// Test epsilon for floating-point comparisons
const double EPSILON = 1e-6;

// Helper function to compare quaternions component-wise
void ExpectQuaternionNear(const Quaternion& actual, const Quaternion& expected, double epsilon = EPSILON) {
    EXPECT_NEAR(actual.getW(), expected.getW(), epsilon) << "W component mismatch";
    EXPECT_NEAR(actual.getX(), expected.getX(), epsilon) << "X component mismatch";
    EXPECT_NEAR(actual.getY(), expected.getY(), epsilon) << "Y component mismatch";
    EXPECT_NEAR(actual.getZ(), expected.getZ(), epsilon) << "Z component mismatch";
}

// Helper function to compare vectors component-wise
void ExpectVectorNear(const Vector& actual, const Vector& expected, double epsilon = EPSILON) {
    EXPECT_NEAR(actual.getX(), expected.getX(), epsilon) << "X component mismatch";
    EXPECT_NEAR(actual.getY(), expected.getY(), epsilon) << "Y component mismatch";
    EXPECT_NEAR(actual.getZ(), expected.getZ(), epsilon) << "Z component mismatch";
}

// ============================================================================
// QuaternionConstruction Tests
// ============================================================================

TEST(QuaternionConstruction, DefaultConstructor) {
    // Default constructor should create identity quaternion (1, 0, 0, 0)
    Quaternion q;

    EXPECT_NEAR(q.getW(), 1.0, EPSILON);
    EXPECT_NEAR(q.getX(), 0.0, EPSILON);
    EXPECT_NEAR(q.getY(), 0.0, EPSILON);
    EXPECT_NEAR(q.getZ(), 0.0, EPSILON);
}

TEST(QuaternionConstruction, ParameterizedConstructor) {
    // Test parameterized constructor with known values
    Quaternion q(0.5, 0.5, 0.5, 0.5);

    EXPECT_NEAR(q.getW(), 0.5, EPSILON);
    EXPECT_NEAR(q.getX(), 0.5, EPSILON);
    EXPECT_NEAR(q.getY(), 0.5, EPSILON);
    EXPECT_NEAR(q.getZ(), 0.5, EPSILON);
}

TEST(QuaternionConstruction, IdentityQuaternion) {
    // Test identity quaternion factory method
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(identity.getW(), 1.0, EPSILON);
    EXPECT_NEAR(identity.getX(), 0.0, EPSILON);
    EXPECT_NEAR(identity.getY(), 0.0, EPSILON);
    EXPECT_NEAR(identity.getZ(), 0.0, EPSILON);

    // Identity quaternion should have norm = 1
    EXPECT_NEAR(identity.norm(), 1.0, EPSILON);
}

TEST(QuaternionConstruction, SetMethod) {
    // Test set method for component modification
    Quaternion q;
    q.set(0.6, 0.8, 0.0, 0.0);

    EXPECT_NEAR(q.getW(), 0.6, EPSILON);
    EXPECT_NEAR(q.getX(), 0.8, EPSILON);
    EXPECT_NEAR(q.getY(), 0.0, EPSILON);
    EXPECT_NEAR(q.getZ(), 0.0, EPSILON);
}

// ============================================================================
// QuaternionFactory Tests
// ============================================================================

TEST(QuaternionFactory, FromAxisAngleZAxis) {
    // Create quaternion for 90-degree rotation around Z-axis
    Vector zAxis(0.0, 0.0, 1.0);
    double angle = M_PI / 2.0;  // 90 degrees

    Quaternion q = Quaternion::fromAxisAngle(zAxis, angle);

    // For axis-angle: q = (cos(θ/2), sin(θ/2)*axis)
    double halfAngle = angle / 2.0;
    EXPECT_NEAR(q.getW(), cos(halfAngle), EPSILON);
    EXPECT_NEAR(q.getX(), 0.0, EPSILON);
    EXPECT_NEAR(q.getY(), 0.0, EPSILON);
    EXPECT_NEAR(q.getZ(), sin(halfAngle), EPSILON);

    // Should be a unit quaternion
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
}

TEST(QuaternionFactory, FromAxisAngleXAxis) {
    // Create quaternion for 180-degree rotation around X-axis
    Vector xAxis(1.0, 0.0, 0.0);
    double angle = M_PI;  // 180 degrees

    Quaternion q = Quaternion::fromAxisAngle(xAxis, angle);

    double halfAngle = angle / 2.0;
    EXPECT_NEAR(q.getW(), cos(halfAngle), EPSILON);
    EXPECT_NEAR(q.getX(), sin(halfAngle), EPSILON);
    EXPECT_NEAR(q.getY(), 0.0, EPSILON);
    EXPECT_NEAR(q.getZ(), 0.0, EPSILON);

    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
}

TEST(QuaternionFactory, FromAxisAngleArbitrary) {
    // Create quaternion for rotation around arbitrary axis
    Vector axis(1.0, 1.0, 1.0);
    double angle = M_PI / 3.0;  // 60 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // Should be a unit quaternion
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Verify axis normalization happened correctly
    double halfAngle = angle / 2.0;
    double axisNorm = sqrt(3.0);
    EXPECT_NEAR(q.getW(), cos(halfAngle), EPSILON);
    EXPECT_NEAR(q.getX(), sin(halfAngle) / axisNorm, EPSILON);
    EXPECT_NEAR(q.getY(), sin(halfAngle) / axisNorm, EPSILON);
    EXPECT_NEAR(q.getZ(), sin(halfAngle) / axisNorm, EPSILON);
}

TEST(QuaternionFactory, FromEulerAngles) {
    // Test Euler angle conversion (roll, pitch, yaw)
    double roll = M_PI / 4.0;   // 45 degrees
    double pitch = M_PI / 6.0;  // 30 degrees
    double yaw = M_PI / 3.0;    // 60 degrees

    Quaternion q = Quaternion::fromEuler(roll, pitch, yaw);

    // Should be a unit quaternion
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Test specific case: zero rotation
    Quaternion qZero = Quaternion::fromEuler(0.0, 0.0, 0.0);
    ExpectQuaternionNear(qZero, Quaternion::identity());
}

// ============================================================================
// QuaternionArithmetic Tests
// ============================================================================

TEST(QuaternionArithmetic, Addition) {
    // Test quaternion addition
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion q2(5.0, 6.0, 7.0, 8.0);

    Quaternion result = q1 + q2;
    Quaternion expected(6.0, 8.0, 10.0, 12.0);

    ExpectQuaternionNear(result, expected);
}

TEST(QuaternionArithmetic, Subtraction) {
    // Test quaternion subtraction
    Quaternion q1(10.0, 9.0, 8.0, 7.0);
    Quaternion q2(1.0, 2.0, 3.0, 4.0);

    Quaternion result = q1 - q2;
    Quaternion expected(9.0, 7.0, 5.0, 3.0);

    ExpectQuaternionNear(result, expected);
}

TEST(QuaternionArithmetic, ScalarMultiplication) {
    // Test scalar multiplication
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    double scalar = 2.5;

    Quaternion result = q * scalar;
    Quaternion expected(2.5, 5.0, 7.5, 10.0);

    ExpectQuaternionNear(result, expected);
}

TEST(QuaternionArithmetic, HamiltonProduct) {
    // Test quaternion multiplication (Hamilton product)
    // q1 * q2 where q1 = (1, i, 0, 0) and q2 = (1, 0, j, 0)
    Quaternion q1(1.0, 1.0, 0.0, 0.0);
    Quaternion q2(1.0, 0.0, 1.0, 0.0);

    Quaternion result = q1 * q2;

    // Hamilton product: (w1w2 - x1x2 - y1y2 - z1z2,
    //                    w1x2 + x1w2 + y1z2 - z1y2,
    //                    w1y2 - x1z2 + y1w2 + z1x2,
    //                    w1z2 + x1y2 - y1x2 + z1w2)
    // (1*1 - 1*0 - 0*1 - 0*0) = 1
    // (1*0 + 1*1 + 0*0 - 0*1) = 1
    // (1*1 - 1*0 + 0*1 + 0*0) = 1
    // (1*0 + 1*1 - 0*0 + 0*1) = 1
    Quaternion expected(1.0, 1.0, 1.0, 1.0);

    ExpectQuaternionNear(result, expected);
}

TEST(QuaternionArithmetic, IdentityMultiplication) {
    // Multiplying by identity should return same quaternion
    Quaternion q(0.5, 0.5, 0.5, 0.5);
    Quaternion identity = Quaternion::identity();

    Quaternion result1 = q * identity;
    Quaternion result2 = identity * q;

    ExpectQuaternionNear(result1, q);
    ExpectQuaternionNear(result2, q);
}

// ============================================================================
// QuaternionProperties Tests
// ============================================================================

TEST(QuaternionProperties, Norm) {
    // Test norm calculation
    Quaternion q(1.0, 0.0, 0.0, 0.0);
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    Quaternion q2(0.0, 1.0, 0.0, 0.0);
    EXPECT_NEAR(q2.norm(), 1.0, EPSILON);

    Quaternion q3(1.0, 1.0, 1.0, 1.0);
    EXPECT_NEAR(q3.norm(), 2.0, EPSILON);

    Quaternion q4(3.0, 4.0, 0.0, 0.0);
    EXPECT_NEAR(q4.norm(), 5.0, EPSILON);
}

TEST(QuaternionProperties, Length) {
    // Length should be same as norm
    Quaternion q(3.0, 4.0, 0.0, 0.0);
    EXPECT_NEAR(q.length(), q.norm(), EPSILON);
}

TEST(QuaternionProperties, DotProduct) {
    // Test dot product
    Quaternion q1(1.0, 2.0, 3.0, 4.0);
    Quaternion q2(5.0, 6.0, 7.0, 8.0);

    double dot = q1.dot(q2);
    // 1*5 + 2*6 + 3*7 + 4*8 = 5 + 12 + 21 + 32 = 70
    EXPECT_NEAR(dot, 70.0, EPSILON);
}

TEST(QuaternionProperties, Normalize) {
    // Test normalization
    Quaternion q(2.0, 0.0, 0.0, 0.0);
    Quaternion normalized = q.normalize();

    EXPECT_NEAR(normalized.norm(), 1.0, EPSILON);
    EXPECT_NEAR(normalized.getW(), 1.0, EPSILON);
    EXPECT_NEAR(normalized.getX(), 0.0, EPSILON);

    // Test with arbitrary quaternion
    Quaternion q2(1.0, 2.0, 3.0, 4.0);
    Quaternion normalized2 = q2.normalize();

    EXPECT_NEAR(normalized2.norm(), 1.0, EPSILON);

    // Original quaternion should be unchanged
    EXPECT_NEAR(q2.getW(), 1.0, EPSILON);
}

TEST(QuaternionProperties, Conjugate) {
    // Test conjugate: q* = (w, -x, -y, -z)
    Quaternion q(1.0, 2.0, 3.0, 4.0);
    Quaternion conj = q.conjugate();

    EXPECT_NEAR(conj.getW(), 1.0, EPSILON);
    EXPECT_NEAR(conj.getX(), -2.0, EPSILON);
    EXPECT_NEAR(conj.getY(), -3.0, EPSILON);
    EXPECT_NEAR(conj.getZ(), -4.0, EPSILON);
}

TEST(QuaternionProperties, Inverse) {
    // Test inverse: q^-1 = q* / |q|^2
    Quaternion q(1.0, 0.0, 0.0, 0.0);
    Quaternion inv = q.inverse();

    // For unit quaternion, inverse equals conjugate
    ExpectQuaternionNear(inv, q.conjugate());

    // Test q * q^-1 = identity
    Quaternion product = q * inv;
    ExpectQuaternionNear(product, Quaternion::identity());
}

TEST(QuaternionProperties, InverseNonUnit) {
    // Test inverse of non-unit quaternion
    Quaternion q(2.0, 0.0, 0.0, 0.0);
    Quaternion inv = q.inverse();

    // q * q^-1 should equal identity
    Quaternion product = q * inv;
    ExpectQuaternionNear(product, Quaternion::identity());
}

TEST(QuaternionProperties, UnitQuaternionProperty) {
    // Unit quaternions created from axis-angle should have norm = 1
    Vector axis(1.0, 2.0, 3.0);
    double angle = M_PI / 4.0;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
}

// ============================================================================
// QuaternionRotation Tests
// ============================================================================

TEST(QuaternionRotation, Rotate90DegreesAroundZ) {
    // Rotate vector (1, 0, 0) by 90 degrees around Z-axis
    // Should result in (0, 1, 0)
    Vector axis(0.0, 0.0, 1.0);
    double angle = M_PI / 2.0;  // 90 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Vector v(1.0, 0.0, 0.0);
    Vector rotated = q.rotate(v);

    EXPECT_NEAR(rotated.getX(), 0.0, EPSILON);
    EXPECT_NEAR(rotated.getY(), 1.0, EPSILON);
    EXPECT_NEAR(rotated.getZ(), 0.0, EPSILON);
}

TEST(QuaternionRotation, Rotate180DegreesAroundX) {
    // Rotate vector (0, 1, 0) by 180 degrees around X-axis
    // Should result in (0, -1, 0)
    Vector axis(1.0, 0.0, 0.0);
    double angle = M_PI;  // 180 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Vector v(0.0, 1.0, 0.0);
    Vector rotated = q.rotate(v);

    EXPECT_NEAR(rotated.getX(), 0.0, EPSILON);
    EXPECT_NEAR(rotated.getY(), -1.0, EPSILON);
    EXPECT_NEAR(rotated.getZ(), 0.0, EPSILON);
}

TEST(QuaternionRotation, RotateArbitraryVector) {
    // Rotate vector (1, 1, 1) by 120 degrees around (1, 1, 1)
    // Vector should remain unchanged (rotating around itself)
    Vector axis(1.0, 1.0, 1.0);
    double angle = 2.0 * M_PI / 3.0;  // 120 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Vector v(1.0, 1.0, 1.0);
    Vector rotated = q.rotate(v);

    // Vector parallel to rotation axis should be unchanged
    ExpectVectorNear(rotated, v);
}

TEST(QuaternionRotation, IdentityRotation) {
    // Identity quaternion should not change the vector
    Quaternion identity = Quaternion::identity();
    Vector v(3.5, -2.1, 4.7);
    Vector rotated = identity.rotate(v);

    ExpectVectorNear(rotated, v);
}

TEST(QuaternionRotation, RotationPreservesLength) {
    // Rotation should preserve vector length
    Vector axis(0.0, 1.0, 0.0);
    double angle = M_PI / 3.0;  // 60 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Vector v(3.0, 4.0, 5.0);
    Vector rotated = q.rotate(v);

    double originalLength = v.getLengthVector();
    double rotatedLength = rotated.getLengthVector();

    EXPECT_NEAR(rotatedLength, originalLength, EPSILON);
}

// ============================================================================
// QuaternionConversion Tests
// ============================================================================

TEST(QuaternionConversion, ToRotationMatrix) {
    // Test conversion to rotation matrix
    Vector axis(0.0, 0.0, 1.0);
    double angle = M_PI / 2.0;  // 90 degrees around Z

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Matrix3x3 rotMat = q.toRotationMatrix();

    // For 90-degree rotation around Z:
    // [cos(90) -sin(90) 0]   [0 -1 0]
    // [sin(90)  cos(90) 0] = [1  0 0]
    // [0        0       1]   [0  0 1]

    EXPECT_NEAR(rotMat.get(0, 0), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(0, 1), -1.0, EPSILON);
    EXPECT_NEAR(rotMat.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(1, 0), 1.0, EPSILON);
    EXPECT_NEAR(rotMat.get(1, 1), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(1, 2), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(2, 0), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(2, 1), 0.0, EPSILON);
    EXPECT_NEAR(rotMat.get(2, 2), 1.0, EPSILON);
}

TEST(QuaternionConversion, RotationMatrixIdentity) {
    // Identity quaternion should produce identity matrix
    Quaternion identity = Quaternion::identity();
    Matrix3x3 rotMat = identity.toRotationMatrix();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                EXPECT_NEAR(rotMat.get(i, j), 1.0, EPSILON);
            } else {
                EXPECT_NEAR(rotMat.get(i, j), 0.0, EPSILON);
            }
        }
    }
}

TEST(QuaternionConversion, ToAxisAngle) {
    // Test conversion to axis-angle representation
    Vector originalAxis(0.0, 1.0, 0.0);
    double originalAngle = M_PI / 3.0;  // 60 degrees

    Quaternion q = Quaternion::fromAxisAngle(originalAxis, originalAngle);

    Vector convertedAxis;
    double convertedAngle;
    q.toAxisAngle(convertedAxis, convertedAngle);

    // Check angle
    EXPECT_NEAR(convertedAngle, originalAngle, EPSILON);

    // Check axis (should be normalized)
    ExpectVectorNear(convertedAxis, originalAxis);
}

TEST(QuaternionConversion, RoundTripAxisAngle) {
    // Test axis-angle -> quaternion -> axis-angle round trip
    Vector originalAxis(1.0, 2.0, 3.0);
    double originalAngle = M_PI / 4.0;

    Quaternion q = Quaternion::fromAxisAngle(originalAxis, originalAngle);

    Vector convertedAxis;
    double convertedAngle;
    q.toAxisAngle(convertedAxis, convertedAngle);

    // Normalize original axis for comparison
    Vector normalizedOriginalAxis = originalAxis;
    normalizedOriginalAxis.setVectorLength(1.0);

    EXPECT_NEAR(convertedAngle, originalAngle, EPSILON);
    ExpectVectorNear(convertedAxis, normalizedOriginalAxis);
}

TEST(QuaternionConversion, MatrixVectorConsistency) {
    // Verify that quaternion rotation and matrix rotation give same result
    Vector axis(1.0, 1.0, 0.0);
    double angle = M_PI / 6.0;  // 30 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Matrix3x3 rotMat = q.toRotationMatrix();

    Vector v(1.0, 2.0, 3.0);
    Vector rotatedByQuaternion = q.rotate(v);
    Vector rotatedByMatrix = rotMat * v;

    ExpectVectorNear(rotatedByQuaternion, rotatedByMatrix);
}

// ============================================================================
// QuaternionIntegration Tests
// ============================================================================

TEST(QuaternionIntegration, IntegrateAngularVelocity) {
    // Test integration of angular velocity
    Quaternion q = Quaternion::identity();
    Vector angularVelocity(0.0, 0.0, 1.0);  // Rotating around Z-axis
    double dt = 0.1;

    Quaternion integrated = q.integrate(angularVelocity, dt);

    // Result should still be a unit quaternion
    EXPECT_NEAR(integrated.norm(), 1.0, EPSILON);

    // After integration, quaternion should represent a small rotation
    EXPECT_GT(integrated.getZ(), 0.0);  // Z component should be positive
}

TEST(QuaternionIntegration, IntegrationPreservesNormalization) {
    // Test that integration maintains unit quaternion property
    Quaternion q = Quaternion::fromAxisAngle(Vector(1.0, 0.0, 0.0), M_PI / 4.0);
    Vector angularVelocity(0.0, 1.0, 0.0);
    double dt = 0.05;

    Quaternion integrated = q.integrate(angularVelocity, dt);

    // Should remain normalized
    EXPECT_NEAR(integrated.norm(), 1.0, EPSILON);
}

TEST(QuaternionIntegration, MultipleIntegrationSteps) {
    // Test multiple integration steps
    Quaternion q = Quaternion::identity();
    Vector angularVelocity(0.0, 0.0, M_PI);  // Fast rotation around Z
    double dt = 0.01;

    // Integrate multiple times
    for (int i = 0; i < 100; i++) {
        q = q.integrate(angularVelocity, dt);
        // Should remain normalized after each step
        EXPECT_NEAR(q.norm(), 1.0, EPSILON);
    }
}

TEST(QuaternionIntegration, ZeroAngularVelocity) {
    // Zero angular velocity should result in no change
    Quaternion q = Quaternion::fromAxisAngle(Vector(1.0, 0.0, 0.0), M_PI / 4.0);
    Vector zeroVelocity(0.0, 0.0, 0.0);
    double dt = 0.1;

    Quaternion integrated = q.integrate(zeroVelocity, dt);

    // Should be approximately the same (may have small numerical differences)
    ExpectQuaternionNear(integrated, q);
}

// ============================================================================
// QuaternionEdgeCases Tests
// ============================================================================

TEST(QuaternionEdgeCases, ZeroQuaternion) {
    // Test zero quaternion behavior
    Quaternion zero(0.0, 0.0, 0.0, 0.0);

    EXPECT_NEAR(zero.norm(), 0.0, EPSILON);

    // Normalizing zero quaternion should return identity
    Quaternion normalized = zero.normalize();
    ExpectQuaternionNear(normalized, Quaternion::identity());

    // Inverse of zero quaternion should return identity (safe fallback)
    Quaternion inverse = zero.inverse();
    ExpectQuaternionNear(inverse, Quaternion::identity());
}

TEST(QuaternionEdgeCases, DenormalizedQuaternion) {
    // Test quaternion that's not normalized
    Quaternion q(2.0, 0.0, 0.0, 0.0);

    EXPECT_NEAR(q.norm(), 2.0, EPSILON);

    Quaternion normalized = q.normalize();
    EXPECT_NEAR(normalized.norm(), 1.0, EPSILON);
    EXPECT_NEAR(normalized.getW(), 1.0, EPSILON);
}

TEST(QuaternionEdgeCases, NearZeroNorm) {
    // Test quaternion with very small norm
    Quaternion q(1e-12, 1e-12, 1e-12, 1e-12);

    // Should have very small norm
    EXPECT_LT(q.norm(), 1e-10);

    // Normalizing should return identity (safe fallback)
    Quaternion normalized = q.normalize();
    ExpectQuaternionNear(normalized, Quaternion::identity());
}

TEST(QuaternionEdgeCases, LargeComponents) {
    // Test quaternion with large components
    Quaternion q(1e6, 1e6, 1e6, 1e6);

    double expectedNorm = 2e6;  // sqrt(4 * 1e12)
    EXPECT_NEAR(q.norm(), expectedNorm, 1.0);

    Quaternion normalized = q.normalize();
    EXPECT_NEAR(normalized.norm(), 1.0, EPSILON);
}

TEST(QuaternionEdgeCases, NegativeComponents) {
    // Test quaternion with negative components
    Quaternion q(-0.5, -0.5, -0.5, -0.5);

    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Negative quaternion represents same rotation as positive
    // q and -q represent the same rotation
    Quaternion qPos(0.5, 0.5, 0.5, 0.5);
    Vector v(1.0, 0.0, 0.0);

    Vector rotated1 = q.rotate(v);
    Vector rotated2 = qPos.rotate(v);

    // Results should be negatives or same rotation result
    // (quaternion double cover)
    EXPECT_NEAR(rotated1.getLengthVector(), rotated2.getLengthVector(), EPSILON);
}

TEST(QuaternionEdgeCases, VerySmallAngle) {
    // Test rotation with very small angle
    Vector axis(0.0, 0.0, 1.0);
    double angle = 1e-8;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // Should still be normalized
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Should be very close to identity
    double halfAngle = angle / 2.0;
    EXPECT_NEAR(q.getW(), cos(halfAngle), EPSILON);
    EXPECT_NEAR(q.getZ(), sin(halfAngle), EPSILON);
}

TEST(QuaternionEdgeCases, FullRotation) {
    // Test 360-degree rotation (should be identity)
    Vector axis(0.0, 1.0, 0.0);
    double angle = 2.0 * M_PI;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // Should rotate vector back to original position
    Vector v(1.0, 0.0, 0.0);
    Vector rotated = q.rotate(v);

    ExpectVectorNear(rotated, v);
}

// ============================================================================
// Mathematical Identity Tests
// ============================================================================

TEST(QuaternionIdentities, InverseIdentity) {
    // Test q * q^-1 = identity
    Vector axis(1.0, 2.0, 3.0);
    double angle = M_PI / 5.0;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Quaternion qInv = q.inverse();
    Quaternion product = q * qInv;

    ExpectQuaternionNear(product, Quaternion::identity());
}

TEST(QuaternionIdentities, ConjugateProperty) {
    // For unit quaternion: q^-1 = q*
    Vector axis(1.0, 0.0, 0.0);
    double angle = M_PI / 4.0;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);
    Quaternion qConj = q.conjugate();
    Quaternion qInv = q.inverse();

    // Since q is unit quaternion, inverse should equal conjugate
    ExpectQuaternionNear(qInv, qConj);
}

TEST(QuaternionIdentities, NormPreservation) {
    // |q1 * q2| = |q1| * |q2|
    Quaternion q1(0.6, 0.8, 0.0, 0.0);
    Quaternion q2(0.6, 0.0, 0.8, 0.0);

    double norm1 = q1.norm();
    double norm2 = q2.norm();
    Quaternion product = q1 * q2;
    double normProduct = product.norm();

    EXPECT_NEAR(normProduct, norm1 * norm2, EPSILON);
}

TEST(QuaternionIdentities, RotationComposition) {
    // Rotating by q1 then q2 equals rotating by q2*q1
    Vector axis1(0.0, 0.0, 1.0);
    Vector axis2(0.0, 1.0, 0.0);
    double angle1 = M_PI / 4.0;
    double angle2 = M_PI / 3.0;

    Quaternion q1 = Quaternion::fromAxisAngle(axis1, angle1);
    Quaternion q2 = Quaternion::fromAxisAngle(axis2, angle2);

    Vector v(1.0, 0.0, 0.0);

    // Method 1: Rotate by q1, then by q2
    Vector step1 = q1.rotate(v);
    Vector result1 = q2.rotate(step1);

    // Method 2: Rotate by composed quaternion q2*q1
    Quaternion composed = q2 * q1;
    Vector result2 = composed.rotate(v);

    ExpectVectorNear(result1, result2);
}
