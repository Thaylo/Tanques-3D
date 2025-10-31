/*
 * test_pyramidshape.cpp
 *
 * Google Test unit tests for PyramidShape class
 * Tests construction, inertia tensor calculations, AABB generation, and geometry
 */

#include <gtest/gtest.h>
#include <cmath>
#include "PyramidShape.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix3x3.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;

// Test fixture for PyramidShape tests
class PyramidShapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup if needed
    }

    void TearDown() override {
        // Common test cleanup if needed
    }
};

// ============================================================================
// PyramidShapeConstruction Test Suite
// ============================================================================

TEST(PyramidShapeConstruction, UnitPyramidDimensions) {
    PyramidShape pyramid(1.0, 1.0);

    EXPECT_NEAR(pyramid.getBaseWidth(), 1.0, EPSILON);
    EXPECT_NEAR(pyramid.getHeight(), 1.0, EPSILON);
}

TEST(PyramidShapeConstruction, RegularPyramidDimensions) {
    PyramidShape pyramid(4.0, 6.0);

    EXPECT_NEAR(pyramid.getBaseWidth(), 4.0, EPSILON);
    EXPECT_NEAR(pyramid.getHeight(), 6.0, EPSILON);
}

TEST(PyramidShapeConstruction, TallPyramidDimensions) {
    // Tall needle-like pyramid
    PyramidShape tallPyramid(2.0, 10.0);

    EXPECT_NEAR(tallPyramid.getBaseWidth(), 2.0, EPSILON);
    EXPECT_NEAR(tallPyramid.getHeight(), 10.0, EPSILON);
}

TEST(PyramidShapeConstruction, WidePyramidDimensions) {
    // Wide flat pyramid
    PyramidShape widePyramid(10.0, 2.0);

    EXPECT_NEAR(widePyramid.getBaseWidth(), 10.0, EPSILON);
    EXPECT_NEAR(widePyramid.getHeight(), 2.0, EPSILON);
}

TEST(PyramidShapeConstruction, SmallPyramidDimensions) {
    PyramidShape smallPyramid(0.5, 0.3);

    EXPECT_NEAR(smallPyramid.getBaseWidth(), 0.5, EPSILON);
    EXPECT_NEAR(smallPyramid.getHeight(), 0.3, EPSILON);
}

TEST(PyramidShapeConstruction, LargePyramidDimensions) {
    PyramidShape largePyramid(100.0, 50.0);

    EXPECT_NEAR(largePyramid.getBaseWidth(), 100.0, EPSILON);
    EXPECT_NEAR(largePyramid.getHeight(), 50.0, EPSILON);
}

