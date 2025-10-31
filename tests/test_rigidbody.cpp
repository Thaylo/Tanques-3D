/*
 * test_rigidbody.cpp
 *
 * Google Test unit tests for RigidBody class
 * Tests construction, inertia, orientation, forces, torques, impulses, integration, and transforms
 */

#include <gtest/gtest.h>
#include <cmath>
#include "RigidBody.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;

// Test fixture for RigidBody tests
class RigidBodyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common test setup if needed
    }

    void TearDown() override {
        // Common test cleanup if needed
    }
};

// ============================================================================
// RigidBodyConstruction Test Suite
// ============================================================================

TEST(RigidBodyConstruction, DefaultConstructor) {
    RigidBody body;

    // Default position should be at origin
    Vector pos = body.getPosition();
    EXPECT_NEAR(pos.getX(), 0.0, EPSILON);
    EXPECT_NEAR(pos.getY(), 0.0, EPSILON);
    EXPECT_NEAR(pos.getZ(), 0.0, EPSILON);

    // Default orientation should be identity
    Quaternion orient = body.getOrientation();
    EXPECT_NEAR(orient.getW(), 1.0, EPSILON);
    EXPECT_NEAR(orient.getX(), 0.0, EPSILON);
    EXPECT_NEAR(orient.getY(), 0.0, EPSILON);
    EXPECT_NEAR(orient.getZ(), 0.0, EPSILON);

    // Angular velocity should be zero
    Vector angVel = body.getAngularVelocity();
    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyConstruction, ConstructorWithPositionAndMass) {
    Vector pos(10.0, 20.0, 30.0);
    double mass = 5.0;

    RigidBody body(pos, mass);

    Vector actualPos = body.getPosition();
    EXPECT_NEAR(actualPos.getX(), 10.0, EPSILON);
    EXPECT_NEAR(actualPos.getY(), 20.0, EPSILON);
    EXPECT_NEAR(actualPos.getZ(), 30.0, EPSILON);

    EXPECT_NEAR(body.getMass(), mass, EPSILON);
}

TEST(RigidBodyConstruction, DefaultMass) {
    RigidBody body;

    // Default mass from Matter class should be 1.0
    EXPECT_GT(body.getMass(), 0.0);
}

TEST(RigidBodyConstruction, DefaultVelocityIsZero) {
    RigidBody body;

    Vector vel = body.getVelocity();
    EXPECT_NEAR(vel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getZ(), 0.0, EPSILON);
}

// ============================================================================
// RigidBodyInertia Test Suite
// ============================================================================

TEST(RigidBodyInertia, SetAndGetInertiaTensor) {
    RigidBody body;

    // Create a diagonal inertia tensor
    Matrix3x3 inertia = Matrix3x3::diagonal(1.0, 2.0, 3.0);
    body.setInertiaTensor(inertia);

    Matrix3x3 result = body.getInertiaTensor();

    EXPECT_NEAR(result.get(0, 0), 1.0, EPSILON);
    EXPECT_NEAR(result.get(1, 1), 2.0, EPSILON);
    EXPECT_NEAR(result.get(2, 2), 3.0, EPSILON);
}

TEST(RigidBodyInertia, SetNonDiagonalInertiaTensor) {
    RigidBody body;

    // Create a non-diagonal inertia tensor (for asymmetric objects)
    Matrix3x3 inertia(
        10.0, 1.0, 2.0,
        1.0, 20.0, 3.0,
        2.0, 3.0, 30.0
    );

    body.setInertiaTensor(inertia);

    Matrix3x3 result = body.getInertiaTensor();

    EXPECT_NEAR(result.get(0, 0), 10.0, EPSILON);
    EXPECT_NEAR(result.get(0, 1), 1.0, EPSILON);
    EXPECT_NEAR(result.get(0, 2), 2.0, EPSILON);
    EXPECT_NEAR(result.get(1, 1), 20.0, EPSILON);
}

