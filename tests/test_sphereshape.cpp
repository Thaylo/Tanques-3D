/*
 * test_sphereshape.cpp
 *
 * Google Test unit tests for SphereShape class
 * Tests construction, inertia tensor calculations, AABB generation, and geometry
 */

#include <gtest/gtest.h>
#include <cmath>
#include "SphereShape.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include "test_helpers.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;

// Test fixture for SphereShape tests
class SphereShapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup if needed
    }

    void TearDown() override {
        // Common test cleanup if needed
    }
};

// ============================================================================
// SphereShapeConstruction Test Suite
// ============================================================================

TEST(SphereShapeConstruction, UnitSphereRadius) {
    SphereShape sphere(1.0);

    EXPECT_NEAR(sphere.getRadius(), 1.0, EPSILON);
}

TEST(SphereShapeConstruction, SmallSphereRadius) {
    SphereShape smallSphere(0.5);

    EXPECT_NEAR(smallSphere.getRadius(), 0.5, EPSILON);
}

TEST(SphereShapeConstruction, LargeSphereRadius) {
    SphereShape largeSphere(100.0);

    EXPECT_NEAR(largeSphere.getRadius(), 100.0, EPSILON);
}

TEST(SphereShapeConstruction, VerySmallSphereRadius) {
    SphereShape tinySphere(0.001);

    EXPECT_NEAR(tinySphere.getRadius(), 0.001, EPSILON);
}

TEST(SphereShapeConstruction, VeryLargeSphereRadius) {
    SphereShape hugeSphere(10000.0);

    EXPECT_NEAR(hugeSphere.getRadius(), 10000.0, EPSILON);
}

TEST(SphereShapeConstruction, DefaultColor) {
    SphereShape sphere(1.0);
    Vector color = sphere.getColor();

    // Default color should be white (1, 1, 1)
    EXPECT_NEAR(color.getX(), 1.0, EPSILON);
    EXPECT_NEAR(color.getY(), 1.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

TEST(SphereShapeConstruction, SetCustomColor) {
    SphereShape sphere(1.0);
    Vector blue(0.0, 0.0, 1.0);
    sphere.setColor(blue);

    Vector color = sphere.getColor();
    EXPECT_NEAR(color.getX(), 0.0, EPSILON);
    EXPECT_NEAR(color.getY(), 0.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

TEST(SphereShapeConstruction, InvalidRadiusZero) {
    // Radius must be positive, zero should throw
    EXPECT_THROW(SphereShape sphere(0.0), std::invalid_argument);
}

TEST(SphereShapeConstruction, InvalidRadiusNegative) {
    // Negative radius should throw
    EXPECT_THROW(SphereShape sphere(-1.0), std::invalid_argument);
}

// ============================================================================
// SphereShapeInertia Test Suite
// ============================================================================

TEST(SphereShapeInertia, UnitSphereInertia) {
    SphereShape sphere(1.0);
    double mass = 1.0;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // For a sphere with radius 1.0 and mass 1.0:
    // I = (2/5) * m * r^2 = (2/5) * 1.0 * 1.0 = 0.4

    double expected = (2.0 / 5.0) * mass * 1.0 * 1.0;

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);

    // Verify the expected value
    EXPECT_NEAR(expected, 0.4, EPSILON);
}

TEST(SphereShapeInertia, SphereDifferentRadius) {
    SphereShape sphere(2.0);
    double mass = 1.0;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // I = (2/5) * 1.0 * 2.0^2 = 0.4 * 4.0 = 1.6
    double expected = (2.0 / 5.0) * mass * 2.0 * 2.0;

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);

    EXPECT_NEAR(expected, 1.6, EPSILON);
}

TEST(SphereShapeInertia, SphereDifferentMass) {
    SphereShape sphere(1.0);
    double mass = 10.0;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // I = (2/5) * 10.0 * 1.0 = 4.0
    double expected = (2.0 / 5.0) * mass;

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);

    EXPECT_NEAR(expected, 4.0, EPSILON);
}

TEST(SphereShapeInertia, SphereHeavyMass) {
    SphereShape sphere(5.0);
    double mass = 100.0;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // I = (2/5) * 100.0 * 25.0 = 1000.0
    double expected = (2.0 / 5.0) * mass * 5.0 * 5.0;

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);

    EXPECT_NEAR(expected, 1000.0, EPSILON);
}

TEST(SphereShapeInertia, InertiaTensorIsSymmetric) {
    // For a sphere, all diagonal elements should be equal (isotropic)
    SphereShape sphere(3.0);
    double mass = 7.5;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // All diagonal elements should be equal
    EXPECT_NEAR(inertia.get(0, 0), inertia.get(1, 1), EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), inertia.get(2, 2), EPSILON);
    EXPECT_NEAR(inertia.get(0, 0), inertia.get(2, 2), EPSILON);
}

TEST(SphereShapeInertia, OffDiagonalElementsZero) {
    SphereShape sphere(2.0);
    double mass = 5.0;

    Matrix3x3 inertia = sphere.calculateInertiaTensor(mass);

    // Off-diagonal elements should be zero
    EXPECT_NEAR(inertia.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(1, 0), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(1, 2), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(2, 0), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(2, 1), 0.0, EPSILON);
}

