/*
 * test_boxshape.cpp
 *
 * Google Test unit tests for BoxShape class
 * Tests construction, inertia tensor calculations, AABB generation, and geometry
 */

#include <gtest/gtest.h>
#include <cmath>
#include "BoxShape.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Matrix3x3.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;

// Test fixture for BoxShape tests
class BoxShapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup if needed
    }

    void TearDown() override {
        // Common test cleanup if needed
    }
};

// ============================================================================
// BoxShapeConstruction Test Suite
// ============================================================================

TEST(BoxShapeConstruction, DefaultCubeDimensions) {
    BoxShape cube(2.0, 2.0, 2.0);

    EXPECT_NEAR(cube.getWidth(), 2.0, EPSILON);
    EXPECT_NEAR(cube.getHeight(), 2.0, EPSILON);
    EXPECT_NEAR(cube.getDepth(), 2.0, EPSILON);
}

TEST(BoxShapeConstruction, RectangularBoxDimensions) {
    BoxShape box(4.0, 2.0, 1.0);

    EXPECT_NEAR(box.getWidth(), 4.0, EPSILON);
    EXPECT_NEAR(box.getHeight(), 2.0, EPSILON);
    EXPECT_NEAR(box.getDepth(), 1.0, EPSILON);
}

TEST(BoxShapeConstruction, SmallBoxDimensions) {
    BoxShape smallBox(0.5, 0.3, 0.2);

    EXPECT_NEAR(smallBox.getWidth(), 0.5, EPSILON);
    EXPECT_NEAR(smallBox.getHeight(), 0.3, EPSILON);
    EXPECT_NEAR(smallBox.getDepth(), 0.2, EPSILON);
}

TEST(BoxShapeConstruction, LargeBoxDimensions) {
    BoxShape largeBox(100.0, 50.0, 25.0);

    EXPECT_NEAR(largeBox.getWidth(), 100.0, EPSILON);
    EXPECT_NEAR(largeBox.getHeight(), 50.0, EPSILON);
    EXPECT_NEAR(largeBox.getDepth(), 25.0, EPSILON);
}

TEST(BoxShapeConstruction, DefaultColor) {
    BoxShape box(1.0, 1.0, 1.0);
    Vector color = box.getColor();

    // Default color should be white (1, 1, 1)
    EXPECT_NEAR(color.getX(), 1.0, EPSILON);
    EXPECT_NEAR(color.getY(), 1.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 1.0, EPSILON);
}

TEST(BoxShapeConstruction, SetCustomColor) {
    BoxShape box(1.0, 1.0, 1.0);
    Vector red(1.0, 0.0, 0.0);
    box.setColor(red);

    Vector color = box.getColor();
    EXPECT_NEAR(color.getX(), 1.0, EPSILON);
    EXPECT_NEAR(color.getY(), 0.0, EPSILON);
    EXPECT_NEAR(color.getZ(), 0.0, EPSILON);
}

// ============================================================================
// BoxShapeInertia Test Suite
// ============================================================================

TEST(BoxShapeInertia, UnitCubeInertia) {
    BoxShape cube(2.0, 2.0, 2.0);
    double mass = 1.0;

    Matrix3x3 inertia = cube.calculateInertiaTensor(mass);

    // For a cube with side 2.0 and mass 1.0:
    // I_xx = (1/12) * m * (h^2 + d^2) = (1/12) * 1.0 * (4.0 + 4.0) = 0.666667
    // I_yy = (1/12) * m * (w^2 + d^2) = (1/12) * 1.0 * (4.0 + 4.0) = 0.666667
    // I_zz = (1/12) * m * (w^2 + h^2) = (1/12) * 1.0 * (4.0 + 4.0) = 0.666667

    double expected = (1.0 / 12.0) * mass * (4.0 + 4.0);

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);

    // Off-diagonal elements should be zero
    EXPECT_NEAR(inertia.get(0, 1), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(0, 2), 0.0, EPSILON);
    EXPECT_NEAR(inertia.get(1, 2), 0.0, EPSILON);
}

