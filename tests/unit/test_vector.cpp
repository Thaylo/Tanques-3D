/**
 * test_vector.cpp - Unit tests for Vector class
 */

#include "core/Vector.h"
#include <cmath>
#include <gtest/gtest.h>
#include <sstream>

// Constructor tests
TEST(VectorTest, DefaultConstructorIsZero) {
  Vector v;
  EXPECT_DOUBLE_EQ(v.getX(), 0.0);
  EXPECT_DOUBLE_EQ(v.getY(), 0.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 0.0);
}

TEST(VectorTest, ParameterizedConstructor) {
  Vector v(1.5, 2.5, 3.5);
  EXPECT_DOUBLE_EQ(v.getX(), 1.5);
  EXPECT_DOUBLE_EQ(v.getY(), 2.5);
  EXPECT_DOUBLE_EQ(v.getZ(), 3.5);
}

// Setter tests
TEST(VectorTest, Setters) {
  Vector v;
  v.setX(10.0);
  v.setY(20.0);
  v.setZ(30.0);
  EXPECT_DOUBLE_EQ(v.getX(), 10.0);
  EXPECT_DOUBLE_EQ(v.getY(), 20.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 30.0);
}

// Arithmetic operator tests
TEST(VectorTest, Addition) {
  Vector a(1, 2, 3);
  Vector b(4, 5, 6);
  Vector c = a + b;
  EXPECT_DOUBLE_EQ(c.getX(), 5.0);
  EXPECT_DOUBLE_EQ(c.getY(), 7.0);
  EXPECT_DOUBLE_EQ(c.getZ(), 9.0);
}

TEST(VectorTest, Subtraction) {
  Vector a(5, 7, 9);
  Vector b(1, 2, 3);
  Vector c = a - b;
  EXPECT_DOUBLE_EQ(c.getX(), 4.0);
  EXPECT_DOUBLE_EQ(c.getY(), 5.0);
  EXPECT_DOUBLE_EQ(c.getZ(), 6.0);
}

TEST(VectorTest, ScalarMultiplication) {
  Vector v(1, 2, 3);
  Vector result = v * 2.0;
  EXPECT_DOUBLE_EQ(result.getX(), 2.0);
  EXPECT_DOUBLE_EQ(result.getY(), 4.0);
  EXPECT_DOUBLE_EQ(result.getZ(), 6.0);
}

TEST(VectorTest, ScalarDivision) {
  Vector v(4, 8, 12);
  Vector result = v / 2.0;
  EXPECT_DOUBLE_EQ(result.getX(), 2.0);
  EXPECT_DOUBLE_EQ(result.getY(), 4.0);
  EXPECT_DOUBLE_EQ(result.getZ(), 6.0);
}

// Length and normalization tests
TEST(VectorTest, Length) {
  Vector v(3, 4, 0);
  EXPECT_DOUBLE_EQ(v.getLengthVector(), 5.0);
}

TEST(VectorTest, Length3D) {
  Vector v(1, 2, 2);
  EXPECT_DOUBLE_EQ(v.getLengthVector(), 3.0);
}

TEST(VectorTest, ZeroVectorLength) {
  Vector v(0, 0, 0);
  EXPECT_DOUBLE_EQ(v.getLengthVector(), 0.0);
}

TEST(VectorTest, Normalization) {
  Vector v(3, 4, 0);
  v.setVectorLength(1.0);
  EXPECT_NEAR(v.getLengthVector(), 1.0, 1e-9);
  EXPECT_NEAR(v.getX(), 0.6, 1e-9);
  EXPECT_NEAR(v.getY(), 0.8, 1e-9);
}

TEST(VectorTest, SetVectorLength) {
  Vector v(1, 0, 0);
  v.setVectorLength(5.0);
  EXPECT_DOUBLE_EQ(v.getLengthVector(), 5.0);
  EXPECT_DOUBLE_EQ(v.getX(), 5.0);
}

// Dot product tests
TEST(VectorTest, DotProduct) {
  Vector a(1, 2, 3);
  Vector b(4, 5, 6);
  EXPECT_DOUBLE_EQ(a.dotProduct(b), 32.0); // 1*4 + 2*5 + 3*6 = 32
}

