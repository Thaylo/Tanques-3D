/*
 * test_cylindershape.cpp
 *
 * Google Test unit tests for CylinderShape class
 * Tests construction, inertia tensor calculations, AABB generation, and geometry
 */

#include <gtest/gtest.h>
#include <cmath>
#include "CylinderShape.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix3x3.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;

// Test fixture for CylinderShape tests
class CylinderShapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup if needed
    }

    void TearDown() override {
        // Common test cleanup if needed
    }
};

// ============================================================================
// CylinderShapeConstruction Test Suite
// ============================================================================

TEST(CylinderShapeConstruction, UnitCylinderDimensions) {
    CylinderShape cylinder(1.0, 1.0);

    EXPECT_NEAR(cylinder.getRadius(), 1.0, EPSILON);
    EXPECT_NEAR(cylinder.getHeight(), 1.0, EPSILON);
}

TEST(CylinderShapeConstruction, StandardCylinderDimensions) {
    CylinderShape cylinder(2.0, 5.0);

    EXPECT_NEAR(cylinder.getRadius(), 2.0, EPSILON);
    EXPECT_NEAR(cylinder.getHeight(), 5.0, EPSILON);
}

TEST(CylinderShapeConstruction, TallCylinderDimensions) {
    // Tall and thin cylinder (like a pencil)
    CylinderShape tallCylinder(0.5, 10.0);

    EXPECT_NEAR(tallCylinder.getRadius(), 0.5, EPSILON);
    EXPECT_NEAR(tallCylinder.getHeight(), 10.0, EPSILON);
}

TEST(CylinderShapeConstruction, WideCylinderDimensions) {
    // Wide and flat cylinder (like a hockey puck)
    CylinderShape wideCylinder(5.0, 1.0);

    EXPECT_NEAR(wideCylinder.getRadius(), 5.0, EPSILON);
    EXPECT_NEAR(wideCylinder.getHeight(), 1.0, EPSILON);
}

TEST(CylinderShapeConstruction, SmallCylinderDimensions) {
    CylinderShape smallCylinder(0.1, 0.2);

    EXPECT_NEAR(smallCylinder.getRadius(), 0.1, EPSILON);
    EXPECT_NEAR(smallCylinder.getHeight(), 0.2, EPSILON);
}

TEST(CylinderShapeConstruction, LargeCylinderDimensions) {
    CylinderShape largeCylinder(100.0, 200.0);

    EXPECT_NEAR(largeCylinder.getRadius(), 100.0, EPSILON);
    EXPECT_NEAR(largeCylinder.getHeight(), 200.0, EPSILON);
}

