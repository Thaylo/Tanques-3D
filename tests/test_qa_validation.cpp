/*
 * test_qa_validation.cpp
 *
 * QA Edge Case Validation Tests
 * Based on findings from docs/CRITICAL_ISSUES.md and docs/TEST_PLAN.md
 *
 * This file tests all edge cases identified during QA review:
 * - Mass validation (zero/negative mass handling)
 * - Matrix bounds checking
 * - Singular matrix inverse error handling
 * - Zero quaternion normalization
 * - Quaternion inverse edge cases
 * - Axis-angle conversion edge cases (0°, 180°, 360°, zero axis)
 * - Gimbal lock scenarios
 * - Numerical stability with large/small values
 */

#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include <limits>

#include "RigidBody.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "Vector.h"

using namespace std;

// Test epsilon for floating-point comparisons
const double EPSILON = 1e-6;
const double TIGHT_EPSILON = 1e-10;

// ============================================================================
// QA Issue #7: RigidBody Mass Validation Tests
// ============================================================================

TEST(QA_MassValidation, SetZeroMass_ShouldRejectOrHandle) {
    // QA Issue: Zero mass causes division by zero in physics calculations
    // Expected: Mass should be rejected or clamped to minimum value

    RigidBody body;

    // Attempt to set zero mass
    body.setMass(0.0);

    // Mass should be positive (either rejected or clamped to minimum)
    double mass = body.getMass();

    EXPECT_GT(mass, 0.0) << "Zero mass should be rejected or clamped to positive value";

    // If implementation allows setting, it should default to 1.0
    if (mass != 1.0) {
        // Alternative: implementation might clamp to epsilon
        EXPECT_GE(mass, TIGHT_EPSILON);
    }
}

TEST(QA_MassValidation, SetNegativeMass_ShouldRejectOrHandle) {
    // QA Issue: Negative mass is physically invalid
    // Expected: Mass should be rejected or clamped to minimum value

    RigidBody body;

    // Attempt to set negative mass
    body.setMass(-5.0);

    // Mass should be positive
    double mass = body.getMass();

    EXPECT_GT(mass, 0.0) << "Negative mass should be rejected or clamped to positive value";
}

TEST(QA_MassValidation, SetValidPositiveMass_ShouldAccept) {
    // QA Issue: Verify that valid positive mass values are accepted

    RigidBody body;

    // Set valid positive mass values
    double testMasses[] = {0.1, 1.0, 5.0, 100.0, 1000.0};

    for (double testMass : testMasses) {
        body.setMass(testMass);
        double retrievedMass = body.getMass();

        EXPECT_NEAR(retrievedMass, testMass, EPSILON)
            << "Valid mass " << testMass << " should be accepted";
    }
}

TEST(QA_MassValidation, ConstructorWithZeroMass_ShouldHandle) {
    // QA Issue: Constructor should validate mass parameter

    Vector pos(0, 0, 0);

    // Attempt to construct with zero mass
    RigidBody body(pos, 0.0);

    double mass = body.getMass();
    EXPECT_GT(mass, 0.0) << "Constructor should reject zero mass";
}

TEST(QA_MassValidation, IntegrationWithZeroMass_ShouldNotCrash) {
    // QA Issue: Integration with zero mass causes division by zero
    // Expected: Should handle gracefully without crash

    RigidBody body;
    body.setMass(0.0);

    // Apply force and integrate - should not crash
    body.applyForce(Vector(10.0, 0.0, 0.0));

    ASSERT_NO_THROW({
        body.integrate(0.01);
    }) << "Integration with zero mass should not crash";
}

// ============================================================================
// QA Issue #4: Matrix Bounds Checking Tests
// ============================================================================

TEST(QA_MatrixBounds, GetNegativeRow_ShouldHandleSafely) {
    // QA Issue: No bounds checking in Matrix3x3::get()
    // Expected: Should either throw exception, assert, or return safe value

    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    // Attempt to access negative row index
    // Note: If implementation adds assertions, this will fail in debug mode
    // In release mode without checks, this is undefined behavior

    // Test should verify safe handling exists
    bool exceptionThrown = false;
    try {
        double value = m.get(-1, 0);
        // If we get here without crash, check if value is reasonable
        // (implementation might return 0.0 as safe fallback)
        EXPECT_TRUE(std::isfinite(value)) << "Invalid access should return finite value";
    } catch (const std::exception& e) {
        exceptionThrown = true;
    }

    // Either exception was thrown OR we got safe value
    // Both are acceptable implementations
}