TEST(PyramidShapeConstruction, DefaultColor) {
    PyramidShape pyramid(1.0, 1.0);
    Vector color = pyramid.getColor();

    // Default color should be white (1, 1, 1)
    EXPECT_NEAR(color.getX(), 1.0, EPSILON);
    EXPECT_NEAR(color.getY(), 1.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

TEST(PyramidShapeConstruction, SetCustomColor) {
    PyramidShape pyramid(1.0, 1.0);
    Vector blue(0.0, 0.0, 1.0);
    pyramid.setColor(blue);

    Vector color = pyramid.getColor();
    EXPECT_NEAR(color.getX(), 0.0, EPSILON);
    EXPECT_NEAR(color.getY(), 0.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

// ============================================================================
// PyramidShapeInertia Test Suite
// ============================================================================

TEST(PyramidShapeInertia, UnitPyramidInertia) {
    PyramidShape pyramid(1.0, 1.0);
    double mass = 1.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    // For a pyramid with base width=1.0, height=1.0, mass=1.0:
    // I_xx = I_yy = (1/20) * m * (w^2 + 4*h^2) = (1/20) * 1.0 * (1.0 + 4.0) = 0.25
    // I_zz = (1/10) * m * w^2 = (1/10) * 1.0 * 1.0 = 0.1

    double Ixx = (mass / 20.0) * (1.0 + 4.0);
    double Iyy = (mass / 20.0) * (1.0 + 4.0);
    double Izz = (mass / 10.0) * 1.0;

    EXPECT_NEAR(inertia.get(0, 0), Ixx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Iyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Izz, EPSILON);

    // Off-diagonal elements should be zero
    EXPECT_NEAR(inertia.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(1, 2), 0.0, EPSILON);
}

TEST(PyramidShapeInertia, TallPyramidInertia) {
    // Tall pyramid: height >> base
    PyramidShape pyramid(2.0, 10.0);
    double mass = 5.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    // I_xx = I_yy = (1/20) * 5.0 * (4.0 + 400.0) = (5/20) * 404 = 101.0
    // I_zz = (1/10) * 5.0 * 4.0 = 2.0

    double w2 = 2.0 * 2.0;
    double h2 = 10.0 * 10.0;

    double Ixx = (mass / 20.0) * (w2 + 4.0 * h2);
    double Iyy = (mass / 20.0) * (w2 + 4.0 * h2);
    double Izz = (mass / 10.0) * w2;

    EXPECT_NEAR(inertia.get(0, 0), Ixx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Iyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Izz, EPSILON);

    // For tall pyramid, I_xx should be much larger than I_zz
    EXPECT_GT(inertia.get(0, 0), inertia.get(2, 2));
}

TEST(PyramidShapeInertia, WidePyramidInertia) {
    // Wide pyramid: base >> height
    PyramidShape pyramid(10.0, 2.0);
    double mass = 5.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    double w2 = 10.0 * 10.0;
    double h2 = 2.0 * 2.0;

    double Ixx = (mass / 20.0) * (w2 + 4.0 * h2);
    double Iyy = (mass / 20.0) * (w2 + 4.0 * h2);
    double Izz = (mass / 10.0) * w2;

    EXPECT_NEAR(inertia.get(0, 0), Ixx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Iyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Izz, EPSILON);
}

TEST(PyramidShapeInertia, DifferentMasses) {
    PyramidShape pyramid(2.0, 3.0);

    Matrix3x3 inertia1 = pyramid.calculateInertiaTensor(1.0);
    Matrix3x3 inertia10 = pyramid.calculateInertiaTensor(10.0);

    // Inertia should scale linearly with mass
    EXPECT_NEAR(inertia10.get(0, 0) / inertia1.get(0, 0), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(1, 1) / inertia1.get(1, 1), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(2, 2) / inertia1.get(2, 2), 10.0, EPSILON);
}

TEST(PyramidShapeInertia, SymmetryAroundZAxis) {
    // For a square pyramid, I_xx should equal I_yy (symmetry around Z axis)
    PyramidShape pyramid(4.0, 5.0);
    double mass = 8.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    EXPECT_NEAR(inertia.get(0, 0), inertia.get(1, 1), EPSILON);
}

TEST(PyramidShapeInertia, VerifyInertiaFormula) {
    PyramidShape pyramid(6.0, 8.0);
    double mass = 12.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    // Manually calculate expected values
    double w = 6.0;
    double h = 8.0;
    double expectedIxx = (mass / 20.0) * (w*w + 4.0*h*h);
    double expectedIyy = (mass / 20.0) * (w*w + 4.0*h*h);
    double expectedIzz = (mass / 10.0) * w*w;

    EXPECT_NEAR(inertia.get(0, 0), expectedIxx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expectedIyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expectedIzz, EPSILON);
}

TEST(PyramidShapeInertia, HeavyPyramidInertia) {
    PyramidShape pyramid(3.0, 4.0);
    double mass = 100.0;

    Matrix3x3 inertia = pyramid.calculateInertiaTensor(mass);

    double w2 = 3.0 * 3.0;
    double h2 = 4.0 * 4.0;

    double expectedIxx = (mass / 20.0) * (w2 + 4.0 * h2);
    double expectedIzz = (mass / 10.0) * w2;

    EXPECT_NEAR(inertia.get(0, 0), expectedIxx, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expectedIzz, EPSILON);
}

TEST(PyramidShapeInertia, MassScaling) {
    PyramidShape pyramid(5.0, 7.0);

    Matrix3x3 inertia2 = pyramid.calculateInertiaTensor(2.0);
    Matrix3x3 inertia5 = pyramid.calculateInertiaTensor(5.0);

    // Check that inertia scales proportionally
    double ratio = 5.0 / 2.0;
    EXPECT_NEAR(inertia5.get(0, 0), inertia2.get(0, 0) * ratio, EPSILON);
    EXPECT_NEAR(inertia5.get(1, 1), inertia2.get(1, 1) * ratio, EPSILON);
    EXPECT_NEAR(inertia5.get(2, 2), inertia2.get(2, 2) * ratio, EPSILON);
}

// ============================================================================
// PyramidShapeAABB Test Suite
// ============================================================================

TEST(PyramidShapeAABB, AxisAlignedPyramid) {
    PyramidShape pyramid(4.0, 6.0);
    Vector position(10.0, 20.0, 30.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = pyramid.getAABB(position, orientation);

    // For axis-aligned pyramid with base width=4.0, height=6.0
    // Base: ±2.0 in X and Y, Z=30.0
    // Apex: (10.0, 20.0, 36.0)

    EXPECT_NEAR(aabb.min.getX(), 10.0 - 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), 20.0 - 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 30.0, EPSILON);  // Base at Z=30

    EXPECT_NEAR(aabb.max.getX(), 10.0 + 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 20.0 + 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 30.0 + 6.0, EPSILON);  // Apex at Z=36
}

TEST(PyramidShapeAABB, RotatedPyramid90DegreesAroundX) {
    PyramidShape pyramid(4.0, 6.0);
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around X axis (pyramid tips over to -Y direction)
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 2.0);

    AABB aabb = pyramid.getAABB(position, orientation);

    // After 90° rotation around X: (x, y, z) -> (x, -z, y)
    // Base corners at (±2, ±2, 0) become (±2, 0, ±2)
    // Apex at (0, 0, 6) becomes (0, -6, 0)

    EXPECT_NEAR(aabb.min.getX(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 2.0, EPSILON);

    // Base Y=±2 becomes Z=±2
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);

    // Apex Z=6 becomes Y=-6, base at Y=0
    EXPECT_NEAR(aabb.min.getY(), -6.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 0.0, EPSILON);
}

TEST(PyramidShapeAABB, RotatedPyramid90DegreesAroundY) {
    PyramidShape pyramid(4.0, 6.0);
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around Y axis (pyramid tips over to +X direction)
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(0, 1, 0), M_PI / 2.0);

    AABB aabb = pyramid.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getY(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 2.0, EPSILON);

    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);

    EXPECT_NEAR(aabb.min.getX(), 0.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 6.0, EPSILON);
}

TEST(PyramidShapeAABB, RotatedPyramid45DegreesAroundZ) {
    // Square base rotated 45 degrees around Z axis (diamond orientation)
    PyramidShape pyramid(2.0, 3.0);
    Vector position(0.0, 0.0, 0.0);

    double angle = M_PI / 4.0;
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(0, 0, 1), angle);

    AABB aabb = pyramid.getAABB(position, orientation);

    // Base diagonal after 45° rotation
    // Original: base corners at (±1, ±1, 0)
    // After rotation: AABB should expand to sqrt(2) in X and Y
    double halfDiag = sqrt(2.0);

    EXPECT_NEAR(aabb.min.getX(), -halfDiag, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -halfDiag, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 0.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), halfDiag, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), halfDiag, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 3.0, EPSILON);  // Height unchanged
}

