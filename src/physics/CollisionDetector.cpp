/*
 * CollisionDetector.cpp
 *
 *  Created on: 2025-01-31
 *
 *  Implementation of narrow-phase collision detection algorithms
 */

#include "CollisionDetector.h"
#include "RigidBody.h"
#include "SphereShape.h"
#include "BoxShape.h"
#include "CylinderShape.h"
#include "PyramidShape.h"
#include "Quaternion.h"
#include <cmath>
#include <algorithm>

using namespace std;

//==============================================================================
// Sphere-Sphere Collision Detection
//==============================================================================

bool CollisionDetector::detectSphereSphere(
    const SphereShape* sphereA, const Vector& posA,
    const SphereShape* sphereB, const Vector& posB,
    Contact& contact) {

    // Calculate distance between centers
    Vector delta = posB - posA;
    double distanceSquared = delta.dotProduct(delta);
    double radiusSum = sphereA->getRadius() + sphereB->getRadius();
    double radiusSumSquared = radiusSum * radiusSum;

    // Check for collision (avoid sqrt if possible)
    if (distanceSquared >= radiusSumSquared) {
        return false;  // No collision
    }

    // Calculate actual distance
    double distance = sqrt(distanceSquared);

    // Handle exact overlap case (spheres at same position)
    if (distance < 1e-10) {
        // Spheres are at exactly the same position
        // Choose arbitrary normal (up direction)
        contact.normal = Vector(0, 0, 1);
        contact.penetration = radiusSum;
        contact.point = posA;  // Contact at sphere center
        return true;
    }

    // Calculate contact normal (from A to B)
    contact.normal = delta * (1.0 / distance);  // Normalize

    // Calculate penetration depth
    contact.penetration = radiusSum - distance;

    // Calculate contact point (on surface of sphere A)
    contact.point = posA + contact.normal * sphereA->getRadius();

    return true;
}

//==============================================================================
// Sphere-Box Collision Detection
//==============================================================================

Vector CollisionDetector::closestPointOnBox(
    const Vector& point,
    const BoxShape* box,
    const Vector& boxPos,
    const Quaternion& boxOrient) {

    // Transform point to box local space
    Vector localPoint = boxOrient.inverse().rotate(point - boxPos);

    // Get box half-extents
    double halfWidth = box->getWidth() * 0.5;
    double halfHeight = box->getHeight() * 0.5;
    double halfDepth = box->getDepth() * 0.5;

    // Clamp point to box bounds
    double closestX = max(-halfWidth, min(halfWidth, localPoint.getX()));
    double closestY = max(-halfHeight, min(halfHeight, localPoint.getY()));
    double closestZ = max(-halfDepth, min(halfDepth, localPoint.getZ()));

    Vector closestLocal(closestX, closestY, closestZ);

    // Transform back to world space
    return boxPos + boxOrient.rotate(closestLocal);
}

bool CollisionDetector::detectSphereBox(
    const SphereShape* sphere, const Vector& spherePos,
    const BoxShape* box, const Vector& boxPos, const Quaternion& boxOrient,
    Contact& contact) {

    // Find closest point on box to sphere center
    Vector closestPoint = closestPointOnBox(spherePos, box, boxPos, boxOrient);

    // Calculate distance from sphere center to closest point
    Vector delta = closestPoint - spherePos;
    double distanceSquared = delta.dotProduct(delta);
    double radius = sphere->getRadius();
    double radiusSquared = radius * radius;

    // Check for collision
    if (distanceSquared >= radiusSquared) {
        return false;  // No collision
    }

    double distance = sqrt(distanceSquared);

    // Handle case where sphere center is inside box
    if (distance < 1e-10) {
        // Sphere center is inside or very close to box
        // Find the closest face by checking which axis has minimum penetration

        Vector localSpherePos = boxOrient.inverse().rotate(spherePos - boxPos);
        double halfWidth = box->getWidth() * 0.5;
        double halfHeight = box->getHeight() * 0.5;
        double halfDepth = box->getDepth() * 0.5;

        // Calculate penetration depth for each axis
        double penX = halfWidth - fabs(localSpherePos.getX());
        double penY = halfHeight - fabs(localSpherePos.getY());
        double penZ = halfDepth - fabs(localSpherePos.getZ());

        // Find minimum penetration (closest face)
        Vector localNormal;
        if (penX < penY && penX < penZ) {
            // X face is closest
            localNormal = Vector(localSpherePos.getX() > 0 ? 1.0 : -1.0, 0, 0);
            contact.penetration = penX + radius;
        } else if (penY < penZ) {
            // Y face is closest
            localNormal = Vector(0, localSpherePos.getY() > 0 ? 1.0 : -1.0, 0);
            contact.penetration = penY + radius;
        } else {
            // Z face is closest
            localNormal = Vector(0, 0, localSpherePos.getZ() > 0 ? 1.0 : -1.0);
            contact.penetration = penZ + radius;
        }

        // Transform normal to world space
        contact.normal = boxOrient.rotate(localNormal);
        contact.point = spherePos - contact.normal * radius;
        return true;
    }

    // Normal case: sphere center outside box
    contact.normal = delta * (1.0 / distance);  // Normalize, pointing from sphere to box
    contact.normal = contact.normal * -1.0;     // Flip to point from box to sphere
    contact.penetration = radius - distance;
    contact.point = closestPoint;

    return true;
}