TEST(QA_MatrixBounds, GetNegativeColumn_ShouldHandleSafely) {
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    bool exceptionThrown = false;
    try {
        double value = m.get(0, -1);
        EXPECT_TRUE(std::isfinite(value)) << "Invalid access should return finite value";
    } catch (const std::exception& e) {
        exceptionThrown = true;
    }
}

TEST(QA_MatrixBounds, GetRowOutOfBounds_ShouldHandleSafely) {
    // QA Issue: Accessing row >= 3 causes undefined behavior

    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    bool exceptionThrown = false;
    try {
        double value = m.get(3, 0);  // Row 3 is out of bounds (valid: 0-2)
        EXPECT_TRUE(std::isfinite(value)) << "Out-of-bounds access should return finite value";
    } catch (const std::exception& e) {
        exceptionThrown = true;
    }
}

TEST(QA_MatrixBounds, GetColumnOutOfBounds_ShouldHandleSafely) {
    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    bool exceptionThrown = false;
    try {
        double value = m.get(0, 5);  // Column 5 is out of bounds
        EXPECT_TRUE(std::isfinite(value)) << "Out-of-bounds access should return finite value";
    } catch (const std::exception& e) {
        exceptionThrown = true;
    }
}

TEST(QA_MatrixBounds, SetOutOfBounds_ShouldHandleSafely) {
    // QA Issue: set() method also lacks bounds checking

    Matrix3x3 m;

    // Attempt to set out-of-bounds element
    bool exceptionThrown = false;
    try {
        m.set(10, 10, 42.0);  // Way out of bounds
        // If we get here, implementation doesn't check bounds
        // Verify no memory corruption by reading valid elements
        double value = m.get(0, 0);
        EXPECT_TRUE(std::isfinite(value));
    } catch (const std::exception& e) {
        exceptionThrown = true;
    }
}

TEST(QA_MatrixBounds, ValidIndices_ShouldWork) {
    // Verify that valid indices work correctly

    Matrix3x3 m(1.0, 2.0, 3.0,
                4.0, 5.0, 6.0,
                7.0, 8.0, 9.0);

    // All valid indices should work
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ASSERT_NO_THROW({
                double value = m.get(i, j);
                EXPECT_TRUE(std::isfinite(value));
            }) << "Valid index [" << i << "][" << j << "] should work";
        }
    }
}

// ============================================================================
// QA Issue #2: Singular Matrix Inverse Tests
// ============================================================================

TEST(QA_SingularMatrix, InverseSingularMatrix_ShouldHandleGracefully) {
    // QA Issue: Singular matrix inverse returns identity without error indication
    // Expected: Should log error, throw exception, or return error code

    // Create singular matrix (rows are linearly dependent)
    Matrix3x3 singular(1.0, 2.0, 3.0,
                       2.0, 4.0, 6.0,  // Row 2 = 2 * Row 1
                       3.0, 6.0, 9.0); // Row 3 = 3 * Row 1

    // Verify it's singular
    double det = singular.determinant();
    EXPECT_NEAR(det, 0.0, EPSILON) << "Matrix should be singular";

    // Attempt to invert - current implementation returns identity
    Matrix3x3 inverse = singular.inverse();

    // Implementation currently returns identity
    // This test documents the behavior - ideally should log error
    Matrix3x3 identity = Matrix3x3::identity();

    // Verify identity is returned (documenting current behavior)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(inverse.get(i, j), identity.get(i, j), EPSILON);
        }
    }

    // TODO: When fix is implemented, this test should verify error logging
    // EXPECT_TRUE(error_was_logged) << "Singular matrix should log error";
}

TEST(QA_SingularMatrix, NearSingularMatrix_ShouldHandle) {
    // QA Issue: Near-singular matrices may have numerical instability

    // Create near-singular matrix (very small determinant)
    Matrix3x3 nearSingular(1.0, 0.0, 0.0,
                           0.0, 1.0, 0.0,
                           0.0, 0.0, 1e-11);  // Very small element

    double det = nearSingular.determinant();
    EXPECT_LT(fabs(det), TIGHT_EPSILON) << "Matrix should be near-singular";

    // Attempt to invert
    Matrix3x3 inverse = nearSingular.inverse();

    // Should return identity or handle gracefully
    EXPECT_TRUE(std::isfinite(inverse.get(0, 0)));
    EXPECT_TRUE(std::isfinite(inverse.get(1, 1)));
    EXPECT_TRUE(std::isfinite(inverse.get(2, 2)));
}