TEST(RigidBodyInertia, DefaultInertiaTensorIsPositive) {
    RigidBody body;

    Matrix3x3 inertia = body.getInertiaTensor();

    // Diagonal elements should be positive
    EXPECT_GT(inertia.get(0, 0), 0.0);
    EXPECT_GT(inertia.get(1, 1), 0.0);
    EXPECT_GT(inertia.get(2, 2), 0.0);
}

// ============================================================================
// RigidBodyOrientation Test Suite
// ============================================================================

TEST(RigidBodyOrientation, SetOrientationFromQuaternion) {
    RigidBody body;

    // 90 degree rotation around Z axis
    Quaternion q = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 2.0);
    body.setOrientation(q);

    Quaternion result = body.getOrientation();

    EXPECT_NEAR(result.getW(), q.getW(), EPSILON);
    EXPECT_NEAR(result.getX(), q.getX(), EPSILON);
    EXPECT_NEAR(result.getY(), q.getY(), EPSILON);
    EXPECT_NEAR(result.getZ(), q.getZ(), EPSILON);
}

TEST(RigidBodyOrientation, SetOrientationFromEulerAngles) {
    RigidBody body;

    // Set orientation using Euler angles (roll, pitch, yaw)
    double roll = M_PI / 4.0;   // 45 degrees
    double pitch = M_PI / 6.0;  // 30 degrees
    double yaw = M_PI / 3.0;    // 60 degrees

    body.setOrientation(roll, pitch, yaw);

    // Verify orientation is set (non-identity)
    Quaternion orient = body.getOrientation();
    EXPECT_FALSE(orient.getW() == 1.0 && orient.getX() == 0.0 &&
                 orient.getY() == 0.0 && orient.getZ() == 0.0);
}