TEST(VectorTest, DotProductOrthogonal) {
  Vector a(1, 0, 0);
  Vector b(0, 1, 0);
  EXPECT_DOUBLE_EQ(a.dotProduct(b), 0.0);
}

TEST(VectorTest, DotProductParallel) {
  Vector a(1, 0, 0);
  Vector b(5, 0, 0);
  EXPECT_DOUBLE_EQ(a.dotProduct(b), 5.0);
}

// Cross product tests
TEST(VectorTest, CrossProduct) {
  Vector x(1, 0, 0);
  Vector y(0, 1, 0);
  Vector z = x.crossProduct(y);
  EXPECT_DOUBLE_EQ(z.getX(), 0.0);
  EXPECT_DOUBLE_EQ(z.getY(), 0.0);
  EXPECT_DOUBLE_EQ(z.getZ(), 1.0);
}

TEST(VectorTest, CrossProductReverse) {
  Vector x(1, 0, 0);
  Vector y(0, 1, 0);
  Vector z = y.crossProduct(x);
  EXPECT_DOUBLE_EQ(z.getZ(), -1.0);
}

TEST(VectorTest, CrossProductSelf) {
  Vector v(1, 2, 3);
  Vector result = v.crossProduct(v);
  EXPECT_DOUBLE_EQ(result.getX(), 0.0);
  EXPECT_DOUBLE_EQ(result.getY(), 0.0);
  EXPECT_DOUBLE_EQ(result.getZ(), 0.0);
}

// Distance tests
TEST(VectorTest, Distance) {
  Vector a(0, 0, 0);
  Vector b(3, 4, 0);
  EXPECT_DOUBLE_EQ(a.distanceVector(b), 5.0);
}

// Angle tests
TEST(VectorTest, AngleOrthogonal) {
  Vector a(1, 0, 0);
  Vector b(0, 1, 0);
  EXPECT_NEAR(a.angleVectors(b), M_PI / 2, 1e-9);
}

TEST(VectorTest, AngleParallel) {
  Vector a(1, 0, 0);
  Vector b(2, 0, 0);
  EXPECT_NEAR(a.angleVectors(b), 0.0, 1e-9);
}

// Rotation tests
TEST(VectorTest, RotateVector90Degrees) {
  Vector v(1, 0, 0);
  Vector axis(0, 0, 1);
  Vector rotated = v.rotateVector(axis, M_PI / 2);
  EXPECT_NEAR(rotated.getX(), 0.0, 1e-9);
  EXPECT_NEAR(rotated.getY(), 1.0, 1e-9);
  EXPECT_NEAR(rotated.getZ(), 0.0, 1e-9);
}

TEST(VectorTest, RotateVector180Degrees) {
  Vector v(1, 0, 0);
  Vector axis(0, 0, 1);
  Vector rotated = v.rotateVector(axis, M_PI);
  EXPECT_NEAR(rotated.getX(), -1.0, 1e-9);
  EXPECT_NEAR(rotated.getY(), 0.0, 1e-9);
}

// Assignment tests
TEST(VectorTest, Assignment) {
  Vector a(1, 2, 3);
  Vector b;
  b = a;
  EXPECT_DOUBLE_EQ(b.getX(), 1.0);
  EXPECT_DOUBLE_EQ(b.getY(), 2.0);
  EXPECT_DOUBLE_EQ(b.getZ(), 3.0);
}

TEST(VectorTest, AssignmentZero) {
  Vector v(1, 2, 3);
  v = 0;
  EXPECT_DOUBLE_EQ(v.getX(), 0.0);
  EXPECT_DOUBLE_EQ(v.getY(), 0.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 0.0);
}

// C++23 compound assignment tests
TEST(VectorTest, CompoundAddition) {
  Vector v(1, 2, 3);
  v += Vector(4, 5, 6);
  EXPECT_DOUBLE_EQ(v.getX(), 5.0);
  EXPECT_DOUBLE_EQ(v.getY(), 7.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 9.0);
}

TEST(VectorTest, CompoundSubtraction) {
  Vector v(5, 7, 9);
  v -= Vector(1, 2, 3);
  EXPECT_DOUBLE_EQ(v.getX(), 4.0);
  EXPECT_DOUBLE_EQ(v.getY(), 5.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 6.0);
}