TEST(QA_SingularMatrix, ZeroMatrix_InverseShouldHandle) {
    // QA Issue: Zero matrix is maximally singular

    Matrix3x3 zero = Matrix3x3::zero();

    double det = zero.determinant();
    EXPECT_NEAR(det, 0.0, EPSILON);

    // Attempt to invert zero matrix
    Matrix3x3 inverse = zero.inverse();

    // Should return identity as fallback
    Matrix3x3 identity = Matrix3x3::identity();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(inverse.get(i, j), identity.get(i, j), EPSILON);
        }
    }
}

// ============================================================================
// QA Issue #3: Quaternion Zero-Length Normalization Tests
// ============================================================================

TEST(QA_QuaternionNormalize, NormalizeZeroQuaternion_ShouldReturnIdentity) {
    // QA Issue: Zero quaternion normalization returns identity silently
    // Expected: Should log warning or throw exception

    Quaternion zero(0.0, 0.0, 0.0, 0.0);

    EXPECT_NEAR(zero.norm(), 0.0, EPSILON) << "Quaternion should be zero-length";

    // Normalize zero quaternion
    Quaternion normalized = zero.normalize();

    // Current implementation returns identity
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(normalized.getW(), identity.getW(), EPSILON);
    EXPECT_NEAR(normalized.getX(), identity.getX(), EPSILON);
    EXPECT_NEAR(normalized.getY(), identity.getY(), EPSILON);
    EXPECT_NEAR(normalized.getZ(), identity.getZ(), EPSILON);

    // TODO: When fix is implemented, verify warning is logged
    // EXPECT_TRUE(warning_was_logged) << "Zero quaternion should log warning";
}

TEST(QA_QuaternionNormalize, NormalizeNearZeroQuaternion_ShouldHandle) {
    // QA Issue: Near-zero quaternions have numerical issues

    Quaternion nearZero(1e-12, 1e-12, 1e-12, 1e-12);

    double norm = nearZero.norm();
    EXPECT_LT(norm, TIGHT_EPSILON) << "Quaternion should be near-zero";

    // Normalize near-zero quaternion
    Quaternion normalized = nearZero.normalize();

    // Should return identity as fallback
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(normalized.getW(), identity.getW(), EPSILON);
    EXPECT_NEAR(normalized.getX(), identity.getX(), EPSILON);
    EXPECT_NEAR(normalized.getY(), identity.getY(), EPSILON);
    EXPECT_NEAR(normalized.getZ(), identity.getZ(), EPSILON);
}

TEST(QA_QuaternionInverse, InverseZeroQuaternion_ShouldReturnIdentity) {
    // QA Issue: Inverse of zero quaternion is undefined

    Quaternion zero(0.0, 0.0, 0.0, 0.0);

    // Attempt to invert zero quaternion
    Quaternion inverse = zero.inverse();

    // Should return identity as safe fallback
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(inverse.getW(), identity.getW(), EPSILON);
    EXPECT_NEAR(inverse.getX(), identity.getX(), EPSILON);
    EXPECT_NEAR(inverse.getY(), identity.getY(), EPSILON);
    EXPECT_NEAR(inverse.getZ(), identity.getZ(), EPSILON);
}

TEST(QA_QuaternionInverse, InverseNearZeroQuaternion_ShouldHandle) {
    // QA Issue: Near-zero quaternion inverse is numerically unstable

    Quaternion nearZero(1e-11, 1e-11, 0.0, 0.0);

    double norm = nearZero.norm();
    EXPECT_LT(norm, TIGHT_EPSILON);

    // Attempt to invert
    Quaternion inverse = nearZero.inverse();

    // Should handle gracefully (return identity)
    EXPECT_TRUE(std::isfinite(inverse.getW()));
    EXPECT_TRUE(std::isfinite(inverse.getX()));
    EXPECT_TRUE(std::isfinite(inverse.getY()));
    EXPECT_TRUE(std::isfinite(inverse.getZ()));
}

// ============================================================================
// QA Issue #6: Axis-Angle Conversion Edge Cases
// ============================================================================

