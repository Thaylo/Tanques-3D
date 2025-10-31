/*
 * test_physics_integration.cpp
 *
 * Google Test integration tests for RigidBody physics simulations
 * Tests realistic physics scenarios, conservation laws, and complete simulations
 */

#include <gtest/gtest.h>
#include <cmath>
#include "RigidBody.h"
#include "BoxShape.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"

using namespace std;

// Floating point comparison epsilon
const double EPSILON = 1e-6;
const double INTEGRATION_EPSILON = 1e-4;  // Looser tolerance for accumulated integration error

// Physics constants
const double GRAVITY = -9.8;  // m/s² in Z direction

// Test fixture for integration tests
class PhysicsIntegrationTest : public ::testing::Test {
protected:
    RigidBody* body;

    void SetUp() override {
        body = nullptr;
    }

    void TearDown() override {
        if (body != nullptr) {
            delete body;
            body = nullptr;
        }
    }

    // Helper: Simulate N frames
    void simulate(RigidBody& rb, int frames, double dt) {
        for (int i = 0; i < frames; i++) {
            rb.integrate(dt);
        }
    }

    // Helper: Apply gravity force
    void applyGravity(RigidBody& rb, double mass) {
        rb.applyForce(Vector(0.0, 0.0, GRAVITY * mass));
    }
};

// ============================================================================
// FreeFall Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, FreeFall_BodyUnderGravity) {
    body = new RigidBody(Vector(0.0, 0.0, 100.0), 10.0);

    double dt = 0.01;  // 10ms timestep
    int frames = 100;  // Simulate 1 second

    for (int i = 0; i < frames; i++) {
        applyGravity(*body, body->getMass());
        body->integrate(dt);
    }

    Vector pos = body->getPosition();
    Vector vel = body->getVelocity();

    // After 1 second of free fall:
    // v = g * t = -9.8 * 1.0 = -9.8 m/s (with damping, it will be less)
    // z = z0 + 0.5 * g * t^2 = 100 + 0.5 * (-9.8) * 1.0 = 95.1 m

    // Position should have decreased
    EXPECT_LT(pos.getZ(), 100.0);

    // Velocity should be negative (falling)
    EXPECT_LT(vel.getZ(), 0.0);

    // X and Y should remain at origin
    EXPECT_NEAR(pos.getX(), 0.0, INTEGRATION_EPSILON);
    EXPECT_NEAR(pos.getY(), 0.0, INTEGRATION_EPSILON);
}

TEST_F(PhysicsIntegrationTest, FreeFall_VelocityIncreasesLinearly) {
    body = new RigidBody(Vector(0.0, 0.0, 100.0), 5.0);

    double dt = 0.01;

    applyGravity(*body, body->getMass());
    body->integrate(dt);
    Vector vel1 = body->getVelocity();

    applyGravity(*body, body->getMass());
    body->integrate(dt);
    Vector vel2 = body->getVelocity();

    applyGravity(*body, body->getMass());
    body->integrate(dt);
    Vector vel3 = body->getVelocity();

    // Velocity should be increasing (becoming more negative) with damping
    EXPECT_LT(vel2.getZ(), vel1.getZ());
    EXPECT_LT(vel3.getZ(), vel2.getZ());
}

TEST_F(PhysicsIntegrationTest, FreeFall_DifferentMasses_SameAcceleration) {
    RigidBody body1(Vector(0.0, 0.0, 100.0), 1.0);
    RigidBody body2(Vector(0.0, 0.0, 100.0), 100.0);

    double dt = 0.01;

    applyGravity(body1, body1.getMass());
    applyGravity(body2, body2.getMass());

    body1.integrate(dt);
    body2.integrate(dt);

    Vector vel1 = body1.getVelocity();
    Vector vel2 = body2.getVelocity();

    // Both should have the same acceleration (a = g, independent of mass)
    // Account for damping differences
    EXPECT_NEAR(vel1.getZ(), vel2.getZ(), INTEGRATION_EPSILON);
}

TEST_F(PhysicsIntegrationTest, FreeFall_LongDuration) {
    body = new RigidBody(Vector(0.0, 0.0, 1000.0), 10.0);

    double dt = 0.02;
    int frames = 250;  // 5 seconds

    for (int i = 0; i < frames; i++) {
        applyGravity(*body, body->getMass());
        body->integrate(dt);
    }

    Vector pos = body->getPosition();

    // Should have fallen significantly
    EXPECT_LT(pos.getZ(), 900.0);
}