TEST(BoxShapeInertia, RectangularBoxInertia) {
    BoxShape box(4.0, 2.0, 1.0);
    double mass = 10.0;

    Matrix3x3 inertia = box.calculateInertiaTensor(mass);

    // I_xx = (1/12) * 10.0 * (2^2 + 1^2) = (10/12) * 5 = 4.166667
    // I_yy = (1/12) * 10.0 * (4^2 + 1^2) = (10/12) * 17 = 14.166667
    // I_zz = (1/12) * 10.0 * (4^2 + 2^2) = (10/12) * 20 = 16.666667

    double Ixx = (mass / 12.0) * (2.0*2.0 + 1.0*1.0);
    double Iyy = (mass / 12.0) * (4.0*4.0 + 1.0*1.0);
    double Izz = (mass / 12.0) * (4.0*4.0 + 2.0*2.0);

    EXPECT_NEAR(inertia.get(0, 0), Ixx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Iyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Izz, EPSILON);
}

TEST(BoxShapeInertia, HeavyBoxInertia) {
    BoxShape box(2.0, 2.0, 2.0);
    double mass = 100.0;

    Matrix3x3 inertia = box.calculateInertiaTensor(mass);

    double expected = (mass / 12.0) * (4.0 + 4.0);

    EXPECT_NEAR(inertia.get(0, 0), expected, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), expected, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), expected, EPSILON);
}

TEST(BoxShapeInertia, ThinPlateInertia) {
    // Thin plate: very small depth
    BoxShape plate(10.0, 5.0, 0.1);
    double mass = 5.0;

    Matrix3x3 inertia = plate.calculateInertiaTensor(mass);

    // I_xx should be dominated by height^2
    // I_yy should be dominated by width^2
    // I_zz should be dominated by width^2 + height^2

    double Ixx = (mass / 12.0) * (5.0*5.0 + 0.1*0.1);
    double Iyy = (mass / 12.0) * (10.0*10.0 + 0.1*0.1);
    double Izz = (mass / 12.0) * (10.0*10.0 + 5.0*5.0);

    EXPECT_NEAR(inertia.get(0, 0), Ixx, EPSILON);
    EXPECT_NEAR(inertia.get(1, 1), Iyy, EPSILON);
    EXPECT_NEAR(inertia.get(2, 2), Izz, EPSILON);
}

TEST(BoxShapeInertia, InertiaDifferentMasses) {
    BoxShape box(2.0, 2.0, 2.0);

    Matrix3x3 inertia1 = box.calculateInertiaTensor(1.0);
    Matrix3x3 inertia10 = box.calculateInertiaTensor(10.0);

    // Inertia should scale linearly with mass
    EXPECT_NEAR(inertia10.get(0, 0) / inertia1.get(0, 0), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(1, 1) / inertia1.get(1, 1), 10.0, EPSILON);
    EXPECT_NEAR(inertia10.get(2, 2) / inertia1.get(2, 2), 10.0, EPSILON);
}

// ============================================================================
// BoxShapeAABB Test Suite
// ============================================================================

TEST(BoxShapeAABB, AxisAlignedBox) {
    BoxShape box(2.0, 4.0, 6.0);
    Vector position(10.0, 20.0, 30.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = box.getAABB(position, orientation);

    // Min corner: position - half dimensions
    EXPECT_NEAR(aabb.min.getX(), 10.0 - 1.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), 20.0 - 2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 30.0 - 3.0, EPSILON);

    // Max corner: position + half dimensions
    EXPECT_NEAR(aabb.max.getX(), 10.0 + 1.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 20.0 + 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 30.0 + 3.0, EPSILON);
}

TEST(BoxShapeAABB, RotatedBox45DegreesAroundZ) {
    // Square box rotated 45 degrees around Z axis
    BoxShape box(2.0, 2.0, 2.0);
    Vector position(0.0, 0.0, 0.0);

    // 45 degree rotation around Z axis
    double angle = M_PI / 4.0;
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(0, 0, 1), angle);

    AABB aabb = box.getAABB(position, orientation);

    // For a 2x2 square rotated 45 degrees, the AABB should be sqrt(2) on each side
    // Half-diagonal of a 2x2 square = sqrt(2)
    double halfDiag = sqrt(2.0);

    EXPECT_NEAR(aabb.min.getX(), -halfDiag, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -halfDiag, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -1.0, EPSILON);  // Z unchanged

    EXPECT_NEAR(aabb.max.getX(), halfDiag, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), halfDiag, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 1.0, EPSILON);
}