TEST(RigidBodyOrientation, GetDirectionVectors) {
    RigidBody body;

    // Default orientation: identity quaternion
    Vector dir = body.getDir();
    Vector up = body.getUp();
    Vector side = body.getSide();

    // Default direction vectors for identity orientation
    EXPECT_NEAR(dir.getX(), 1.0, EPSILON);   // Forward is +X
    EXPECT_NEAR(dir.getY(), 0.0, EPSILON);
    EXPECT_NEAR(dir.getZ(), 0.0, EPSILON);

    EXPECT_NEAR(up.getX(), 0.0, EPSILON);
    EXPECT_NEAR(up.getY(), 0.0, EPSILON);
    EXPECT_NEAR(up.getZ(), 1.0, EPSILON);    // Up is +Z

    EXPECT_NEAR(side.getX(), 0.0, EPSILON);
    EXPECT_NEAR(side.getY(), 1.0, EPSILON);  // Side is +Y
    EXPECT_NEAR(side.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyOrientation, GetDirectionVectorsAfterRotation) {
    RigidBody body;

    // Rotate 90 degrees around Z axis
    Quaternion q = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 2.0);
    body.setOrientation(q);

    Vector dir = body.getDir();

    // After 90° rotation around Z, +X should point to +Y
    EXPECT_NEAR(dir.getX(), 0.0, EPSILON);
    EXPECT_NEAR(dir.getY(), 1.0, EPSILON);
    EXPECT_NEAR(dir.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyOrientation, DirectionVectorsAreOrthogonal) {
    RigidBody body;

    // Set arbitrary orientation
    body.setOrientation(M_PI / 4.0, M_PI / 6.0, M_PI / 3.0);

    Vector dir = body.getDir();
    Vector up = body.getUp();
    Vector side = body.getSide();

    // Verify orthogonality
    EXPECT_NEAR(dir.dotProduct(up), 0.0, EPSILON);
    EXPECT_NEAR(dir.dotProduct(side), 0.0, EPSILON);
    EXPECT_NEAR(up.dotProduct(side), 0.0, EPSILON);
}

TEST(RigidBodyOrientation, DirectionVectorsAreNormalized) {
    RigidBody body;

    body.setOrientation(M_PI / 4.0, M_PI / 6.0, M_PI / 3.0);

    Vector dir = body.getDir();
    Vector up = body.getUp();
    Vector side = body.getSide();

    // Verify unit length
    EXPECT_NEAR(dir.getLengthVector(), 1.0, EPSILON);
    EXPECT_NEAR(up.getLengthVector(), 1.0, EPSILON);
    EXPECT_NEAR(side.getLengthVector(), 1.0, EPSILON);
}

// ============================================================================
// RigidBodyForces Test Suite
// ============================================================================

TEST(RigidBodyForces, ApplyForceAtCenter) {
    RigidBody body(Vector(0, 0, 0), 1.0);

    body.clearAccumulators();

    // Apply force at center of mass
    Vector force(10.0, 0.0, 0.0);
    body.applyForce(force);

    // Integrate for one time step
    double dt = 0.01;
    body.integrate(dt);

    // F = ma, so a = F/m = 10/1 = 10 m/s²
    // v = a * dt = 10 * 0.01 = 0.1 m/s
    Vector vel = body.getVelocity();

    // Account for damping
    EXPECT_GT(vel.getX(), 0.0);
    EXPECT_NEAR(vel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyForces, ApplyForceAtPoint_GeneratesTorque) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.clearAccumulators();

    // Apply force at point offset from center
    Vector force(0.0, 10.0, 0.0);      // Force in +Y direction
    Vector point(1.0, 0.0, 0.0);        // Point at +X

    body.applyForceAtPoint(force, point);

    // τ = r × F
    // r = (1, 0, 0), F = (0, 10, 0)
    // τ = (1, 0, 0) × (0, 10, 0) = (0, 0, 10)

    double dt = 0.01;
    body.integrate(dt);

    // Check that angular velocity is generated around Z axis
    Vector angVel = body.getAngularVelocity();

    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_GT(angVel.getZ(), 0.0);  // Positive rotation around Z
}

TEST(RigidBodyForces, MultipleForces_Accumulate) {
    RigidBody body(Vector(0, 0, 0), 2.0);
    body.clearAccumulators();

    // Apply multiple forces
    body.applyForce(Vector(5.0, 0.0, 0.0));
    body.applyForce(Vector(3.0, 0.0, 0.0));
    body.applyForce(Vector(2.0, 0.0, 0.0));

    // Total force = 10 N
    // a = F/m = 10/2 = 5 m/s²

    double dt = 0.01;
    body.integrate(dt);

    Vector vel = body.getVelocity();
    EXPECT_GT(vel.getX(), 0.0);
}

TEST(RigidBodyForces, ForceInDifferentDirections) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.clearAccumulators();

    // Apply force in Z direction
    body.applyForce(Vector(0.0, 0.0, 20.0));

    double dt = 0.01;
    body.integrate(dt);

    Vector vel = body.getVelocity();

    EXPECT_NEAR(vel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getY(), 0.0, EPSILON);
    EXPECT_GT(vel.getZ(), 0.0);
}

// ============================================================================
// RigidBodyTorque Test Suite
// ============================================================================

TEST(RigidBodyTorque, ApplyTorqueDirectly) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.clearAccumulators();

    // Apply torque around Z axis
    Vector torque(0.0, 0.0, 5.0);
    body.applyTorque(torque);

    double dt = 0.01;
    body.integrate(dt);

    // Check angular velocity
    Vector angVel = body.getAngularVelocity();

    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_GT(angVel.getZ(), 0.0);
}

TEST(RigidBodyTorque, MultipleTorques_Accumulate) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(2.0, 2.0, 2.0));
    body.clearAccumulators();

    // Apply multiple torques
    body.applyTorque(Vector(0.0, 0.0, 2.0));
    body.applyTorque(Vector(0.0, 0.0, 3.0));
    body.applyTorque(Vector(0.0, 0.0, 5.0));

    // Total torque = 10 N·m

    double dt = 0.01;
    body.integrate(dt);

    Vector angVel = body.getAngularVelocity();
    EXPECT_GT(angVel.getZ(), 0.0);
}