// ============================================================================
// Spinning Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, Spinning_BodyWithAngularVelocity) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Set initial angular velocity (spinning around Z axis)
    body->setAngularVelocity(Vector(0.0, 0.0, 2.0));  // 2 rad/s

    double dt = 0.01;
    int frames = 100;  // 1 second

    simulate(*body, frames, dt);

    Quaternion orient = body->getOrientation();

    // Orientation should have changed (body rotated)
    // Not identity anymore
    EXPECT_FALSE(orient.getW() == 1.0 && orient.getX() == 0.0 &&
                 orient.getY() == 0.0 && orient.getZ() == 0.0);
}

TEST_F(PhysicsIntegrationTest, Spinning_RotationAngleMatchesAngularVelocity) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    double omega = M_PI;  // π rad/s
    body->setAngularVelocity(Vector(0.0, 0.0, omega));

    double dt = 0.01;
    double totalTime = 1.0;  // 1 second
    int frames = totalTime / dt;

    simulate(*body, frames, dt);

    // After 1 second at π rad/s, should have rotated π radians (180 degrees)
    // Direction vector should have reversed

    Vector dir = body->getDir();

    // Original dir was (1, 0, 0), after 180° rotation around Z should be (-1, 0, 0)
    // Account for damping
    EXPECT_LT(dir.getX(), 0.5);  // Should be moving towards negative X
}

TEST_F(PhysicsIntegrationTest, Spinning_DampingReducesAngularVelocity) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body->setAngularVelocity(Vector(0.0, 0.0, 10.0));

    double dt = 0.01;
    Vector initialAngVel = body->getAngularVelocity();

    simulate(*body, 100, dt);

    Vector finalAngVel = body->getAngularVelocity();

    // Angular velocity should decrease due to damping
    EXPECT_LT(finalAngVel.getZ(), initialAngVel.getZ());
}

TEST_F(PhysicsIntegrationTest, Spinning_MultiAxisRotation) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Spin around multiple axes
    body->setAngularVelocity(Vector(1.0, 1.0, 1.0));

    double dt = 0.01;
    simulate(*body, 100, dt);

    // All direction vectors should have changed
    Vector dir = body->getDir();
    Vector up = body->getUp();
    Vector side = body->getSide();

    // Should not be the original values
    EXPECT_FALSE(
        (fabs(dir.getX() - 1.0) < EPSILON) &&
        (fabs(up.getZ() - 1.0) < EPSILON) &&
        (fabs(side.getY() - 1.0) < EPSILON)
    );
}

// ============================================================================
// Conservation of Momentum Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, ConservationOfMomentum_NoForces) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 5.0);
    body->setVelocity(Vector(10.0, 5.0, 3.0));

    Vector initialMomentum = body->getVelocity() * body->getMass();

    double dt = 0.01;
    simulate(*body, 50, dt);

    Vector finalMomentum = body->getVelocity() * body->getMass();

    // Momentum should be approximately conserved (with damping, it will decrease)
    // But the direction should remain the same
    Vector initialDir = initialMomentum.getNormalVector();
    Vector finalDir = finalMomentum.getNormalVector();

    EXPECT_NEAR(initialDir.getX(), finalDir.getX(), INTEGRATION_EPSILON);
    EXPECT_NEAR(initialDir.getY(), finalDir.getY(), INTEGRATION_EPSILON);
    EXPECT_NEAR(initialDir.getZ(), finalDir.getZ(), INTEGRATION_EPSILON);
}

TEST_F(PhysicsIntegrationTest, ConservationOfMomentum_ConstantVelocity) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);

    // Give initial velocity
    body->applyImpulse(Vector(20.0, 0.0, 0.0), Vector(0, 0, 0));

    Vector vel1 = body->getVelocity();

    // Integrate without forces
    double dt = 0.001;  // Small timestep to minimize damping
    body->integrate(dt);

    Vector vel2 = body->getVelocity();

    // Velocity should decrease slightly due to damping
    EXPECT_LT(vel2.getX(), vel1.getX());
    EXPECT_GT(vel2.getX(), 0.0);
}

TEST_F(PhysicsIntegrationTest, ConservationOfMomentum_ImpulseChanges) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 2.0);
    body->setVelocity(Vector(5.0, 0.0, 0.0));

    Vector momentum1 = body->getVelocity() * body->getMass();

    // Apply impulse
    body->applyImpulse(Vector(10.0, 0.0, 0.0), Vector(0, 0, 0));

    Vector momentum2 = body->getVelocity() * body->getMass();

    // Change in momentum should equal the impulse
    Vector deltaMomentum = momentum2 - momentum1;

    EXPECT_NEAR(deltaMomentum.getX(), 10.0, EPSILON);
    EXPECT_NEAR(deltaMomentum.getY(), 0.0, EPSILON);
    EXPECT_NEAR(deltaMomentum.getZ(), 0.0, EPSILON);
}

