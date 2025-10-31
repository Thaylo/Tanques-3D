/*
 * RigidBody.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Rigid body physics - extends Matter to include rotational dynamics
 */

#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

#include "Matter.h"
#include "Movable.h"
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"

namespace std {

class RigidBody : public Matter, public Movable {
protected:
    // Rotational state
    Quaternion orientation;      // Orientation as quaternion (replaces roll, pitch, yaw)
    Vector angularVelocity;      // Angular velocity vector (rad/s)
    Vector angularMomentum;      // Angular momentum

    // Inertia properties (in body space)
    Matrix3x3 inertiaTensor;     // Inertia tensor (3x3 matrix)
    Matrix3x3 inertiaTensorInv;  // Inverse inertia tensor (for efficiency)

    // Force and torque accumulators
    Vector forceAccumulator;
    Vector torqueAccumulator;

    // Damping factors
    double linearDamping;
    double angularDamping;

    // Helper methods
    Matrix3x3 getInertiaTensorWorld() const;      // Transform inertia tensor to world space
    Matrix3x3 getInertiaTensorInvWorld() const;   // Inverse inertia tensor in world space

public:
    RigidBody();
    RigidBody(const Vector &position, double mass);

    // Setup methods
    void setInertiaTensor(const Matrix3x3 &tensor);
    void setOrientation(const Quaternion &q);
    void setOrientation(double roll, double pitch, double yaw);  // From Euler angles
    void setAngularVelocity(const Vector &omega);

    // Getters
    Quaternion getOrientation() const { return orientation; }
    Vector getAngularVelocity() const { return angularVelocity; }
    Vector getAngularMomentum() const { return angularMomentum; }
    Matrix3x3 getInertiaTensor() const { return inertiaTensor; }

    // Override Movable getters to use quaternion-based orientation
    Vector getUp() const;
    Vector getDir() const;
    Vector getSide() const;

    // Force and torque application
    void clearAccumulators();
    void applyForce(const Vector &force);                          // Apply force at center of mass
    void applyForceAtPoint(const Vector &force, const Vector &point);  // Apply force at a point (generates torque)
    void applyTorque(const Vector &torque);
    void applyImpulse(const Vector &impulse, const Vector &point);     // Apply instantaneous impulse

    // Physics integration
    virtual void integrate(double dt);
    virtual void iterate();  // Override Movable::iterate() to use new physics

    // Utilities
    Vector getPointVelocity(const Vector &point) const;  // Get velocity of a point on the body
    Vector localToWorld(const Vector &local) const;      // Transform local coordinates to world
    Vector worldToLocal(const Vector &world) const;      // Transform world coordinates to local

    virtual ~RigidBody() {}
};

} /* namespace std */

#endif /* RIGIDBODY_H_ */