TEST(BoxShapeAABB, RotatedBox90DegreesAroundX) {
    // Rectangular box rotated 90 degrees around X axis
    BoxShape box(2.0, 4.0, 6.0);  // width=2, height=4, depth=6
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around X axis swaps Y and Z
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(1, 0, 0), M_PI / 2.0);

    AABB aabb = box.getAABB(position, orientation);

    // After 90° rotation around X:
    // - Width (X) stays the same: 2.0
    // - Height (Y) becomes depth: 6.0
    // - Depth (Z) becomes height: 4.0

    EXPECT_NEAR(aabb.min.getX(), -1.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 1.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);
}

TEST(BoxShapeAABB, RotatedBox90DegreesAroundY) {
    BoxShape box(2.0, 4.0, 6.0);
    Vector position(0.0, 0.0, 0.0);

    // 90 degree rotation around Y axis swaps X and Z
    Quaternion orientation = Quaternion::fromAxisAngle(Vector(0, 1, 0), M_PI / 2.0);

    AABB aabb = box.getAABB(position, orientation);

    // After 90° rotation around Y:
    // - Width (X) becomes depth: 6.0
    // - Height (Y) stays the same: 4.0
    // - Depth (Z) becomes width: 2.0

    EXPECT_NEAR(aabb.min.getX(), -3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -1.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 1.0, EPSILON);
}

TEST(BoxShapeAABB, AABBAtDifferentPosition) {
    BoxShape box(2.0, 2.0, 2.0);
    Vector position(100.0, -50.0, 25.0);
    Quaternion orientation = Quaternion::identity();

    AABB aabb = box.getAABB(position, orientation);

    EXPECT_NEAR(aabb.min.getX(), 99.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -51.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 24.0, EPSILON);

    EXPECT_NEAR(aabb.max.getX(), 101.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), -49.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 26.0, EPSILON);
}