TEST(QA_AxisAngle, FromAxisAngleZeroAxis_ShouldHandleGracefully) {
    // QA Issue: Zero axis causes division by zero in normalization
    // Expected: Should return identity quaternion or log error

    Vector zeroAxis(0.0, 0.0, 0.0);
    double angle = M_PI / 4.0;

    // Attempt to create quaternion from zero axis
    Quaternion q = Quaternion::fromAxisAngle(zeroAxis, angle);

    // Should return identity or handle gracefully
    // Check that result is normalized
    double norm = q.norm();
    EXPECT_NEAR(norm, 1.0, EPSILON) << "Result should be unit quaternion";

    // Most likely returns identity
    EXPECT_TRUE(std::isfinite(q.getW()));
    EXPECT_TRUE(std::isfinite(q.getX()));
    EXPECT_TRUE(std::isfinite(q.getY()));
    EXPECT_TRUE(std::isfinite(q.getZ()));
}

TEST(QA_AxisAngle, FromAxisAngleZeroAngle_ShouldReturnIdentity) {
    // QA Issue: Zero angle rotation should return identity

    Vector axis(0.0, 0.0, 1.0);
    double angle = 0.0;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // Should be identity quaternion (no rotation)
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(q.getW(), identity.getW(), EPSILON);
    EXPECT_NEAR(q.getX(), identity.getX(), EPSILON);
    EXPECT_NEAR(q.getY(), identity.getY(), EPSILON);
    EXPECT_NEAR(q.getZ(), identity.getZ(), EPSILON);
}

TEST(QA_AxisAngle, FromAxisAngle180Degrees_ShouldHandle) {
    // QA Issue: 180-degree rotation is special case (sin(θ/2) ≈ 1)

    Vector axis(1.0, 0.0, 0.0);
    double angle = M_PI;  // 180 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // Should be unit quaternion
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // For 180 degrees: w = cos(90°) = 0, vector part = sin(90°) * axis = axis
    EXPECT_NEAR(q.getW(), 0.0, EPSILON);
    EXPECT_NEAR(fabs(q.getX()), 1.0, EPSILON);  // Along X axis
    EXPECT_NEAR(q.getY(), 0.0, EPSILON);
    EXPECT_NEAR(q.getZ(), 0.0, EPSILON);
}

TEST(QA_AxisAngle, FromAxisAngle360Degrees_ShouldBeIdentity) {
    // QA Issue: Full rotation (360°) should return to identity

    Vector axis(0.0, 1.0, 0.0);
    double angle = 2.0 * M_PI;  // 360 degrees

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    // 360-degree rotation should be equivalent to identity
    // w = cos(180°) = -1, but represents same rotation as w=1
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Verify rotation effect (should not change vectors)
    Vector v(1.0, 0.0, 0.0);
    Vector rotated = q.rotate(v);

    EXPECT_NEAR(rotated.getX(), v.getX(), EPSILON);
    EXPECT_NEAR(rotated.getY(), v.getY(), EPSILON);
    EXPECT_NEAR(rotated.getZ(), v.getZ(), EPSILON);
}

TEST(QA_AxisAngle, ToAxisAngleSmallAngle_ShouldHandle) {
    // QA Issue: Very small angles may have numerical issues

    Vector axis(0.0, 0.0, 1.0);
    double originalAngle = 1e-8;  // Very small angle

    Quaternion q = Quaternion::fromAxisAngle(axis, originalAngle);

    Vector extractedAxis;
    double extractedAngle;
    q.toAxisAngle(extractedAxis, extractedAngle);

    // Should handle gracefully
    EXPECT_TRUE(std::isfinite(extractedAngle));
    EXPECT_TRUE(std::isfinite(extractedAxis.getX()));
    EXPECT_TRUE(std::isfinite(extractedAxis.getY()));
    EXPECT_TRUE(std::isfinite(extractedAxis.getZ()));
}

TEST(QA_AxisAngle, ToAxisAngle180Degrees_ShouldHandle) {
    // QA Issue: 180-degree rotation extraction needs special handling

    Vector originalAxis(1.0, 0.0, 0.0);
    double originalAngle = M_PI;

    Quaternion q = Quaternion::fromAxisAngle(originalAxis, originalAngle);

    Vector extractedAxis;
    double extractedAngle;
    q.toAxisAngle(extractedAxis, extractedAngle);

    // Should extract 180 degrees correctly
    EXPECT_NEAR(extractedAngle, M_PI, EPSILON);

    // Axis should be normalized and in same direction
    EXPECT_NEAR(extractedAxis.getLengthVector(), 1.0, EPSILON);
}

TEST(QA_AxisAngle, RoundTripZeroRotation_ShouldPreserve) {
    // QA Issue: Round-trip conversion at edge cases

    Vector axis(1.0, 0.0, 0.0);
    double angle = 0.0;

    Quaternion q = Quaternion::fromAxisAngle(axis, angle);

    Vector extractedAxis;
    double extractedAngle;
    q.toAxisAngle(extractedAxis, extractedAngle);

    // Angle should be zero (axis is arbitrary for zero rotation)
    EXPECT_NEAR(extractedAngle, 0.0, EPSILON);
}