//==============================================================================
// Sphere-Cylinder Collision Detection
//==============================================================================

Vector CollisionDetector::closestPointOnCylinder(
    const Vector& point,
    const CylinderShape* cylinder,
    const Vector& cylPos,
    const Quaternion& cylOrient) {

    // Transform point to cylinder local space (cylinder aligned with Z-axis)
    Vector localPoint = cylOrient.inverse().rotate(point - cylPos);

    double radius = cylinder->getRadius();
    double halfHeight = cylinder->getHeight() * 0.5;

    // Clamp Z to cylinder height
    double clampedZ = max(-halfHeight, min(halfHeight, localPoint.getZ()));

    // Project point onto cylinder axis (Z-axis in local space)
    Vector axisPoint(0, 0, clampedZ);

    // Calculate radial vector from axis to point
    Vector radial(localPoint.getX(), localPoint.getY(), 0);
    double radialDistance = sqrt(radial.getX() * radial.getX() + radial.getY() * radial.getY());

    Vector closestLocal;
    if (radialDistance > 1e-10) {
        // Clamp to cylinder radius
        double clampedRadial = min(radius, radialDistance);
        closestLocal = axisPoint + radial * (clampedRadial / radialDistance);
    } else {
        // Point is on cylinder axis
        closestLocal = axisPoint;
    }

    // Transform back to world space
    return cylPos + cylOrient.rotate(closestLocal);
}

bool CollisionDetector::detectSphereCylinder(
    const SphereShape* sphere, const Vector& spherePos,
    const CylinderShape* cylinder, const Vector& cylPos, const Quaternion& cylOrient,
    Contact& contact) {

    // Find closest point on cylinder to sphere center
    Vector closestPoint = closestPointOnCylinder(spherePos, cylinder, cylPos, cylOrient);

    // Calculate distance from sphere center to closest point
    Vector delta = closestPoint - spherePos;
    double distanceSquared = delta.dotProduct(delta);
    double radius = sphere->getRadius();
    double radiusSquared = radius * radius;

    // Check for collision
    if (distanceSquared >= radiusSquared) {
        return false;  // No collision
    }

    double distance = sqrt(distanceSquared);

    // Handle degenerate case
    if (distance < 1e-10) {
        // Sphere center very close to cylinder surface
        // Use approximation: normal points from cylinder center to sphere center
        Vector approxNormal = spherePos - cylPos;
        double approxDist = approxNormal.getLengthVector();
        if (approxDist > 1e-10) {
            contact.normal = approxNormal * (1.0 / approxDist);
        } else {
            contact.normal = Vector(0, 0, 1);  // Arbitrary
        }
        contact.penetration = radius;
        contact.point = spherePos;
        return true;
    }

    // Normal case
    contact.normal = delta * (1.0 / distance);  // Normalize
    contact.normal = contact.normal * -1.0;     // Flip to point from cylinder to sphere
    contact.penetration = radius - distance;
    contact.point = closestPoint;

    return true;
}

//==============================================================================
// Sphere-Pyramid Collision Detection
//==============================================================================

bool CollisionDetector::detectSpherePyramid(
    const SphereShape* sphere, const Vector& spherePos,
    const PyramidShape* pyramid, const Vector& pyrPos, const Quaternion& pyrOrient,
    Contact& contact) {

    // For now, use AABB approximation
    // TODO: Implement precise pyramid collision detection

    // Get pyramid vertices
    Vector vertices[5];
    pyramid->getVertices(vertices);

    // Transform vertices to world space
    for (int i = 0; i < 5; i++) {
        vertices[i] = pyrPos + pyrOrient.rotate(vertices[i]);
    }

    // Find closest vertex to sphere
    double minDistSquared = 1e30;
    Vector closestPoint = vertices[0];

    for (int i = 0; i < 5; i++) {
        Vector delta = vertices[i] - spherePos;
        double distSquared = delta.dotProduct(delta);
        if (distSquared < minDistSquared) {
            minDistSquared = distSquared;
            closestPoint = vertices[i];
        }
    }

    // Check collision with closest vertex (simple approximation)
    double radius = sphere->getRadius();
    if (minDistSquared >= radius * radius) {
        return false;  // No collision
    }

    double distance = sqrt(minDistSquared);
    if (distance < 1e-10) {
        contact.normal = Vector(0, 0, 1);
        contact.penetration = radius;
        contact.point = spherePos;
        return true;
    }

    Vector delta = closestPoint - spherePos;
    contact.normal = delta * (1.0 / distance);
    contact.normal = contact.normal * -1.0;
    contact.penetration = radius - distance;
    contact.point = closestPoint;

    return true;
}