TEST(BoxShapeAABB, AABBIntersection) {
    BoxShape box1(2.0, 2.0, 2.0);
    BoxShape box2(2.0, 2.0, 2.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(1.5, 0.0, 0.0);  // Overlapping

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = box1.getAABB(pos1, orient);
    AABB aabb2 = box2.getAABB(pos2, orient);

    EXPECT_TRUE(aabb1.intersects(aabb2));
}

TEST(BoxShapeAABB, AABBNoIntersection) {
    BoxShape box1(2.0, 2.0, 2.0);
    BoxShape box2(2.0, 2.0, 2.0);

    Vector pos1(0.0, 0.0, 0.0);
    Vector pos2(5.0, 0.0, 0.0);  // Not overlapping

    Quaternion orient = Quaternion::identity();

    AABB aabb1 = box1.getAABB(pos1, orient);
    AABB aabb2 = box2.getAABB(pos2, orient);

    EXPECT_FALSE(aabb1.intersects(aabb2));
}

// ============================================================================
// BoxShapeGeometry Test Suite
// ============================================================================

TEST(BoxShapeGeometry, CharacteristicSizeCube) {
    BoxShape cube(2.0, 2.0, 2.0);

    // Diagonal of a 2x2x2 cube
    double expected = sqrt(2.0*2.0 + 2.0*2.0 + 2.0*2.0);

    EXPECT_NEAR(cube.getCharacteristicSize(), expected, EPSILON);
}

TEST(BoxShapeGeometry, CharacteristicSizeRectangular) {
    BoxShape box(3.0, 4.0, 5.0);

    double expected = sqrt(3.0*3.0 + 4.0*4.0 + 5.0*5.0);

    EXPECT_NEAR(box.getCharacteristicSize(), expected, EPSILON);
}

TEST(BoxShapeGeometry, CharacteristicSizeUnitCube) {
    BoxShape unit(1.0, 1.0, 1.0);

    double expected = sqrt(3.0);

    EXPECT_NEAR(unit.getCharacteristicSize(), expected, EPSILON);
}

TEST(BoxShapeGeometry, VerticesCountAndPositions) {
    BoxShape box(2.0, 4.0, 6.0);
    Vector vertices[8];

    box.getVertices(vertices);

    // Verify all 8 vertices
    // Bottom-left-back
    EXPECT_NEAR(vertices[0].getX(), -1.0, EPSILON);
    EXPECT_NEAR(vertices[0].getY(), -2.0, EPSILON);
    EXPECT_NEAR(vertices[0].getZ(), -3.0, EPSILON);

    // Bottom-right-back
    EXPECT_NEAR(vertices[1].getX(), 1.0, EPSILON);
    EXPECT_NEAR(vertices[1].getY(), -2.0, EPSILON);
    EXPECT_NEAR(vertices[1].getZ(), -3.0, EPSILON);

    // Top-right-back
    EXPECT_NEAR(vertices[2].getX(), 1.0, EPSILON);
    EXPECT_NEAR(vertices[2].getY(), 2.0, EPSILON);
    EXPECT_NEAR(vertices[2].getZ(), -3.0, EPSILON);

    // Top-left-back
    EXPECT_NEAR(vertices[3].getX(), -1.0, EPSILON);
    EXPECT_NEAR(vertices[3].getY(), 2.0, EPSILON);
    EXPECT_NEAR(vertices[3].getZ(), -3.0, EPSILON);

    // Bottom-left-front
    EXPECT_NEAR(vertices[4].getX(), -1.0, EPSILON);
    EXPECT_NEAR(vertices[4].getY(), -2.0, EPSILON);
    EXPECT_NEAR(vertices[4].getZ(), 3.0, EPSILON);

    // Bottom-right-front
    EXPECT_NEAR(vertices[5].getX(), 1.0, EPSILON);
    EXPECT_NEAR(vertices[5].getY(), -2.0, EPSILON);
    EXPECT_NEAR(vertices[5].getZ(), 3.0, EPSILON);

    // Top-right-front
    EXPECT_NEAR(vertices[6].getX(), 1.0, EPSILON);
    EXPECT_NEAR(vertices[6].getY(), 2.0, EPSILON);
    EXPECT_NEAR(vertices[6].getZ(), 3.0, EPSILON);

    // Top-left-front
    EXPECT_NEAR(vertices[7].getX(), -1.0, EPSILON);
    EXPECT_NEAR(vertices[7].getY(), 2.0, EPSILON);
    EXPECT_NEAR(vertices[7].getZ(), 3.0, EPSILON);
}

TEST(BoxShapeGeometry, VerticesCenteredAtOrigin) {
    BoxShape box(4.0, 6.0, 8.0);
    Vector vertices[8];

    box.getVertices(vertices);

    // Calculate centroid of all vertices
    Vector centroid(0, 0, 0);
    for (int i = 0; i < 8; i++) {
        centroid = centroid + vertices[i];
    }
    centroid = centroid / 8.0;

    // Centroid should be at origin
    EXPECT_NEAR(centroid.getX(), 0.0, EPSILON);
    EXPECT_NEAR(centroid.getY(), 0.0, EPSILON);
    EXPECT_NEAR(centroid.getZ(), 0.0, EPSILON);
}

TEST(BoxShapeGeometry, VerticesSymmetry) {
    BoxShape box(2.0, 4.0, 6.0);
    Vector vertices[8];

    box.getVertices(vertices);

    // Verify symmetry: opposite vertices should be negatives
    // vertices[0] and vertices[6] are diagonal opposites
    EXPECT_NEAR(vertices[0].getX(), -vertices[6].getX(), EPSILON);
    EXPECT_NEAR(vertices[0].getY(), -vertices[6].getY(), EPSILON);
    EXPECT_NEAR(vertices[0].getZ(), -vertices[6].getZ(), EPSILON);

    // vertices[1] and vertices[7] are diagonal opposites
    EXPECT_NEAR(vertices[1].getX(), -vertices[7].getX(), EPSILON);
    EXPECT_NEAR(vertices[1].getY(), -vertices[7].getY(), EPSILON);
    EXPECT_NEAR(vertices[1].getZ(), -vertices[7].getZ(), EPSILON);
}

TEST(BoxShapeGeometry, VerticesDistanceFromCenter) {
    BoxShape box(2.0, 4.0, 6.0);
    Vector vertices[8];

    box.getVertices(vertices);

    // All vertices should be at the same distance from origin (center)
    double expectedDistance = sqrt(1.0*1.0 + 2.0*2.0 + 3.0*3.0);

    for (int i = 0; i < 8; i++) {
        double distance = vertices[i].getLengthVector();
        EXPECT_NEAR(distance, expectedDistance, EPSILON);
    }
}

// Main function