TEST(SphereShapeInertia, MassScalingValidation) {
    SphereShape sphere(2.0);

    Matrix3x3 inertia1 = sphere.calculateInertiaTensor(1.0);
    Matrix3x3 inertia10 = sphere.calculateInertiaTensor(10.0);

    // Inertia should scale linearly with mass
    EXPECT_NEAR(inertia10.get(0, 0) / inertia1.get(0, 0), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(1, 1) / inertia1.get(1, 1), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(2, 2) / inertia1.get(2, 2), 10.0, EPSILON);
}

TEST(SphereShapeInertia, RadiusScalingValidation) {
    // Inertia should scale with r^2
    SphereShape sphere1(1.0);
    SphereShape sphere2(2.0);

    double mass = 1.0;
    Matrix3x3 inertia1 = sphere1.calculateInertiaTensor(mass);
    Matrix3x3 inertia2 = sphere2.calculateInertiaTensor(mass);

    // Inertia should scale with radius squared (2^2 = 4)
    EXPECT_NEAR(inertia2.get(0, 0) / inertia1.get(0, 0), 4.0, EPSILON);
    EXPECT_NEAR(inertia2.get(1, 1) / inertia1.get(1, 1), 4.0, EPSILON);
    EXPECT_NEAR(inertia2.get(2, 2) / inertia1.get(2, 2), 4.0, EPSILON);
}

// ============================================================================
// SphereShapeAABB Test Suite
// ============================================================================

TEST(SphereShapeAABB, CenterAtOrigin) {
    SphereShape sphere(2.0);
    Vector position(0.0, 0.0, 0.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = sphere.getAABB(position, orientation);

    // AABB should extend radius in all directions from origin
    EXPECT_NEAR(aabb.min.getX(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);
}

TEST(SphereShapeAABB, OffsetPosition) {
    SphereShape sphere(1.0);
    Vector position(10.0, 20.0, 30.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = sphere.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), 9.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), 19.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 29.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 11.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 21.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 31.0, EPSILON);
}

TEST(SphereShapeAABB, NegativePosition) {
    SphereShape sphere(5.0);
    Vector position(-100.0, -50.0, -25.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = sphere.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), -105.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -55.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -30.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), -95.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), -45.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), -20.0, EPSILON);
}

TEST(SphereShapeAABB, RotationDoesNotAffectAABB_45Degrees) {
    // Critical test: sphere AABB should be independent of rotation
    SphereShape sphere(3.0);
    Vector position(0.0, 0.0, 0.0);

    // 45 degree rotation around Z axis
    Quaternion rotation45 = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 4.0);

    AABB aabbRotated = sphere.getAABB(position, rotation45);
    AABB aabbIdentity = sphere.getAABB(position, Quaternion::identity());

    // Both AABBs should be identical (sphere is rotationally symmetric)
    EXPECT_NEAR(aabbRotated.min.getX(), aabbIdentity.min.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getY(), aabbIdentity.min.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getZ(), aabbIdentity.min.getZ(), EPSILON);

    EXPECT_NEAR(aabbRotated.max.getX(), aabbIdentity.max.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getY(), aabbIdentity.max.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getZ(), aabbIdentity.max.getZ(), EPSILON);
}

TEST(SphereShapeAABB, RotationDoesNotAffectAABB_90DegreesX) {
    SphereShape sphere(2.5);
    Vector position(5.0, 10.0, 15.0);

    Quaternion rotation90X = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 2.0);

    AABB aabbRotated = sphere.getAABB(position, rotation90X);
    AABB aabbIdentity = sphere.getAABB(position, Quaternion::identity());

    // AABBs should be identical regardless of rotation
    EXPECT_NEAR(aabbRotated.min.getX(), aabbIdentity.min.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getY(), aabbIdentity.min.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getZ(), aabbIdentity.min.getZ(), EPSILON);

    EXPECT_NEAR(aabbRotated.max.getX(), aabbIdentity.max.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getY(), aabbIdentity.max.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getZ(), aabbIdentity.max.getZ(), EPSILON);
}

TEST(SphereShapeAABB, RotationDoesNotAffectAABB_ArbitraryRotation) {
    SphereShape sphere(1.5);
    Vector position(0.0, 0.0, 0.0);

    // Arbitrary complex rotation
    Quaternion rotationX = Quaternion::fromAxisAngle(Vector(1, 0, 0), 0.7);
    Quaternion rotationY = Quaternion::fromAxisAngle(Vector(0, 1, 0), 1.2);
    Quaternion complexRotation = rotationY * rotationX;

    AABB aabbRotated = sphere.getAABB(position, complexRotation);
    AABB aabbIdentity = sphere.getAABB(position, Quaternion::identity());

    // AABBs should still be identical
    EXPECT_NEAR(aabbRotated.min.getX(), aabbIdentity.min.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getY(), aabbIdentity.min.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getZ(), aabbIdentity.min.getZ(), EPSILON);

    EXPECT_NEAR(aabbRotated.max.getX(), aabbIdentity.max.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getY(), aabbIdentity.max.getY(), EPSILON);
    EXPECT_NEAR(aabbRotated.max.getZ(), aabbIdentity.max.getZ(), EPSILON);
}