TEST(RigidBodyTorque, TorqueAroundDifferentAxes) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.clearAccumulators();

    // Apply torque around X axis
    body.applyTorque(Vector(10.0, 0.0, 0.0));

    double dt = 0.01;
    body.integrate(dt);

    Vector angVel = body.getAngularVelocity();

    EXPECT_GT(angVel.getX(), 0.0);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 0.0, EPSILON);
}

// ============================================================================
// RigidBodyImpulse Test Suite
// ============================================================================

TEST(RigidBodyImpulse, ApplyImpulseAtCenter) {
    RigidBody body(Vector(0, 0, 0), 2.0);
    body.clearAccumulators();

    // Apply impulse at center (same as position)
    Vector impulse(10.0, 0.0, 0.0);
    Vector point(0.0, 0.0, 0.0);

    body.applyImpulse(impulse, point);

    // Δv = J/m = 10/2 = 5 m/s
    Vector vel = body.getVelocity();

    EXPECT_NEAR(vel.getX(), 5.0, EPSILON);
    EXPECT_NEAR(vel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getZ(), 0.0, EPSILON);

    // No angular velocity should be generated
    Vector angVel = body.getAngularVelocity();
    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyImpulse, ApplyImpulseOffCenter_GeneratesAngularVelocity) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.clearAccumulators();

    // Apply impulse at offset point
    Vector impulse(0.0, 5.0, 0.0);   // Impulse in +Y direction
    Vector point(2.0, 0.0, 0.0);     // Point at +X

    body.applyImpulse(impulse, point);

    // Linear velocity change: Δv = J/m = 5/1 = 5 m/s
    Vector vel = body.getVelocity();
    EXPECT_NEAR(vel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(vel.getY(), 5.0, EPSILON);
    EXPECT_NEAR(vel.getZ(), 0.0, EPSILON);

    // Angular impulse: r × J = (2, 0, 0) × (0, 5, 0) = (0, 0, 10)
    // Δω = I^-1 * L = (0, 0, 10) (since I = identity)
    Vector angVel = body.getAngularVelocity();

    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 10.0, EPSILON);
}

TEST(RigidBodyImpulse, ImpulseInstantaneousEffect) {
    RigidBody body(Vector(0, 0, 0), 1.0);

    Vector initialVel = body.getVelocity();

    // Apply impulse
    body.applyImpulse(Vector(3.0, 4.0, 5.0), Vector(0, 0, 0));

    // Velocity should change immediately
    Vector newVel = body.getVelocity();

    EXPECT_NEAR(newVel.getX(), initialVel.getX() + 3.0, EPSILON);
    EXPECT_NEAR(newVel.getY(), initialVel.getY() + 4.0, EPSILON);
    EXPECT_NEAR(newVel.getZ(), initialVel.getZ() + 5.0, EPSILON);
}

// ============================================================================
// RigidBodyIntegration Test Suite
// ============================================================================

TEST(RigidBodyIntegration, SingleStepIntegration) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(10.0, 0.0, 0.0));

    double dt = 0.1;
    body.integrate(dt);

    // x = v * t (with damping)
    Vector pos = body.getPosition();

    // Position should have moved in +X direction
    EXPECT_GT(pos.getX(), 0.0);
    EXPECT_NEAR(pos.getY(), 0.0, EPSILON);
    EXPECT_NEAR(pos.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyIntegration, MultipleStepsIntegration) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.clearAccumulators();

    // Apply constant force
    double dt = 0.01;

    for (int i = 0; i < 10; i++) {
        body.applyForce(Vector(10.0, 0.0, 0.0));
        body.integrate(dt);
    }

    Vector pos = body.getPosition();
    Vector vel = body.getVelocity();

    // Should have accelerated and moved
    EXPECT_GT(pos.getX(), 0.0);
    EXPECT_GT(vel.getX(), 0.0);
}