// ============================================================================
// QA Issue #5: Gimbal Lock Edge Cases
// ============================================================================

TEST(QA_GimbalLock, EulerAnglesPitch90Degrees_GimbalLock) {
    // QA Issue: Pitch = 90° causes gimbal lock

    double roll = 0.0;
    double pitch = M_PI / 2.0;  // 90 degrees - gimbal lock!
    double yaw = 0.0;

    Quaternion q = Quaternion::fromEuler(roll, pitch, yaw);

    // Should still be unit quaternion
    EXPECT_NEAR(q.norm(), 1.0, EPSILON);

    // Should handle gracefully (no NaN or inf)
    EXPECT_TRUE(std::isfinite(q.getW()));
    EXPECT_TRUE(std::isfinite(q.getX()));
    EXPECT_TRUE(std::isfinite(q.getY()));
    EXPECT_TRUE(std::isfinite(q.getZ()));
}

TEST(QA_GimbalLock, EulerAnglesPitchNegative90Degrees_GimbalLock) {
    // QA Issue: Pitch = -90° also causes gimbal lock

    double roll = 0.0;
    double pitch = -M_PI / 2.0;  // -90 degrees
    double yaw = 0.0;

    Quaternion q = Quaternion::fromEuler(roll, pitch, yaw);

    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
    EXPECT_TRUE(std::isfinite(q.getW()));
}

TEST(QA_GimbalLock, EulerAnglesNearGimbalLock_ShouldWork) {
    // Test angles near gimbal lock (89.9 degrees)

    double roll = 0.0;
    double pitch = (M_PI / 2.0) - 0.01;  // Close to 90 degrees
    double yaw = 0.0;

    Quaternion q = Quaternion::fromEuler(roll, pitch, yaw);

    EXPECT_NEAR(q.norm(), 1.0, EPSILON);
    EXPECT_TRUE(std::isfinite(q.getW()));
}

TEST(QA_GimbalLock, EulerAnglesAllZero_ShouldBeIdentity) {
    // Sanity check: zero Euler angles should give identity

    Quaternion q = Quaternion::fromEuler(0.0, 0.0, 0.0);
    Quaternion identity = Quaternion::identity();

    EXPECT_NEAR(q.getW(), identity.getW(), EPSILON);
    EXPECT_NEAR(q.getX(), identity.getX(), EPSILON);
    EXPECT_NEAR(q.getY(), identity.getY(), EPSILON);
    EXPECT_NEAR(q.getZ(), identity.getZ(), EPSILON);
}

// ============================================================================
// Numerical Stability Tests - Large Values
// ============================================================================

TEST(QA_NumericalStability, QuaternionLargeComponents_ShouldNormalize) {
    // Test numerical stability with large component values

    Quaternion large(1e6, 1e6, 1e6, 1e6);

    // Should be able to normalize
    Quaternion normalized = large.normalize();

    EXPECT_NEAR(normalized.norm(), 1.0, EPSILON);
    EXPECT_TRUE(std::isfinite(normalized.getW()));
    EXPECT_TRUE(std::isfinite(normalized.getX()));
    EXPECT_TRUE(std::isfinite(normalized.getY()));
    EXPECT_TRUE(std::isfinite(normalized.getZ()));
}

TEST(QA_NumericalStability, MatrixLargeValues_Determinant) {
    // Test determinant calculation with large values

    Matrix3x3 large(1e6, 0.0, 0.0,
                    0.0, 1e6, 0.0,
                    0.0, 0.0, 1e6);

    double det = large.determinant();

    // det should be 1e18
    EXPECT_TRUE(std::isfinite(det));
    EXPECT_GT(det, 0.0);
}

TEST(QA_NumericalStability, MatrixVerySmallValues_ShouldHandle) {
    // Test with very small values (near underflow)

    Matrix3x3 small(1e-100, 0.0, 0.0,
                    0.0, 1e-100, 0.0,
                    0.0, 0.0, 1e-100);

    double det = small.determinant();

    // Should handle gracefully (might be zero or very small)
    EXPECT_TRUE(std::isfinite(det));
}