TEST(SphereShapeAABB, AABBIsCube) {
    // AABB of a sphere should form a cube
    SphereShape sphere(4.0);
    Vector position(0.0, 0.0, 0.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = sphere.getAABB(position, orientation);

    // Calculate dimensions
    double xDim = aabb.max.getX() - aabb.min.getX();
    double yDim = aabb.max.getY() - aabb.min.getY();
    double zDim = aabb.max.getZ() - aabb.min.getZ();

    // All dimensions should be equal (forming a cube)
    EXPECT_NEAR(xDim, yDim, EPSILON);
    EXPECT_NEAR(yDim, zDim, EPSILON);
    EXPECT_NEAR(xDim, zDim, EPSILON);

    // Each dimension should be 2 * radius
    EXPECT_NEAR(xDim, 2.0 * sphere.getRadius(), EPSILON);
}

TEST(SphereShapeAABB, AABBIntersection) {
    SphereShape sphere1(2.0);
    SphereShape sphere2(2.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(3.0, 0.0, 0.0);  // Centers 3 units apart, radii sum to 4

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = sphere1.getAABB(pos1, orient);
    AABB aabb2 = sphere2.getAABB(pos2, orient);

    // AABBs should intersect (overlap)
    EXPECT_TRUE(aabb1.intersects(aabb2));
}

TEST(SphereShapeAABB, AABBNoIntersection) {
    SphereShape sphere1(1.0);
    SphereShape sphere2(1.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(10.0, 0.0, 0.0);  // Far apart

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = sphere1.getAABB(pos1, orient);
    AABB aabb2 = sphere2.getAABB(pos2, orient);

    // AABBs should not intersect
    EXPECT_FALSE(aabb1.intersects(aabb2));
}

TEST(SphereShapeAABB, AABBBoundsCenteredCorrectly) {
    SphereShape sphere(3.5);
    Vector position(7.0, -3.0, 12.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = sphere.getAABB(position, orientation);

    // Calculate center of AABB
    Vector aabbCenter(
        (aabb.min.getX() + aabb.max.getX()) / 2.0,
        (aabb.min.getY() + aabb.max.getY()) / 2.0,
        (aabb.min.getZ() + aabb.max.getZ()) / 2.0
    );

    // AABB center should match sphere position
    EXPECT_NEAR(aabbCenter.getX(), position.getX(), EPSILON);
    EXPECT_NEAR(aabbCenter.getY(), position.getY(), EPSILON);
    EXPECT_NEAR(aabbCenter.getZ(), position.getZ(), EPSILON);
}

// ============================================================================
// SphereShapeGeometry Test Suite
// ============================================================================

TEST(SphereShapeGeometry, CharacteristicSizeIsDiameter) {
    SphereShape sphere(5.0);

    // Characteristic size should be diameter (2 * radius)
    EXPECT_NEAR(sphere.getCharacteristicSize(), 10.0, EPSILON);
}

TEST(SphereShapeGeometry, CharacteristicSizeUnitSphere) {
    SphereShape sphere(1.0);

    EXPECT_NEAR(sphere.getCharacteristicSize(), 2.0, EPSILON);
}

TEST(SphereShapeGeometry, CharacteristicSizeSmallSphere) {
    SphereShape sphere(0.25);

    EXPECT_NEAR(sphere.getCharacteristicSize(), 0.5, EPSILON);
}

TEST(SphereShapeGeometry, CharacteristicSizeLargeSphere) {
    SphereShape sphere(50.0);

    EXPECT_NEAR(sphere.getCharacteristicSize(), 100.0, EPSILON);
}

TEST(SphereShapeGeometry, RadiusGetterConsistency) {
    double testRadius = 7.5;
    SphereShape sphere(testRadius);

    // Getter should return the exact radius used in construction
    EXPECT_NEAR(sphere.getRadius(), testRadius, EPSILON);

    // Characteristic size should be exactly 2 * radius
    EXPECT_NEAR(sphere.getCharacteristicSize(), 2.0 * testRadius, EPSILON);
}

TEST(SphereShapeGeometry, VolumeCalculation) {
    // Optional: verify volume formula (4/3) * pi * r^3
    SphereShape sphere(3.0);

    double radius = sphere.getRadius();
    double expectedVolume = (4.0 / 3.0) * M_PI * radius * radius * radius;

    // This is informational - SphereShape doesn't have a getVolume() method,
    // but we can verify the radius is correct for volume calculations
    EXPECT_NEAR(expectedVolume, 113.097335529, 1e-6);
}

TEST(SphereShapeGeometry, SurfaceAreaCalculation) {
    // Optional: verify surface area formula 4 * pi * r^2
    SphereShape sphere(2.0);

    double radius = sphere.getRadius();
    double expectedSurfaceArea = 4.0 * M_PI * radius * radius;

    // Verify the radius is correct for surface area calculations
    EXPECT_NEAR(expectedSurfaceArea, 50.265482457, 1e-6);
}

// Main function