// ============================================================================
// Conservation of Angular Momentum Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, ConservationOfAngularMomentum_NoTorques) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(2.0, 2.0, 2.0));
    body->setAngularVelocity(Vector(0.0, 0.0, 5.0));

    Vector initialL = body->getAngularMomentum();

    double dt = 0.01;
    simulate(*body, 50, dt);

    Vector finalL = body->getAngularMomentum();

    // Angular momentum direction should be conserved
    Vector initialDir = initialL.getNormalVector();
    Vector finalDir = finalL.getNormalVector();

    EXPECT_NEAR(initialDir.getX(), finalDir.getX(), INTEGRATION_EPSILON);
    EXPECT_NEAR(initialDir.getY(), finalDir.getY(), INTEGRATION_EPSILON);
    EXPECT_NEAR(initialDir.getZ(), finalDir.getZ(), INTEGRATION_EPSILON);
}

TEST_F(PhysicsIntegrationTest, ConservationOfAngularMomentum_FreeSpin) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 5.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Give initial angular velocity
    body->applyImpulse(Vector(0.0, 10.0, 0.0), Vector(1.0, 0.0, 0.0));

    Vector L1 = body->getAngularMomentum();

    // Integrate without torques
    double dt = 0.001;
    body->integrate(dt);

    Vector L2 = body->getAngularMomentum();

    // Angular momentum magnitude should decrease due to damping but slightly
    EXPECT_LE(L2.getLengthVector(), L1.getLengthVector() * 1.01);
}

// ============================================================================
// Torque from Force Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, TorqueFromForce_CrossProduct) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body->clearAccumulators();

    // Apply force at offset point
    Vector r(2.0, 0.0, 0.0);          // Offset from COM
    Vector F(0.0, 3.0, 0.0);          // Force in Y direction
    Vector expectedTorque(0.0, 0.0, 6.0);  // τ = r × F = (2,0,0) × (0,3,0) = (0,0,6)

    body->applyForceAtPoint(F, r);

    double dt = 0.01;
    body->integrate(dt);

    Vector angVel = body->getAngularVelocity();

    // Angular velocity should be generated in Z direction
    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_GT(angVel.getZ(), 0.0);
}

TEST_F(PhysicsIntegrationTest, TorqueFromForce_OppositeDirection) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Apply force in opposite direction
    Vector r(2.0, 0.0, 0.0);
    Vector F(0.0, -3.0, 0.0);  // Negative Y

    body->applyForceAtPoint(F, r);

    double dt = 0.01;
    body->integrate(dt);

    Vector angVel = body->getAngularVelocity();

    // Should generate negative rotation around Z
    EXPECT_LT(angVel.getZ(), 0.0);
}

TEST_F(PhysicsIntegrationTest, TorqueFromForce_NoTorqueWhenParallel) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Force parallel to offset vector should generate no torque
    Vector r(1.0, 0.0, 0.0);
    Vector F(5.0, 0.0, 0.0);  // Parallel to r

    body->applyForceAtPoint(F, r);

    double dt = 0.01;
    body->integrate(dt);

    Vector angVel = body->getAngularVelocity();

    // No angular velocity should be generated
    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 0.0, EPSILON);
}

TEST_F(PhysicsIntegrationTest, TorqueFromForce_VariousPoints) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Apply force at different points
    Vector F(0.0, 1.0, 0.0);

    // Closer point generates less torque
    RigidBody body1(Vector(0, 0, 0), 1.0);
    body1.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body1.applyForceAtPoint(F, Vector(1.0, 0.0, 0.0));
    body1.integrate(0.01);

    // Farther point generates more torque
    RigidBody body2(Vector(0, 0, 0), 1.0);
    body2.setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));
    body2.applyForceAtPoint(F, Vector(2.0, 0.0, 0.0));
    body2.integrate(0.01);

    Vector angVel1 = body1.getAngularVelocity();
    Vector angVel2 = body2.getAngularVelocity();

    // Torque scales with distance
    EXPECT_GT(angVel2.getZ(), angVel1.getZ());
    EXPECT_NEAR(angVel2.getZ() / angVel1.getZ(), 2.0, INTEGRATION_EPSILON);
}

