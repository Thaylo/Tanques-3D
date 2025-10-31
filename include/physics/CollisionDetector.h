/*
 * CollisionDetector.h
 *
 *  Created on: 2025-01-31
 *
 *  Narrow-phase collision detection for different shape pairs.
 *  Uses specialized algorithms for each shape combination.
 */

#ifndef COLLISIONDETECTOR_H_
#define COLLISIONDETECTOR_H_

#include "Vector.h"
#include "Shape.h"
#include <vector>

namespace std {

// Forward declarations
class RigidBody;
class SphereShape;
class BoxShape;
class CylinderShape;
class PyramidShape;

// Contact point information for collision response
struct Contact {
    Vector point;           // Contact point in world space
    Vector normal;          // Contact normal (from bodyA to bodyB)
    double penetration;     // Penetration depth (positive = overlap)
    RigidBody* bodyA;       // First body in collision
    RigidBody* bodyB;       // Second body in collision

    // Material properties (for Phase 4 - collision response)
    double restitution;     // Coefficient of restitution (bounciness)
    double friction;        // Coefficient of friction

    Contact()
        : point(0, 0, 0), normal(0, 0, 1), penetration(0.0),
          bodyA(nullptr), bodyB(nullptr),
          restitution(0.5), friction(0.3) {}
};

// Collision manifold - multiple contact points for stable collision
struct CollisionManifold {
    vector<Contact> contacts;   // Contact points
    Vector normal;              // Average contact normal
    double penetration;         // Maximum penetration depth

    CollisionManifold() : normal(0, 0, 1), penetration(0.0) {}

    void addContact(const Contact& contact) {
        contacts.push_back(contact);
        if (contact.penetration > penetration) {
            penetration = contact.penetration;
            normal = contact.normal;
        }
    }

    bool hasContacts() const {
        return !contacts.empty();
    }

    int getContactCount() const {
        return contacts.size();
    }
};

// Main collision detector class
class CollisionDetector {
public:
    // Sphere-Sphere collision (simplest case)
    static bool detectSphereSphere(
        const SphereShape* sphereA, const Vector& posA,
        const SphereShape* sphereB, const Vector& posB,
        Contact& contact);

    // Sphere-Box collision
    static bool detectSphereBox(
        const SphereShape* sphere, const Vector& spherePos,
        const BoxShape* box, const Vector& boxPos, const class Quaternion& boxOrient,
        Contact& contact);

    // Sphere-Cylinder collision
    static bool detectSphereCylinder(
        const SphereShape* sphere, const Vector& spherePos,
        const CylinderShape* cylinder, const Vector& cylPos, const class Quaternion& cylOrient,
        Contact& contact);

    // Sphere-Pyramid collision
    static bool detectSpherePyramid(
        const SphereShape* sphere, const Vector& spherePos,
        const PyramidShape* pyramid, const Vector& pyrPos, const class Quaternion& pyrOrient,
        Contact& contact);

    // Box-Box collision (SAT algorithm)
    static bool detectBoxBox(
        const BoxShape* boxA, const Vector& posA, const class Quaternion& orientA,
        const BoxShape* boxB, const Vector& posB, const class Quaternion& orientB,
        CollisionManifold& manifold);

    // Generic shape pair detection (dispatches to specific method)
    static bool detectCollision(
        RigidBody* bodyA, RigidBody* bodyB,
        Contact& contact);

    static bool detectCollision(
        RigidBody* bodyA, RigidBody* bodyB,
        CollisionManifold& manifold);

private:
    // Helper functions
    static Vector closestPointOnBox(
        const Vector& point,
        const BoxShape* box,
        const Vector& boxPos,
        const class Quaternion& boxOrient);

    static Vector closestPointOnCylinder(
        const Vector& point,
        const CylinderShape* cylinder,
        const Vector& cylPos,
        const class Quaternion& cylOrient);

    static double pointToLineSegmentDistance(
        const Vector& point,
        const Vector& lineStart,
        const Vector& lineEnd,
        Vector& closestPoint);
};

} /* namespace std */

#endif /* COLLISIONDETECTOR_H_ */