//==============================================================================
// Box-Box Collision Detection (SAT Algorithm)
//==============================================================================

bool CollisionDetector::detectBoxBox(
    const BoxShape* boxA, const Vector& posA, const Quaternion& orientA,
    const BoxShape* boxB, const Vector& posB, const Quaternion& orientB,
    CollisionManifold& manifold) {

    // TODO: Implement full SAT algorithm
    // For now, use AABB approximation as placeholder

    AABB aabbA = boxA->getAABB(posA, orientA);
    AABB aabbB = boxB->getAABB(posB, orientB);

    if (!aabbA.intersects(aabbB)) {
        return false;
    }

    // Simple contact generation (placeholder)
    Contact contact;
    contact.point = (posA + posB) * 0.5;
    contact.normal = (posB - posA);
    double dist = contact.normal.getLengthVector();
    if (dist > 1e-10) {
        contact.normal = contact.normal * (1.0 / dist);
    } else {
        contact.normal = Vector(0, 0, 1);
    }
    contact.penetration = 0.1;  // Placeholder

    manifold.addContact(contact);
    return true;
}

//==============================================================================
// Generic Collision Detection (Shape Polymorphism)
//==============================================================================

bool CollisionDetector::detectCollision(
    RigidBody* bodyA, RigidBody* bodyB,
    Contact& contact) {

    if (!bodyA || !bodyB) return false;

    Shape* shapeA = bodyA->getShape();
    Shape* shapeB = bodyB->getShape();

    if (!shapeA || !shapeB) return false;

    Vector posA = bodyA->getPosition();
    Vector posB = bodyB->getPosition();
    Quaternion orientA = bodyA->getOrientation();
    Quaternion orientB = bodyB->getOrientation();

    // Set body pointers in contact
    contact.bodyA = bodyA;
    contact.bodyB = bodyB;

    // Try to cast to specific shape types and dispatch
    const SphereShape* sphereA = dynamic_cast<const SphereShape*>(shapeA);
    const SphereShape* sphereB = dynamic_cast<const SphereShape*>(shapeB);
    const BoxShape* boxA = dynamic_cast<const BoxShape*>(shapeA);
    const BoxShape* boxB = dynamic_cast<const BoxShape*>(shapeB);
    const CylinderShape* cylA = dynamic_cast<const CylinderShape*>(shapeA);
    const CylinderShape* cylB = dynamic_cast<const CylinderShape*>(shapeB);
    const PyramidShape* pyrA = dynamic_cast<const PyramidShape*>(shapeA);
    const PyramidShape* pyrB = dynamic_cast<const PyramidShape*>(shapeB);

    // Sphere-Sphere
    if (sphereA && sphereB) {
        return detectSphereSphere(sphereA, posA, sphereB, posB, contact);
    }

    // Sphere-Box (or Box-Sphere)
    if (sphereA && boxB) {
        return detectSphereBox(sphereA, posA, boxB, posB, orientB, contact);
    }
    if (boxA && sphereB) {
        bool result = detectSphereBox(sphereB, posB, boxA, posA, orientA, contact);
        if (result) {
            // Flip normal since we swapped order
            contact.normal = contact.normal * -1.0;
        }
        return result;
    }

    // Sphere-Cylinder (or Cylinder-Sphere)
    if (sphereA && cylB) {
        return detectSphereCylinder(sphereA, posA, cylB, posB, orientB, contact);
    }
    if (cylA && sphereB) {
        bool result = detectSphereCylinder(sphereB, posB, cylA, posA, orientA, contact);
        if (result) {
            contact.normal = contact.normal * -1.0;
        }
        return result;
    }

    // Sphere-Pyramid (or Pyramid-Sphere)
    if (sphereA && pyrB) {
        return detectSpherePyramid(sphereA, posA, pyrB, posB, orientB, contact);
    }
    if (pyrA && sphereB) {
        bool result = detectSpherePyramid(sphereB, posB, pyrA, posA, orientA, contact);
        if (result) {
            contact.normal = contact.normal * -1.0;
        }
        return result;
    }

    // Box-Box
    if (boxA && boxB) {
        CollisionManifold manifold;
        bool result = detectBoxBox(boxA, posA, orientA, boxB, posB, orientB, manifold);
        if (result && manifold.hasContacts()) {
            contact = manifold.contacts[0];
            contact.bodyA = bodyA;
            contact.bodyB = bodyB;
        }
        return result;
    }

    // Unsupported shape combination
    return false;
}

bool CollisionDetector::detectCollision(
    RigidBody* bodyA, RigidBody* bodyB,
    CollisionManifold& manifold) {

    Contact contact;
    bool result = detectCollision(bodyA, bodyB, contact);
    if (result) {
        manifold.addContact(contact);
    }
    return result;
}