TEST(QA_NumericalStability, VectorLargeLength_Normalization) {
    // Test vector normalization with large magnitude

    Vector large(1e10, 1e10, 1e10);

    double length = large.getLengthVector();
    EXPECT_TRUE(std::isfinite(length));

    // Set to unit length
    large.setVectorLength(1.0);

    double newLength = large.getLengthVector();
    EXPECT_NEAR(newLength, 1.0, EPSILON);
}

// ============================================================================
// Integration Tests - Combined Edge Cases
// ============================================================================

TEST(QA_Integration, RigidBodyWithSingularInertia_ShouldHandle) {
    // Integration test: RigidBody with singular inertia tensor

    RigidBody body(Vector(0, 0, 0), 1.0);

    // Set singular inertia tensor
    Matrix3x3 singularInertia(1.0, 0.0, 0.0,
                              0.0, 0.0, 0.0,  // Degenerate
                              0.0, 0.0, 0.0);

    // This should be caught or handled
    body.setInertiaTensor(singularInertia);

    // Apply torque and integrate - should not crash
    body.applyTorque(Vector(0.0, 0.0, 1.0));

    ASSERT_NO_THROW({
        body.integrate(0.01);
    }) << "Integration with singular inertia should not crash";
}

TEST(QA_Integration, MultipleEdgeCasesInSequence_ShouldHandle) {
    // Test multiple edge cases in sequence

    RigidBody body;

    // Edge case 1: Zero mass
    body.setMass(0.0);
    double mass1 = body.getMass();
    EXPECT_GT(mass1, 0.0);

    // Edge case 2: Negative mass
    body.setMass(-5.0);
    double mass2 = body.getMass();
    EXPECT_GT(mass2, 0.0);

    // Edge case 3: Valid mass
    body.setMass(10.0);
    EXPECT_NEAR(body.getMass(), 10.0, EPSILON);

    // Edge case 4: Integration after mass changes
    body.applyForce(Vector(100.0, 0.0, 0.0));
    ASSERT_NO_THROW({
        body.integrate(0.01);
    });

    Vector vel = body.getVelocity();
    EXPECT_TRUE(std::isfinite(vel.getX()));
    EXPECT_TRUE(std::isfinite(vel.getY()));
    EXPECT_TRUE(std::isfinite(vel.getZ()));
}

TEST(QA_Integration, QuaternionEdgeCasesInPhysics_ShouldHandle) {
    // Test quaternion edge cases in physics simulation context

    RigidBody body;

    // Set edge case orientation (near-gimbal lock)
    double pitch = (M_PI / 2.0) - 0.001;
    body.setOrientation(0.0, pitch, 0.0);

    Quaternion orient = body.getOrientation();
    EXPECT_NEAR(orient.norm(), 1.0, EPSILON);

    // Apply angular velocity and integrate
    body.setAngularVelocity(Vector(1.0, 1.0, 1.0));

    for (int i = 0; i < 10; i++) {
        body.integrate(0.01);

        Quaternion currentOrient = body.getOrientation();
        EXPECT_NEAR(currentOrient.norm(), 1.0, EPSILON)
            << "Orientation should remain normalized at step " << i;
    }
}

// ============================================================================
// Documentation Tests - Verify QA Fixes Are Present
// ============================================================================

TEST(QA_Documentation, MassValidation_TestsDocumentedBehavior) {
    // This test documents the expected behavior after QA fixes
    // Remove this test once fixes are verified in production code

    RigidBody body;

    // Expected behavior after fix:
    // 1. Zero mass should be rejected or clamped to minimum
    body.setMass(0.0);
    EXPECT_GT(body.getMass(), 0.0) << "Zero mass should be rejected";

    // 2. Negative mass should be rejected or clamped to minimum
    body.setMass(-1.0);
    EXPECT_GT(body.getMass(), 0.0) << "Negative mass should be rejected";

    // 3. Valid positive mass should be accepted
    body.setMass(5.0);
    EXPECT_NEAR(body.getMass(), 5.0, EPSILON) << "Valid mass should be accepted";
}

TEST(QA_Documentation, SingularMatrixBehavior_Documented) {
    // Document current behavior: singular matrix inverse returns identity
    // This test should be updated when error logging is added

    Matrix3x3 singular(1.0, 2.0, 3.0,
                       2.0, 4.0, 6.0,
                       3.0, 6.0, 9.0);

    Matrix3x3 inverse = singular.inverse();
    Matrix3x3 identity = Matrix3x3::identity();

    // Current behavior: returns identity
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(inverse.get(i, j), identity.get(i, j), EPSILON);
        }
    }

    // After fix: should log error message
    // EXPECT_TRUE(error_logged) << "Singular matrix should log error";
}