TEST(VectorTest, CompoundMultiplication) {
  Vector v(1, 2, 3);
  v *= 2.0;
  EXPECT_DOUBLE_EQ(v.getX(), 2.0);
  EXPECT_DOUBLE_EQ(v.getY(), 4.0);
  EXPECT_DOUBLE_EQ(v.getZ(), 6.0);
}

// getLengthSquared tests
TEST(VectorTest, LengthSquared) {
  Vector v(3, 4, 0);
  EXPECT_DOUBLE_EQ(v.getLengthSquared(), 25.0);
}

TEST(VectorTest, LengthSquared3D) {
  Vector v(1, 2, 2);
  EXPECT_DOUBLE_EQ(v.getLengthSquared(), 9.0);
}

// getNormalVector tests
TEST(VectorTest, GetNormalVector) {
  Vector v(3, 4, 0);
  Vector norm = v.getNormalVector();
  EXPECT_NEAR(norm.getX(), 0.6, 1e-9);
  EXPECT_NEAR(norm.getY(), 0.8, 1e-9);
  EXPECT_NEAR(norm.getLengthVector(), 1.0, 1e-9);
}

TEST(VectorTest, GetNormalVectorZero) {
  Vector v(0, 0, 0);
  Vector norm = v.getNormalVector();
  EXPECT_DOUBLE_EQ(norm.getX(), 0.0);
  EXPECT_DOUBLE_EQ(norm.getY(), 0.0);
  EXPECT_DOUBLE_EQ(norm.getZ(), 0.0);
}

// Spaceship operator tests
TEST(VectorTest, SpaceshipOperatorEqual) {
  Vector a(1, 0, 0);
  Vector b(0, 1, 0);
  // Both have length 1, so should be equivalent in ordering
  EXPECT_TRUE((a <=> b) == 0);
}

TEST(VectorTest, SpaceshipOperatorLess) {
  Vector shorter(1, 0, 0); // length 1
  Vector longer(3, 4, 0);  // length 5
  EXPECT_TRUE(shorter < longer);
  EXPECT_FALSE(longer < shorter);
}

TEST(VectorTest, EqualityOperator) {
  Vector a(1, 2, 3);
  Vector b(1, 2, 3);
  Vector c(1, 2, 4);
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

// Scalar left multiplication
TEST(VectorTest, ScalarLeftMultiplication) {
  Vector v(1, 2, 3);
  Vector result = 2.0 * v;
  EXPECT_DOUBLE_EQ(result.getX(), 2.0);
  EXPECT_DOUBLE_EQ(result.getY(), 4.0);
  EXPECT_DOUBLE_EQ(result.getZ(), 6.0);
}

// Copy and Move semantics
TEST(VectorTest, CopyConstructor) {
  Vector a(1, 2, 3);
  Vector b(a);
  EXPECT_DOUBLE_EQ(b.getX(), 1.0);
  EXPECT_DOUBLE_EQ(b.getY(), 2.0);
  EXPECT_DOUBLE_EQ(b.getZ(), 3.0);
}

TEST(VectorTest, MoveConstructor) {
  Vector a(1, 2, 3);
  Vector b(std::move(a));
  EXPECT_DOUBLE_EQ(b.getX(), 1.0);
  EXPECT_DOUBLE_EQ(b.getY(), 2.0);
  EXPECT_DOUBLE_EQ(b.getZ(), 3.0);
}

// Edge cases
TEST(VectorTest, AngleWithZeroVector) {
  Vector a(1, 0, 0);
  Vector zero(0, 0, 0);
  // Should return 0 when one vector is zero
  EXPECT_DOUBLE_EQ(a.angleVectors(zero), 0.0);
}

TEST(VectorTest, SetLengthOnZeroVector) {
  Vector v(0, 0, 0);
  v.setVectorLength(5.0);
  // Should remain zero (can't set length of zero vector)
  EXPECT_DOUBLE_EQ(v.getLengthVector(), 0.0);
}

// Output stream operator
TEST(VectorTest, OutputOperator) {
  Vector v(1.5, 2.5, 3.5);
  std::ostringstream oss;
  oss << v;
  EXPECT_EQ(oss.str(), "(1.5, 2.5, 3.5)");
}