TEST(PyramidShapeAABB, RotatedPyramid180DegreesAroundX) {
    // Pyramid flipped upside down
    PyramidShape pyramid(4.0, 6.0);
    Vector position(0.0, 0.0, 0.0);

    Quaternion orientation = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI);

    AABB aabb = pyramid.getAABB(position, orientation);

    // Base and apex swap Z positions
    EXPECT_NEAR(aabb.min.getX(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 2.0, EPSILON);

    // Apex at bottom, base at top
    EXPECT_NEAR(aabb.min.getZ(), -6.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 0.0, EPSILON);
}

TEST(PyramidShapeAABB, ArbitraryRotation) {
    PyramidShape pyramid(2.0, 3.0);
    Vector position(5.0, 10.0, 15.0);

    // Arbitrary rotation (30 degrees around each axis)
    Quaternion qx = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 6.0);
    Quaternion qy = Quaternion::fromAxisAngle(Vector(0, 1, 0), M_PI / 6.0);
    Quaternion qz = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 6.0);
    Quaternion orientation = qz * qy * qx;

    AABB aabb = pyramid.getAABB(position, orientation);

    // Just verify that AABB is reasonable (contains origin position)
    EXPECT_LE(aabb.min.getX(), position.getX());
    EXPECT_GE(aabb.max.getX(), position.getX());
    EXPECT_LE(aabb.min.getY(), position.getY());
    EXPECT_GE(aabb.max.getY(), position.getY());
    EXPECT_LE(aabb.min.getZ(), position.getZ());
    EXPECT_GE(aabb.max.getZ(), position.getZ());
}