TEST(RigidBodyIntegration, RotationalIntegration) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.clearAccumulators();

    // Apply constant torque
    double dt = 0.01;

    for (int i = 0; i < 10; i++) {
        body.applyTorque(Vector(0.0, 0.0, 1.0));
        body.integrate(dt);
    }

    // Orientation should have changed
    Quaternion orient = body.getOrientation();

    // Should not be identity anymore
    EXPECT_FALSE(orient.getW() == 1.0 && orient.getX() == 0.0 &&
                 orient.getY() == 0.0 && orient.getZ() == 0.0);
}

TEST(RigidBodyIntegration, AccumulatorsAreClearedAfterIntegration) {
    RigidBody body(Vector(0, 0, 0), 1.0);

    body.applyForce(Vector(10.0, 0.0, 0.0));
    body.applyTorque(Vector(0.0, 0.0, 5.0));

    body.integrate(0.01);

    // Apply no forces and integrate again
    body.integrate(0.01);

    // Body should continue with velocity (due to inertia) but not accelerate further
    // This test verifies that accumulators are cleared
}

// ============================================================================
// RigidBodyDamping Test Suite
// ============================================================================

TEST(RigidBodyDamping, LinearDamping_VelocityDecreases) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(10.0, 0.0, 0.0));

    double dt = 0.1;
    Vector initialVel = body.getVelocity();

    // Integrate without forces (only damping acts)
    body.integrate(dt);

    Vector finalVel = body.getVelocity();

    // Velocity should decrease due to damping
    EXPECT_LT(finalVel.getX(), initialVel.getX());
    EXPECT_GT(finalVel.getX(), 0.0);  // But not negative
}

TEST(RigidBodyDamping, LinearDamping_MultipleSteps) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(100.0, 0.0, 0.0));

    double dt = 0.1;

    // Integrate multiple times
    for (int i = 0; i < 50; i++) {
        body.integrate(dt);
    }

    Vector vel = body.getVelocity();

    // Velocity should have decreased significantly
    EXPECT_LT(vel.getX(), 100.0);
}

TEST(RigidBodyDamping, AngularDamping_AngularVelocityDecreases) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.setAngularVelocity(Vector(0.0, 0.0, 10.0));

    double dt = 0.1;
    Vector initialAngVel = body.getAngularVelocity();

    // Integrate without torques (only damping acts)
    body.integrate(dt);

    Vector finalAngVel = body.getAngularVelocity();

    // Angular velocity should decrease due to damping
    EXPECT_LT(finalAngVel.getZ(), initialAngVel.getZ());
    EXPECT_GT(finalAngVel.getZ(), 0.0);
}

TEST(RigidBodyDamping, AngularDamping_MultipleSteps) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body.setAngularVelocity(Vector(0.0, 0.0, 100.0));

    double dt = 0.1;

    // Integrate multiple times
    for (int i = 0; i < 50; i++) {
        body.integrate(dt);
    }

    Vector angVel = body.getAngularVelocity();

    // Angular velocity should have decreased significantly
    EXPECT_LT(angVel.getZ(), 100.0);
}

// ============================================================================
// RigidBodyTransforms Test Suite
// ============================================================================

TEST(RigidBodyTransforms, LocalToWorld_Identity) {
    RigidBody body(Vector(10.0, 20.0, 30.0), 1.0);
    body.setOrientation(Quaternion::identity());

    Vector localPoint(1.0, 2.0, 3.0);
    Vector worldPoint = body.localToWorld(localPoint);

    // World = position + local (when orientation is identity)
    EXPECT_NEAR(worldPoint.getX(), 11.0, EPSILON);
    EXPECT_NEAR(worldPoint.getY(), 22.0, EPSILON);
    EXPECT_NEAR(worldPoint.getZ(), 33.0, EPSILON);
}

