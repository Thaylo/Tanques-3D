/*
 * RigidBody.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "RigidBody.h"
#include "Shape.h"
#include "Constants.h"
#include <cmath>

namespace std {

RigidBody::RigidBody() : Matter(), Movable() {
    orientation = Quaternion::identity();
    angularVelocity = Vector(0, 0, 0);
    angularMomentum = Vector(0, 0, 0);

    // Default inertia tensor for unit sphere
    double I = 0.4 * getMass();  // I = 2/5 * m * r^2, assuming r=1
    inertiaTensor = Matrix3x3::diagonal(I, I, I);
    inertiaTensorInv = Matrix3x3::diagonal(1.0/I, 1.0/I, 1.0/I);

    forceAccumulator = Vector(0, 0, 0);
    torqueAccumulator = Vector(0, 0, 0);

    linearDamping = 0.95;   // 5% damping per second
    angularDamping = 0.90;  // 10% damping per second

    shape = nullptr;  // No shape by default
}

RigidBody::RigidBody(const Vector &pos, double mass) : RigidBody() {
    position = pos;
    setMass(mass);

    // Update inertia tensor for new mass
    double I = 0.4 * mass;
    inertiaTensor = Matrix3x3::diagonal(I, I, I);
    inertiaTensorInv = Matrix3x3::diagonal(1.0/I, 1.0/I, 1.0/I);
}

// Setup methods
void RigidBody::setInertiaTensor(const Matrix3x3 &tensor) {
    inertiaTensor = tensor;
    inertiaTensorInv = tensor.inverse();
}

void RigidBody::setOrientation(const Quaternion &q) {
    orientation = q.normalize();
}

void RigidBody::setOrientation(double roll, double pitch, double yaw) {
    orientation = Quaternion::fromEuler(roll, pitch, yaw);
}

void RigidBody::setAngularVelocity(const Vector &omega) {
    angularVelocity = omega;
    // Update angular momentum to be consistent: L = I * ω
    Matrix3x3 I = getInertiaTensorWorld();
    angularMomentum = I * omega;
}

void RigidBody::setShape(Shape* s) {
    shape = s;
    // Optionally update inertia tensor if shape is set
    if (shape != nullptr) {
        setInertiaTensor(shape->calculateInertiaTensor(getMass()));
    }
}

AABB RigidBody::getAABB() const {
    if (shape == nullptr) {
        // Return a small AABB around position if no shape
        double epsilon = 0.1;
        return AABB(
            Vector(position.getX() - epsilon, position.getY() - epsilon, position.getZ() - epsilon),
            Vector(position.getX() + epsilon, position.getY() + epsilon, position.getZ() + epsilon)
        );
    }
    return shape->getAABB(position, orientation);
}

// Override Movable getters to use quaternion-based orientation
Vector RigidBody::getUp() const {
    return orientation.rotate(Vector(0, 0, 1));
}

Vector RigidBody::getDir() const {
    return orientation.rotate(Vector(1, 0, 0));
}

Vector RigidBody::getSide() const {
    return orientation.rotate(Vector(0, 1, 0));
}

// Helper methods
Matrix3x3 RigidBody::getInertiaTensorWorld() const {
    // Transform inertia tensor from body space to world space
    // I_world = R * I_body * R^T
    Matrix3x3 R = orientation.toRotationMatrix();
    return R * inertiaTensor * R.transpose();
}

Matrix3x3 RigidBody::getInertiaTensorInvWorld() const {
    // Transform inverse inertia tensor from body space to world space
    Matrix3x3 R = orientation.toRotationMatrix();
    return R * inertiaTensorInv * R.transpose();
}

// Force and torque application
void RigidBody::clearAccumulators() {
    forceAccumulator = Vector(0, 0, 0);
    torqueAccumulator = Vector(0, 0, 0);
}

void RigidBody::applyForce(const Vector &force) {
    forceAccumulator = forceAccumulator + force;
}

void RigidBody::applyForceAtPoint(const Vector &force, const Vector &point) {
    // Apply force and calculate resulting torque
    forceAccumulator = forceAccumulator + force;

    // Torque = r × F, where r is the vector from center of mass to point
    Vector r = point - position;
    Vector torque = r.crossProduct(force);
    torqueAccumulator = torqueAccumulator + torque;
}

void RigidBody::applyTorque(const Vector &torque) {
    torqueAccumulator = torqueAccumulator + torque;
}

void RigidBody::applyImpulse(const Vector &impulse, const Vector &point) {
    // Apply instantaneous change in momentum
    // Δv = impulse / mass
    velocity = velocity + impulse * (1.0 / getMass());

    // Apply angular impulse
    // Δω = I^-1 * (r × impulse)
    Vector r = point - position;
    Vector angularImpulse = r.crossProduct(impulse);
    Matrix3x3 IInv = getInertiaTensorInvWorld();
    angularVelocity = angularVelocity + IInv * angularImpulse;
}

// Physics integration
void RigidBody::integrate(double dt) {
    // Linear motion integration
    // a = F / m
    Vector acceleration = forceAccumulator * (1.0 / getMass());

    // v(t+dt) = v(t) + a * dt
    velocity = velocity + acceleration * dt;

    // Apply linear damping
    velocity = velocity * pow(linearDamping, dt);

    // x(t+dt) = x(t) + v * dt
    position = position + velocity * dt;

    // Rotational motion integration
    // τ = dL/dt, so L(t+dt) = L(t) + τ * dt
    angularMomentum = angularMomentum + torqueAccumulator * dt;

    // ω = I^-1 * L
    Matrix3x3 IInv = getInertiaTensorInvWorld();
    angularVelocity = IInv * angularMomentum;

    // Apply angular damping
    angularVelocity = angularVelocity * pow(angularDamping, dt);

    // Integrate orientation: q(t+dt) = q(t) + 0.5 * [ω * q] * dt
    orientation = orientation.integrate(angularVelocity, dt);

    // Clear accumulators for next frame
    clearAccumulators();
}

void RigidBody::iterate() {
    // Override Movable::iterate() to use RigidBody physics
    // Use TIME_STEP from Constants.h
    double dt = TIME_STEP / 1000.0;  // Convert to seconds

    integrate(dt);
}

// Utilities
Vector RigidBody::getPointVelocity(const Vector &point) const {
    // v_point = v_cm + ω × r
    Vector r = point - position;
    return velocity + angularVelocity.crossProduct(r);
}

Vector RigidBody::localToWorld(const Vector &local) const {
    return position + orientation.rotate(local);
}

Vector RigidBody::worldToLocal(const Vector &world) const {
    Vector relative = world - position;
    return orientation.inverse().rotate(relative);
}

} /* namespace std */