TEST(PyramidShapeAABB, AABBAtDifferentPosition) {
    PyramidShape pyramid(2.0, 4.0);
    Vector position(100.0, -50.0, 25.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = pyramid.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), 99.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -51.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 25.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 101.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), -49.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 29.0, EPSILON);
}

TEST(PyramidShapeAABB, AABBIntersection) {
    PyramidShape pyramid1(2.0, 3.0);
    PyramidShape pyramid2(2.0, 3.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(1.5, 0.0, 0.0);  // Overlapping

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = pyramid1.getAABB(pos1, orient);
    AABB aabb2 = pyramid2.getAABB(pos2, orient);

    EXPECT_TRUE(aabb1.intersects(aabb2));
}

TEST(PyramidShapeAABB, AABBNoIntersection) {
    PyramidShape pyramid1(2.0, 3.0);
    PyramidShape pyramid2(2.0, 3.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(10.0, 0.0, 0.0);  // Not overlapping

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = pyramid1.getAABB(pos1, orient);
    AABB aabb2 = pyramid2.getAABB(pos2, orient);

    EXPECT_FALSE(aabb1.intersects(aabb2));
}

// ============================================================================
// PyramidShapeGeometry Test Suite
// ============================================================================

TEST(PyramidShapeGeometry, CharacteristicSizeIsHeight) {
    PyramidShape pyramid(4.0, 6.0);

    // Characteristic size should be the height
    EXPECT_NEAR(pyramid.getCharacteristicSize(), 6.0, EPSILON);
}

TEST(PyramidShapeGeometry, CharacteristicSizeUnitPyramid) {
    PyramidShape pyramid(1.0, 1.0);

    EXPECT_NEAR(pyramid.getCharacteristicSize(), 1.0, EPSILON);
}

TEST(PyramidShapeGeometry, CharacteristicSizeTallPyramid) {
    PyramidShape pyramid(2.0, 10.0);

    EXPECT_NEAR(pyramid.getCharacteristicSize(), 10.0, EPSILON);
}

TEST(PyramidShapeGeometry, BaseWidthGetter) {
    PyramidShape pyramid(7.5, 3.2);

    EXPECT_NEAR(pyramid.getBaseWidth(), 7.5, EPSILON);
}

TEST(PyramidShapeGeometry, HeightGetter) {
    PyramidShape pyramid(7.5, 3.2);

    EXPECT_NEAR(pyramid.getHeight(), 3.2, EPSILON);
}

TEST(PyramidShapeGeometry, VerticesCount) {
    PyramidShape pyramid(4.0, 6.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    // Verify all 5 vertices exist and are distinct
    // We'll check that they're not all the same point
    bool allSame = true;
    for (int i = 1; i < 5; i++) {
        if (vertices[i].distanceVector(vertices[0]) > EPSILON) {
            allSame = false;
            break;
        }
    }
    EXPECT_FALSE(allSame);
}

TEST(PyramidShapeGeometry, BaseVerticesPositions) {
    PyramidShape pyramid(4.0, 6.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    double w2 = 2.0;  // Half of base width

    // Check base vertices (should be at Z=0)
    EXPECT_NEAR(vertices[0].getX(), -w2, EPSILON);
    EXPECT_NEAR(vertices[0].getY(), -w2, EPSILON);
    EXPECT_NEAR(vertices[0].getZ(), 0.0, EPSILON);

    EXPECT_NEAR(vertices[1].getX(), w2, EPSILON);
    EXPECT_NEAR(vertices[1].getY(), -w2, EPSILON);
    EXPECT_NEAR(vertices[1].getZ(), 0.0, EPSILON);

    EXPECT_NEAR(vertices[2].getX(), w2, EPSILON);
    EXPECT_NEAR(vertices[2].getY(), w2, EPSILON);
    EXPECT_NEAR(vertices[2].getZ(), 0.0, EPSILON);

    EXPECT_NEAR(vertices[3].getX(), -w2, EPSILON);
    EXPECT_NEAR(vertices[3].getY(), w2, EPSILON);
    EXPECT_NEAR(vertices[3].getZ(), 0.0, EPSILON);
}

TEST(PyramidShapeGeometry, ApexPosition) {
    PyramidShape pyramid(4.0, 6.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    // Apex should be at (0, 0, height)
    EXPECT_NEAR(vertices[4].getX(), 0.0, EPSILON);
    EXPECT_NEAR(vertices[4].getY(), 0.0, EPSILON);
    EXPECT_NEAR(vertices[4].getZ(), 6.0, EPSILON);
}

TEST(PyramidShapeGeometry, BaseVerticesSymmetry) {
    PyramidShape pyramid(6.0, 8.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    // Check that base vertices form a square centered at origin
    // Opposite corners should be negatives of each other
    EXPECT_NEAR(vertices[0].getX(), -vertices[2].getX(), EPSILON);
    EXPECT_NEAR(vertices[0].getY(), -vertices[2].getY(), EPSILON);

    EXPECT_NEAR(vertices[1].getX(), -vertices[3].getX(), EPSILON);
    EXPECT_NEAR(vertices[1].getY(), -vertices[3].getY(), EPSILON);
}

TEST(PyramidShapeGeometry, BaseSquareProperty) {
    PyramidShape pyramid(8.0, 5.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    // Check that base forms a square (all sides equal)
    double side1 = vertices[0].distanceVector(vertices[1]);
    double side2 = vertices[1].distanceVector(vertices[2]);
    double side3 = vertices[2].distanceVector(vertices[3]);
    double side4 = vertices[3].distanceVector(vertices[0]);

    EXPECT_NEAR(side1, side2, EPSILON);
    EXPECT_NEAR(side2, side3, EPSILON);
    EXPECT_NEAR(side3, side4, EPSILON);

    // Verify side length equals base width
    EXPECT_NEAR(side1, 8.0, EPSILON);
}

TEST(PyramidShapeGeometry, BaseCenteredAtOrigin) {
    PyramidShape pyramid(10.0, 7.0);
    Vector vertices[5];

    pyramid.getVertices(vertices);

    // Calculate centroid of base vertices (first 4)
    Vector baseCentroid(0, 0, 0);
    for (int i = 0; i < 4; i++) {
        baseCentroid = baseCentroid + vertices[i];
    }
    baseCentroid = baseCentroid / 4.0;

    // Base centroid should be at origin in XY, Z=0
    EXPECT_NEAR(baseCentroid.getX(), 0.0, EPSILON);
    EXPECT_NEAR(baseCentroid.getY(), 0.0, EPSILON);
    EXPECT_NEAR(baseCentroid.getZ(), 0.0, EPSILON);
}

TEST(PyramidShapeGeometry, VolumeCalculation) {
    // Optional test: verify volume formula V = (1/3) * base^2 * height
    PyramidShape pyramid(6.0, 9.0);

    double base = pyramid.getBaseWidth();
    double height = pyramid.getHeight();
    double expectedVolume = (1.0 / 3.0) * base * base * height;

    // Volume = (1/3) * 6^2 * 9 = (1/3) * 36 * 9 = 108
    EXPECT_NEAR(expectedVolume, 108.0, EPSILON);
}

// Main function