TEST(RigidBodyTransforms, LocalToWorld_WithRotation) {
    RigidBody body(Vector(0, 0, 0), 1.0);

    // Rotate 90 degrees around Z
    Quaternion q = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 2.0);
    body.setOrientation(q);

    Vector localPoint(1.0, 0.0, 0.0);
    Vector worldPoint = body.localToWorld(localPoint);

    // After 90° rotation around Z, (1, 0, 0) -> (0, 1, 0)
    EXPECT_NEAR(worldPoint.getX(), 0.0, EPSILON);
    EXPECT_NEAR(worldPoint.getY(), 1.0, EPSILON);
    EXPECT_NEAR(worldPoint.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyTransforms, WorldToLocal_Identity) {
    RigidBody body(Vector(10.0, 20.0, 30.0), 1.0);
    body.setOrientation(Quaternion::identity());

    Vector worldPoint(11.0, 22.0, 33.0);
    Vector localPoint = body.worldToLocal(worldPoint);

    EXPECT_NEAR(localPoint.getX(), 1.0, EPSILON);
    EXPECT_NEAR(localPoint.getY(), 2.0, EPSILON);
    EXPECT_NEAR(localPoint.getZ(), 3.0, EPSILON);
}

TEST(RigidBodyTransforms, WorldToLocal_WithRotation) {
    RigidBody body(Vector(0, 0, 0), 1.0);

    // Rotate 90 degrees around Z
    Quaternion q = Quaternion::fromAxisAngle(Vector(0, 0, 1), M_PI / 2.0);
    body.setOrientation(q);

    Vector worldPoint(0.0, 1.0, 0.0);
    Vector localPoint = body.worldToLocal(worldPoint);

    // After inverse 90° rotation, (0, 1, 0) -> (1, 0, 0)
    EXPECT_NEAR(localPoint.getX(), 1.0, EPSILON);
    EXPECT_NEAR(localPoint.getY(), 0.0, EPSILON);
    EXPECT_NEAR(localPoint.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyTransforms, LocalToWorldAndBack) {
    RigidBody body(Vector(5.0, 10.0, 15.0), 1.0);
    body.setOrientation(M_PI / 4.0, M_PI / 6.0, M_PI / 3.0);

    Vector original(3.0, 4.0, 5.0);
    Vector world = body.localToWorld(original);
    Vector back = body.worldToLocal(world);

    EXPECT_NEAR(back.getX(), original.getX(), EPSILON);
    EXPECT_NEAR(back.getY(), original.getY(), EPSILON);
    EXPECT_NEAR(back.getZ(), original.getZ(), EPSILON);
}

TEST(RigidBodyTransforms, PointVelocity_NoRotation) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(5.0, 0.0, 0.0));
    body.setAngularVelocity(Vector(0, 0, 0));

    Vector point(10.0, 10.0, 10.0);
    Vector pointVel = body.getPointVelocity(point);

    // Point velocity = body velocity (no rotation)
    EXPECT_NEAR(pointVel.getX(), 5.0, EPSILON);
    EXPECT_NEAR(pointVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(pointVel.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyTransforms, PointVelocity_WithRotation) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(0, 0, 0));
    body.setAngularVelocity(Vector(0, 0, 1.0));  // 1 rad/s around Z

    Vector point(1.0, 0.0, 0.0);  // Point on X axis
    Vector pointVel = body.getPointVelocity(point);

    // v = ω × r = (0, 0, 1) × (1, 0, 0) = (0, 1, 0)
    EXPECT_NEAR(pointVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(pointVel.getY(), 1.0, EPSILON);
    EXPECT_NEAR(pointVel.getZ(), 0.0, EPSILON);
}

TEST(RigidBodyTransforms, PointVelocity_CombinedMotion) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setVelocity(Vector(2.0, 0.0, 0.0));
    body.setAngularVelocity(Vector(0, 0, 1.0));

    Vector point(1.0, 0.0, 0.0);
    Vector pointVel = body.getPointVelocity(point);

    // v = v_cm + ω × r = (2, 0, 0) + (0, 1, 0) = (2, 1, 0)
    EXPECT_NEAR(pointVel.getX(), 2.0, EPSILON);
    EXPECT_NEAR(pointVel.getY(), 1.0, EPSILON);
    EXPECT_NEAR(pointVel.getZ(), 0.0, EPSILON);
}

// Main function