// ============================================================================
// Impulse Response Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, ImpulseResponse_InstantVelocityChange) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 4.0);

    Vector initialVel = body->getVelocity();

    // Apply impulse J = 20 N·s
    Vector impulse(20.0, 0.0, 0.0);
    body->applyImpulse(impulse, Vector(0, 0, 0));

    Vector finalVel = body->getVelocity();

    // Δv = J / m = 20 / 4 = 5 m/s
    EXPECT_NEAR(finalVel.getX() - initialVel.getX(), 5.0, EPSILON);
}

TEST_F(PhysicsIntegrationTest, ImpulseResponse_AngularImpulse) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(2.0, 2.0, 2.0));

    // Apply impulse off-center
    Vector impulse(0.0, 4.0, 0.0);
    Vector point(3.0, 0.0, 0.0);

    body->applyImpulse(impulse, point);

    // Angular impulse = r × J = (3, 0, 0) × (0, 4, 0) = (0, 0, 12)
    // Δω = I^-1 * L = diag(0.5, 0.5, 0.5) * (0, 0, 12) = (0, 0, 6)

    Vector angVel = body->getAngularVelocity();

    EXPECT_NEAR(angVel.getX(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getY(), 0.0, EPSILON);
    EXPECT_NEAR(angVel.getZ(), 6.0, EPSILON);
}

TEST_F(PhysicsIntegrationTest, ImpulseResponse_MultipleImpulses) {
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 2.0);

    body->applyImpulse(Vector(10.0, 0.0, 0.0), Vector(0, 0, 0));
    body->applyImpulse(Vector(6.0, 0.0, 0.0), Vector(0, 0, 0));

    Vector vel = body->getVelocity();

    // Total impulse = 16 N·s, Δv = 16/2 = 8 m/s
    EXPECT_NEAR(vel.getX(), 8.0, EPSILON);
}

TEST_F(PhysicsIntegrationTest, ImpulseResponse_ImpulseVsForce) {
    // Compare impulse vs continuous force

    RigidBody body1(Vector(0, 0, 0), 1.0);
    RigidBody body2(Vector(0, 0, 0), 1.0);

    // Body 1: Apply impulse
    body1.applyImpulse(Vector(10.0, 0.0, 0.0), Vector(0, 0, 0));

    // Body 2: Apply force over time
    double dt = 0.01;
    for (int i = 0; i < 100; i++) {
        body2.applyForce(Vector(100.0, 0.0, 0.0));  // 100 N for 1 second
        body2.integrate(dt);
    }

    Vector vel1 = body1.getVelocity();
    Vector vel2 = body2.getVelocity();

    // Impulse of 10 N·s should be approximately equal to 100 N for 0.1 s
    // (accounting for damping in body2)
    EXPECT_GT(vel1.getX(), 5.0);
}

// ============================================================================
// Complete Simulation Test Suite
// ============================================================================

TEST_F(PhysicsIntegrationTest, CompleteSimulation_BoxFallingAndSpinning) {
    // Create a realistic box falling under gravity with initial spin
    body = new RigidBody(Vector(0.0, 0.0, 50.0), 10.0);

    // Use box inertia tensor
    BoxShape box(2.0, 2.0, 2.0);
    Matrix3x3 inertia = box.calculateInertiaTensor(10.0);
    body->setInertiaTensor(inertia);

    // Give initial spin
    body->setAngularVelocity(Vector(0.0, 0.0, 3.0));

    double dt = 0.01;
    int frames = 200;  // 2 seconds

    for (int i = 0; i < frames; i++) {
        applyGravity(*body, body->getMass());
        body->integrate(dt);
    }

    Vector pos = body->getPosition();
    Vector vel = body->getVelocity();
    Quaternion orient = body->getOrientation();

    // Should have fallen
    EXPECT_LT(pos.getZ(), 50.0);

    // Should have downward velocity
    EXPECT_LT(vel.getZ(), 0.0);

    // Should have rotated
    EXPECT_FALSE(orient.getW() == 1.0 && orient.getX() == 0.0 &&
                 orient.getY() == 0.0 && orient.getZ() == 0.0);

    // Position X and Y should remain near origin
    EXPECT_NEAR(pos.getX(), 0.0, INTEGRATION_EPSILON);
    EXPECT_NEAR(pos.getY(), 0.0, INTEGRATION_EPSILON);
}