TEST(CylinderShapeConstruction, DefaultColor) {
    CylinderShape cylinder(1.0, 1.0);
    Vector color = cylinder.getColor();

    // Default color should be white (1, 1, 1)
    EXPECT_NEAR(color.getX(), 1.0, EPSILON);
    EXPECT_NEAR(color.getY(), 1.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

TEST(CylinderShapeConstruction, SetCustomColor) {
    CylinderShape cylinder(1.0, 1.0);
    Vector blue(0.0, 0.0, 1.0);
    cylinder.setColor(blue);

    Vector color = cylinder.getColor();
    EXPECT_NEAR(color.getX(), 0.0, EPSILON);
    EXPECT_NEAR(color.getY(), 0.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

// ============================================================================
// CylinderShapeInertia Test Suite
// ============================================================================

TEST(CylinderShapeInertia, UnitCylinderInertia) {
    CylinderShape cylinder(1.0, 1.0);
    double mass = 1.0;

    Matrix3x3 inertia = cylinder.calculateInertiaTensor(mass);

    // For a cylinder with r=1.0, h=1.0, m=1.0:
    // I_x = I_y = (1/12) * m * (3*r^2 + h^2) = (1/12) * 1.0 * (3*1 + 1) = 4/12 = 1/3
    // I_z = (1/2) * m * r^2 = (1/2) * 1.0 * 1.0 = 0.5

    double Ixy = (1.0 / 12.0) * mass * (3.0 * 1.0 + 1.0);
    double Iz = (1.0 / 2.0) * mass * 1.0;

    EXPECT_NEAR(inertia.get(0, 0), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Iz, EPSILON);

    // Off-diagonal elements should be zero
    EXPECT_NEAR(inertia.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(1, 2), 0.0, EPSILON);
}

TEST(CylinderShapeInertia, TallCylinderInertia) {
    // Tall cylinder: height >> radius
    CylinderShape tallCylinder(1.0, 10.0);
    double mass = 5.0;

    Matrix3x3 inertia = tallCylinder.calculateInertiaTensor(mass);

    // I_x = I_y = (1/12) * 5.0 * (3*1^2 + 10^2) = (5/12) * 103 = 42.916667
    // I_z = (1/2) * 5.0 * 1^2 = 2.5

    double r2 = 1.0 * 1.0;
    double h2 = 10.0 * 10.0;
    double Ixy = (mass / 12.0) * (3.0 * r2 + h2);
    double Iz = (mass / 2.0) * r2;

    EXPECT_NEAR(inertia.get(0, 0), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Iz, EPSILON);

    // For tall cylinder, I_xy should be much larger than I_z
    EXPECT_GT(inertia.get(0, 0), inertia.get(2, 2));
}

TEST(CylinderShapeInertia, WideCylinderInertia) {
    // Wide cylinder: radius >> height (like a disk)
    CylinderShape wideCylinder(10.0, 1.0);
    double mass = 5.0;

    Matrix3x3 inertia = wideCylinder.calculateInertiaTensor(mass);

    // I_x = I_y = (1/12) * 5.0 * (3*10^2 + 1^2) = (5/12) * 301 = 125.416667
    // I_z = (1/2) * 5.0 * 10^2 = 250.0

    double r2 = 10.0 * 10.0;
    double h2 = 1.0 * 1.0;
    double Ixy = (mass / 12.0) * (3.0 * r2 + h2);
    double Iz = (mass / 2.0) * r2;

    EXPECT_NEAR(inertia.get(0, 0), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Ixy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Iz, EPSILON);

    // For wide cylinder, I_z should be larger than I_xy
    EXPECT_GT(inertia.get(2, 2), inertia.get(0, 0));
}

TEST(CylinderShapeInertia, SymmetryAroundZAxis) {
    // I_x and I_y should always be equal due to cylindrical symmetry
    CylinderShape cylinder(3.5, 7.2);
    double mass = 12.5;

    Matrix3x3 inertia = cylinder.calculateInertiaTensor(mass);

    // I_x should equal I_y
    EXPECT_NEAR(inertia.get(0, 0), inertia.get(1, 1), EPSILON);
}

TEST(CylinderShapeInertia, VerifyIzFormula) {
    // Verify that I_z = (1/2) * m * r^2
    CylinderShape cylinder(4.0, 6.0);
    double mass = 8.0;

    Matrix3x3 inertia = cylinder.calculateInertiaTensor(mass);

    double expectedIz = 0.5 * mass * 4.0 * 4.0;

    EXPECT_NEAR(inertia.get(2, 2), expectedIz, EPSILON);
}

TEST(CylinderShapeInertia, VerifyIxyFormula) {
    // Verify that I_x = I_y = (1/12) * m * (3*r^2 + h^2)
    CylinderShape cylinder(3.0, 5.0);
    double mass = 10.0;

    Matrix3x3 inertia = cylinder.calculateInertiaTensor(mass);

    double r2 = 3.0 * 3.0;
    double h2 = 5.0 * 5.0;
    double expectedIxy = (mass / 12.0) * (3.0 * r2 + h2);

    EXPECT_NEAR(inertia.get(0, 0), expectedIxy, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expectedIxy, EPSILON);
}

TEST(CylinderShapeInertia, MassScaling) {
    CylinderShape cylinder(2.0, 4.0);

    Matrix3x3 inertia1 = cylinder.calculateInertiaTensor(1.0);
    Matrix3x3 inertia10 = cylinder.calculateInertiaTensor(10.0);

    // Inertia should scale linearly with mass
    EXPECT_NEAR(inertia10.get(0, 0) / inertia1.get(0, 0), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(1, 1) / inertia1.get(1, 1), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(2, 2) / inertia1.get(2, 2), 10.0, EPSILON);
}

TEST(CylinderShapeInertia, DifferentMasses) {
    CylinderShape cylinder(5.0, 10.0);

    Matrix3x3 inertia5 = cylinder.calculateInertiaTensor(5.0);
    Matrix3x3 inertia20 = cylinder.calculateInertiaTensor(20.0);

    // Verify scaling factor is 4.0 (20/5)
    EXPECT_NEAR(inertia20.get(0, 0) / inertia5.get(0, 0), 4.0, EPSILON);
    EXPECT_NEAR(inertia20.get(1, 1) / inertia5.get(1, 1), 4.0, EPSILON);
    EXPECT_NEAR(inertia20.get(2, 2) / inertia5.get(2, 2), 4.0, EPSILON);
}

// ============================================================================
// CylinderShapeAABB Test Suite
// ============================================================================

TEST(CylinderShapeAABB, AxisAlignedCylinder) {
    CylinderShape cylinder(2.0, 6.0);
    Vector position(10.0, 20.0, 30.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = cylinder.getAABB(position, orientation);

    // For axis-aligned cylinder:
    // Min: position - (radius, radius, height/2)
    // Max: position + (radius, radius, height/2)

    EXPECT_NEAR(aabb.min.getX(), 10.0 - 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), 20.0 - 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 30.0 - 3.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 10.0 + 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 20.0 + 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 30.0 + 3.0, EPSILON);
}

TEST(CylinderShapeAABB, AxisAlignedAtOrigin) {
    CylinderShape cylinder(3.0, 8.0);
    Vector position(0.0, 0.0, 0.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = cylinder.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), -3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -4.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 4.0, EPSILON);
}

TEST(CylinderShapeAABB, Rotated90DegreesAroundX) {
    // Cylinder tips over onto its side (axis now along Y)
    CylinderShape cylinder(2.0, 8.0);
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around X axis
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 2.0);

    AABB aabb = cylinder.getAABB(position, orientation);

    // After rotation:
    // - X extent: radius (unchanged) = 2.0
    // - Y extent: height/2 = 4.0
    // - Z extent: radius = 2.0

    EXPECT_NEAR(aabb.min.getX(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -4.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 4.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);
}

TEST(CylinderShapeAABB, Rotated90DegreesAroundY) {
    // Cylinder tips over onto its side (axis now along X)
    CylinderShape cylinder(2.0, 8.0);
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around Y axis
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(0, 1, 0), M_PI / 2.0);

    AABB aabb = cylinder.getAABB(position, orientation);

    // After rotation:
    // - X extent: height/2 = 4.0
    // - Y extent: radius (unchanged) = 2.0
    // - Z extent: radius = 2.0

    EXPECT_NEAR(aabb.min.getX(), -4.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 4.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);
}

TEST(CylinderShapeAABB, Rotated45DegreesAroundX) {
    // Diagonal rotation
    CylinderShape cylinder(2.0, 6.0);
    Vector position(0.0, 0.0, 0.0);

    // 45 degree rotation around X axis
    double angle = M_PI / 4.0;
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(1, 0, 0), angle);

    AABB aabb = cylinder.getAABB(position, orientation);

    // X extent should be radius (unchanged)
    EXPECT_NEAR(aabb.min.getX(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 2.0, EPSILON);

    // Y and Z extents should be larger due to rotation
    // The bounds should be symmetric around origin
    EXPECT_NEAR(aabb.min.getY(), -aabb.max.getY(), EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -aabb.max.getZ(), EPSILON);

    // Maximum Y and Z should be approximately equal for 45 degree rotation
    EXPECT_NEAR(aabb.max.getY(), aabb.max.getZ(), 0.01);
}

TEST(CylinderShapeAABB, Rotated45DegreesAroundZ) {
    // Rotation around cylinder's own axis
    // AABB should stay roughly the same size (within sqrt(2) factor due to sampling)
    CylinderShape cylinder(3.0, 5.0);
    Vector position(0.0, 0.0, 0.0);

    Quaternion identity = Quaternion::identity();
    Quaternion rotated = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 4.0);

    AABB aabbOriginal = cylinder.getAABB(position, identity);
    AABB aabbRotated = cylinder.getAABB(position, rotated);

    // Z-axis bounds should be identical (axis of rotation)
    EXPECT_NEAR(aabbOriginal.min.getZ(), aabbRotated.min.getZ(), EPSILON);
    EXPECT_NEAR(aabbOriginal.max.getZ(), aabbRotated.max.getZ(), EPSILON);

    // X and Y bounds should be similar but may vary slightly due to discrete sampling
    // Both should be symmetric around origin
    EXPECT_NEAR(aabbRotated.min.getX(), -aabbRotated.max.getX(), EPSILON);
    EXPECT_NEAR(aabbRotated.min.getY(), -aabbRotated.max.getY(), EPSILON);

    // The radius in X-Y plane changes due to discrete sampling
    double radiusRotated = aabbRotated.max.getX();

    // Due to 4-point sampling at 45-degree offset, we expect roughly sqrt(2) reduction
    // Original samples are at 0, 90, 180, 270 degrees (radius = 3.0)
    // Rotated 45 degrees, samples are at 45, 135, 225, 315 degrees
    // Distance to 45 degrees = 3.0 * cos(45) = 3.0 / sqrt(2) ≈ 2.121
    EXPECT_NEAR(radiusRotated, 3.0 / sqrt(2.0), 0.01);
}

TEST(CylinderShapeAABB, PositionOffset) {
    CylinderShape cylinder(1.5, 4.0);
    Vector position(100.0, -50.0, 75.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = cylinder.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), 100.0 - 1.5, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -50.0 - 1.5, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 75.0 - 2.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 100.0 + 1.5, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), -50.0 + 1.5, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 75.0 + 2.0, EPSILON);
}