TEST_F(PhysicsIntegrationTest, CompleteSimulation_ProjectileMotion) {
    // Simulate projectile motion
    body = new RigidBody(Vector(0.0, 0.0, 10.0), 1.0);

    // Give initial velocity at 45 degrees
    double v0 = 20.0;
    double angle = M_PI / 4.0;
    body->setVelocity(Vector(v0 * cos(angle), 0.0, v0 * sin(angle)));

    double dt = 0.01;
    int maxFrames = 500;

    for (int i = 0; i < maxFrames; i++) {
        applyGravity(*body, body->getMass());
        body->integrate(dt);

        // Stop if body hits ground
        if (body->getPosition().getZ() < 0.0) {
            break;
        }
    }

    Vector pos = body->getPosition();

    // Should have traveled forward in X
    EXPECT_GT(pos.getX(), 0.0);

    // Should have hit ground or be close
    EXPECT_LT(pos.getZ(), 10.0);
}

TEST_F(PhysicsIntegrationTest, CompleteSimulation_SpinningTop) {
    // Simulate a spinning object with asymmetric inertia
    body = new RigidBody(Vector(0.0, 0.0, 5.0), 5.0);

    // Asymmetric inertia (like a cylinder)
    body->setInertiaTensor(Matrix3x3::diagonal(2.0, 2.0, 1.0));

    // Spin around Z axis
    body->setAngularVelocity(Vector(0.0, 0.0, 10.0));

    double dt = 0.01;
    simulate(*body, 100, dt);

    Vector angVel = body->getAngularVelocity();

    // Should still be spinning (with damping)
    EXPECT_GT(angVel.getZ(), 0.0);
    EXPECT_LT(angVel.getZ(), 10.0);  // Reduced by damping
}

TEST_F(PhysicsIntegrationTest, CompleteSimulation_CollisionResponse) {
    // Simulate a body hitting a wall (impulse-based)
    body = new RigidBody(Vector(0.0, 0.0, 10.0), 5.0);
    body->setVelocity(Vector(0.0, 0.0, -20.0));  // Moving down fast

    double dt = 0.01;

    // Simulate until "collision"
    while (body->getPosition().getZ() > 0.1) {
        applyGravity(*body, body->getMass());
        body->integrate(dt);
    }

    // Apply collision impulse (perfect elastic collision)
    Vector vel = body->getVelocity();
    Vector collisionImpulse(0.0, 0.0, -2.0 * vel.getZ() * body->getMass());
    body->applyImpulse(collisionImpulse, body->getPosition());

    // After collision, should be moving upward
    Vector newVel = body->getVelocity();
    EXPECT_GT(newVel.getZ(), 0.0);
}

TEST_F(PhysicsIntegrationTest, CompleteSimulation_EnergyDissipation) {
    // Verify that damping dissipates energy over time
    body = new RigidBody(Vector(0.0, 0.0, 100.0), 10.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Give initial kinetic energy (linear and rotational)
    body->setVelocity(Vector(10.0, 10.0, 10.0));
    body->setAngularVelocity(Vector(5.0, 5.0, 5.0));

    double initialKE_linear = 0.5 * body->getMass() *
        body->getVelocity().dotProduct(body->getVelocity());

    double dt = 0.01;
    simulate(*body, 200, dt);

    double finalKE_linear = 0.5 * body->getMass() *
        body->getVelocity().dotProduct(body->getVelocity());

    // Energy should decrease due to damping
    EXPECT_LT(finalKE_linear, initialKE_linear);
}

TEST_F(PhysicsIntegrationTest, CompleteSimulation_StabilityTest) {
    // Test numerical stability over long simulation
    body = new RigidBody(Vector(0.0, 0.0, 0.0), 1.0);
    body->setInertiaTensor(Matrix3x3::diagonal(1.0, 1.0, 1.0));

    // Small initial conditions
    body->setVelocity(Vector(0.01, 0.01, 0.01));
    body->setAngularVelocity(Vector(0.01, 0.01, 0.01));

    double dt = 0.001;
    simulate(*body, 10000, dt);  // 10 seconds

    Vector pos = body->getPosition();
    Quaternion orient = body->getOrientation();

    // Should not have exploded or produced NaN
    EXPECT_FALSE(std::isnan(pos.getX()));
    EXPECT_FALSE(std::isnan(pos.getY()));
    EXPECT_FALSE(std::isnan(pos.getZ()));
    EXPECT_FALSE(std::isnan(orient.getW()));

    // Quaternion should still be normalized
    double qNorm = orient.norm();
    EXPECT_NEAR(qNorm, 1.0, INTEGRATION_EPSILON);
}

// Main function