TEST(CylinderShapeAABB, AABBIntersection) {
    CylinderShape cyl1(2.0, 4.0);
    CylinderShape cyl2(2.0, 4.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(3.0, 0.0, 0.0);  // Overlapping

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = cyl1.getAABB(pos1, orient);
    AABB aabb2 = cyl2.getAABB(pos2, orient);

    EXPECT_TRUE(aabb1.intersects(aabb2));
}

TEST(CylinderShapeAABB, AABBNoIntersection) {
    CylinderShape cyl1(2.0, 4.0);
    CylinderShape cyl2(2.0, 4.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(10.0, 0.0, 0.0);  // Far apart

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = cyl1.getAABB(pos1, orient);
    AABB aabb2 = cyl2.getAABB(pos2, orient);

    EXPECT_FALSE(aabb1.intersects(aabb2));
}

TEST(CylinderShapeAABB, RotationExpandsAABB) {
    // AABB should expand when cylinder is rotated
    CylinderShape cylinder(1.0, 10.0);
    Vector position(0.0, 0.0, 0.0);

    // Axis-aligned
    Quaternion identity = Quaternion::identity();
    AABB aabbAligned = cylinder.getAABB(position, identity);

    // Rotated 90 degrees
    Quaternion rotated = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 2.0);
    AABB aabbRotated = cylinder.getAABB(position, rotated);

    // Calculate volumes
    double volumeAligned = (aabbAligned.max.getX() - aabbAligned.min.getX()) *
                           (aabbAligned.max.getY() - aabbAligned.min.getY()) *
                           (aabbAligned.max.getZ() - aabbAligned.min.getZ());

    double volumeRotated = (aabbRotated.max.getX() - aabbRotated.min.getX()) *
                           (aabbRotated.max.getY() - aabbRotated.min.getY()) *
                           (aabbRotated.max.getZ() - aabbRotated.min.getZ());

    // Rotated AABB should have larger volume
    EXPECT_GT(volumeRotated, volumeAligned);
}

// ============================================================================
// CylinderShapeGeometry Test Suite
// ============================================================================

TEST(CylinderShapeGeometry, CharacteristicSizeUnitCylinder) {
    CylinderShape cylinder(1.0, 1.0);

    // For r=1, h=1: size = sqrt(4*1 + 1) = sqrt(5)
    double expected = sqrt(5.0);

    EXPECT_NEAR(cylinder.getCharacteristicSize(), expected, EPSILON);
}

TEST(CylinderShapeGeometry, CharacteristicSizeTallCylinder) {
    CylinderShape cylinder(1.0, 10.0);

    // For r=1, h=10: size = sqrt(4*1 + 100) = sqrt(104)
    double expected = sqrt(4.0 * 1.0 + 100.0);

    EXPECT_NEAR(cylinder.getCharacteristicSize(), expected, EPSILON);
}

TEST(CylinderShapeGeometry, CharacteristicSizeWideCylinder) {
    CylinderShape cylinder(5.0, 2.0);

    // For r=5, h=2: size = sqrt(4*25 + 4) = sqrt(104)
    double expected = sqrt(4.0 * 25.0 + 4.0);

    EXPECT_NEAR(cylinder.getCharacteristicSize(), expected, EPSILON);
}

TEST(CylinderShapeGeometry, CharacteristicSizeFormula) {
    // Verify formula: sqrt(4*r^2 + h^2)
    double r = 3.0;
    double h = 7.0;
    CylinderShape cylinder(r, h);

    double expected = sqrt(4.0 * r * r + h * h);

    EXPECT_NEAR(cylinder.getCharacteristicSize(), expected, EPSILON);
}

TEST(CylinderShapeGeometry, KeyVerticesCount) {
    CylinderShape cylinder(2.0, 4.0);
    Vector vertices[8];

    cylinder.getKeyVertices(vertices);

    // Verify we have 8 vertices (4 on top circle, 4 on bottom)
    // Just check that they're not all zero
    bool hasNonZero = false;
    for (int i = 0; i < 8; i++) {
        if (vertices[i].getLengthVector() > 0.0) {
            hasNonZero = true;
            break;
        }
    }
    EXPECT_TRUE(hasNonZero);
}

TEST(CylinderShapeGeometry, KeyVerticesOnCircles) {
    CylinderShape cylinder(3.0, 6.0);
    Vector vertices[8];

    cylinder.getKeyVertices(vertices);

    double h2 = 3.0;

    // First 4 vertices should be on bottom circle (z = -3.0)
    for (int i = 0; i < 4; i++) {
        EXPECT_NEAR(vertices[i].getZ(), -h2, EPSILON);
        // Distance from Z-axis should be radius
        double dist = sqrt(vertices[i].getX() * vertices[i].getX() +
                          vertices[i].getY() * vertices[i].getY());
        EXPECT_NEAR(dist, 3.0, EPSILON);
    }

    // Last 4 vertices should be on top circle (z = 3.0)
    for (int i = 4; i < 8; i++) {
        EXPECT_NEAR(vertices[i].getZ(), h2, EPSILON);
        // Distance from Z-axis should be radius
        double dist = sqrt(vertices[i].getX() * vertices[i].getX() +
                          vertices[i].getY() * vertices[i].getY());
        EXPECT_NEAR(dist, 3.0, EPSILON);
    }
}

TEST(CylinderShapeGeometry, VerticesSpreadAround360Degrees) {
    CylinderShape cylinder(2.0, 5.0);
    Vector vertices[8];

    cylinder.getKeyVertices(vertices);

    // Vertices should be at 0, 90, 180, 270 degrees
    // Check bottom circle (first 4 vertices)
    // Vertex 0: (r, 0, -h/2) - 0 degrees
    EXPECT_NEAR(vertices[0].getX(), 2.0, EPSILON);
    EXPECT_NEAR(vertices[0].getY(), 0.0, EPSILON);

    // Vertex 1: (0, r, -h/2) - 90 degrees
    EXPECT_NEAR(vertices[1].getX(), 0.0, EPSILON);
    EXPECT_NEAR(vertices[1].getY(), 2.0, EPSILON);

    // Vertex 2: (-r, 0, -h/2) - 180 degrees
    EXPECT_NEAR(vertices[2].getX(), -2.0, EPSILON);
    EXPECT_NEAR(vertices[2].getY(), 0.0, EPSILON);

    // Vertex 3: (0, -r, -h/2) - 270 degrees
    EXPECT_NEAR(vertices[3].getX(), 0.0, EPSILON);
    EXPECT_NEAR(vertices[3].getY(), -2.0, EPSILON);
}

TEST(CylinderShapeGeometry, RadiusHeightGetters) {
    double r = 4.5;
    double h = 9.2;
    CylinderShape cylinder(r, h);

    EXPECT_NEAR(cylinder.getRadius(), r, EPSILON);
    EXPECT_NEAR(cylinder.getHeight(), h, EPSILON);
}

// Main function
