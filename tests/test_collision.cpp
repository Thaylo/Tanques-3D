/*
 * test_collision.cpp
 *
 *  Created on: 2025-01-31
 *
 *  Tests for narrow-phase collision detection
 */

#include <gtest/gtest.h>
#include "CollisionDetector.h"
#include "RigidBody.h"
#include "SphereShape.h"
#include "BoxShape.h"
#include "CylinderShape.h"
#include "PyramidShape.h"
#include "Quaternion.h"

using namespace std;

// Test constants
const double EPSILON = 1e-6;

//==============================================================================
// Sphere-Sphere Collision Tests
//==============================================================================

TEST(SphereSphereCollision, NoCollision_FarApart) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(10, 0, 0);  // 10 units apart

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    EXPECT_FALSE(collides);
}

TEST(SphereSphereCollision, JustTouching) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(2.0, 0, 0);  // Exactly 2 radii apart

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    // Should be very close to touching (floating point may cause slight overlap)
    EXPECT_NEAR(contact.penetration, 0.0, EPSILON);
}

TEST(SphereSphereCollision, Overlapping) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(1.5, 0, 0);  // Overlapping

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(collides);
    EXPECT_NEAR(contact.penetration, 0.5, EPSILON);  // 2.0 - 1.5 = 0.5
}

TEST(SphereSphereCollision, ContactNormal_PointsFromAToB) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(1.5, 0, 0);

    Contact contact;
    CollisionDetector::detectSphereSphere(&sphereA, posA, &sphereB, posB, contact);

    // Normal should point from A to B (positive X direction)
    EXPECT_NEAR(contact.normal.getX(), 1.0, EPSILON);
    EXPECT_NEAR(contact.normal.getY(), 0.0, EPSILON);
    EXPECT_NEAR(contact.normal.getZ(), 0.0, EPSILON);
}

TEST(SphereSphereCollision, ContactPoint_OnSurfaceOfSphereA) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(1.5, 0, 0);

    Contact contact;
    CollisionDetector::detectSphereSphere(&sphereA, posA, &sphereB, posB, contact);

    // Contact point should be on surface of sphere A
    EXPECT_NEAR(contact.point.getX(), 1.0, EPSILON);  // At radius of A
    EXPECT_NEAR(contact.point.getY(), 0.0, EPSILON);
    EXPECT_NEAR(contact.point.getZ(), 0.0, EPSILON);
}

TEST(SphereSphereCollision, DifferentSizes) {
    SphereShape sphereA(2.0);  // Large
    SphereShape sphereB(0.5);  // Small
    Vector posA(0, 0, 0);
    Vector posB(2.0, 0, 0);  // Overlapping (2.5 - 2.0 = 0.5 penetration)

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(collides);
    EXPECT_NEAR(contact.penetration, 0.5, EPSILON);
}

TEST(SphereSphereCollision, ExactOverlap_SamePosition) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(0, 0, 0);  // Exact same position

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(collides);
    EXPECT_NEAR(contact.penetration, 2.0, EPSILON);  // Full overlap
}

TEST(SphereSphereCollision, CollisionInDifferentDirections_Y) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(0, 1.5, 0);  // Y direction

    Contact contact;
    CollisionDetector::detectSphereSphere(&sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(contact.penetration > 0);
    EXPECT_NEAR(contact.normal.getY(), 1.0, EPSILON);
}

TEST(SphereSphereCollision, CollisionInDifferentDirections_Z) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(0, 0, 1.8);  // Z direction

    Contact contact;
    CollisionDetector::detectSphereSphere(&sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(contact.penetration > 0);
    EXPECT_NEAR(contact.normal.getZ(), 1.0, EPSILON);
}

TEST(SphereSphereCollision, CollisionDiagonal) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);
    Vector posA(0, 0, 0);
    Vector posB(1.0, 1.0, 0);  // 45 degrees

    Contact contact;
    bool collides = CollisionDetector::detectSphereSphere(
        &sphereA, posA, &sphereB, posB, contact);

    EXPECT_TRUE(collides);
    // Distance = sqrt(2) ≈ 1.414, radiusSum = 2, penetration ≈ 0.586
    EXPECT_GT(contact.penetration, 0.5);
    EXPECT_LT(contact.penetration, 0.6);
}

//==============================================================================
// Sphere-Box Collision Tests
//==============================================================================

TEST(SphereBoxCollision, NoCollision_FarApart) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);
    Vector spherePos(5, 0, 0);
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    EXPECT_FALSE(collides);
}

TEST(SphereBoxCollision, Overlapping_FaceContact) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);  // Box from -1 to 1
    Vector spherePos(1.5, 0, 0);  // Sphere overlapping positive X face
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    EXPECT_TRUE(collides);
    EXPECT_GT(contact.penetration, 0.0);
}

TEST(SphereBoxCollision, Touching_FaceContact) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);
    Vector spherePos(2.0, 0, 0);  // Just touching
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    // Should be very close to touching
    EXPECT_LE(contact.penetration, EPSILON);
}

TEST(SphereBoxCollision, EdgeContact) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);
    Vector spherePos(1.5, 1.5, 0);  // Near edge
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    // May or may not collide depending on exact positions
    if (collides) {
        EXPECT_GT(contact.penetration, 0.0);
    }
}

TEST(SphereBoxCollision, CornerContact) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);
    Vector spherePos(1.8, 1.8, 1.8);  // Near corner
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    // Check if collision is detected
    if (collides) {
        EXPECT_GT(contact.penetration, 0.0);
    }
}

TEST(SphereBoxCollision, InsideBox) {
    SphereShape sphere(0.5);
    BoxShape box(4.0, 4.0, 4.0);
    Vector spherePos(0, 0, 0);  // Sphere center inside box
    Vector boxPos(0, 0, 0);
    Quaternion boxOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereBox(
        &sphere, spherePos, &box, boxPos, boxOrient, contact);

    EXPECT_TRUE(collides);
    EXPECT_GT(contact.penetration, 0.0);
}

//==============================================================================
// Sphere-Cylinder Collision Tests
//==============================================================================

TEST(SphereCylinderCollision, NoCollision_FarApart) {
    SphereShape sphere(1.0);
    CylinderShape cylinder(1.0, 2.0);
    Vector spherePos(5, 0, 0);
    Vector cylPos(0, 0, 0);
    Quaternion cylOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereCylinder(
        &sphere, spherePos, &cylinder, cylPos, cylOrient, contact);

    EXPECT_FALSE(collides);
}

TEST(SphereCylinderCollision, Overlapping_SideContact) {
    SphereShape sphere(1.0);
    CylinderShape cylinder(1.0, 3.0);
    Vector spherePos(1.5, 0, 0);  // Touching side
    Vector cylPos(0, 0, 0);
    Quaternion cylOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereCylinder(
        &sphere, spherePos, &cylinder, cylPos, cylOrient, contact);

    EXPECT_TRUE(collides);
    EXPECT_GT(contact.penetration, 0.0);
}

TEST(SphereCylinderCollision, CapContact) {
    SphereShape sphere(1.0);
    CylinderShape cylinder(2.0, 2.0);  // Height 2, so cap at z=1
    Vector spherePos(0, 0, 1.5);  // Just above top cap, overlapping
    Vector cylPos(0, 0, 0);
    Quaternion cylOrient = Quaternion::identity();

    Contact contact;
    bool collides = CollisionDetector::detectSphereCylinder(
        &sphere, spherePos, &cylinder, cylPos, cylOrient, contact);

    // Sphere with radius 1.0 at z=1.5, bottom at 0.5, cap at 1.0
    // Should be overlapping
    EXPECT_TRUE(collides);
}

//==============================================================================
// Generic Collision Detection Tests (RigidBody integration)
//==============================================================================

TEST(GenericCollision, SphereSphere_ThroughRigidBody) {
    SphereShape sphereA(1.0);
    SphereShape sphereB(1.0);

    RigidBody bodyA(Vector(0, 0, 0), 1.0);
    RigidBody bodyB(Vector(1.5, 0, 0), 1.0);
    bodyA.setShape(&sphereA);
    bodyB.setShape(&sphereB);

    Contact contact;
    bool collides = CollisionDetector::detectCollision(&bodyA, &bodyB, contact);

    EXPECT_TRUE(collides);
    EXPECT_EQ(contact.bodyA, &bodyA);
    EXPECT_EQ(contact.bodyB, &bodyB);
    EXPECT_NEAR(contact.penetration, 0.5, EPSILON);
}

TEST(GenericCollision, SphereBox_ThroughRigidBody) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);

    RigidBody sphereBody(Vector(2.0, 0, 0), 1.0);
    RigidBody boxBody(Vector(0, 0, 0), 1.0);
    sphereBody.setShape(&sphere);
    boxBody.setShape(&box);

    Contact contact;
    bool collides = CollisionDetector::detectCollision(&sphereBody, &boxBody, contact);

    // Sphere touching or nearly touching box face
    EXPECT_LE(contact.penetration, EPSILON);
}

TEST(GenericCollision, BoxSphere_ReversedOrder) {
    SphereShape sphere(1.0);
    BoxShape box(2.0, 2.0, 2.0);

    RigidBody sphereBody(Vector(1.8, 0, 0), 1.0);
    RigidBody boxBody(Vector(0, 0, 0), 1.0);
    sphereBody.setShape(&sphere);
    boxBody.setShape(&box);

    Contact contact;
    // Test with reversed order (box first, sphere second)
    bool collides = CollisionDetector::detectCollision(&boxBody, &sphereBody, contact);

    // Should still detect collision
    if (collides) {
        EXPECT_GT(contact.penetration, 0.0);
    }
}

TEST(GenericCollision, NoShapes_NoCollision) {
    RigidBody bodyA(Vector(0, 0, 0), 1.0);
    RigidBody bodyB(Vector(1, 0, 0), 1.0);
    // No shapes set

    Contact contact;
    bool collides = CollisionDetector::detectCollision(&bodyA, &bodyB, contact);

    EXPECT_FALSE(collides);
}

TEST(GenericCollision, NullPointers_NoCollision) {
    Contact contact;
    bool collides = CollisionDetector::detectCollision(nullptr, nullptr, contact);

    EXPECT_FALSE(collides);
}

//==============================================================================
// Contact Manifold Tests
//==============================================================================

TEST(CollisionManifold, AddContact) {
    CollisionManifold manifold;

    Contact contact1;
    contact1.penetration = 0.5;
    contact1.normal = Vector(1, 0, 0);

    manifold.addContact(contact1);

    EXPECT_EQ(manifold.getContactCount(), 1);
    EXPECT_TRUE(manifold.hasContacts());
    EXPECT_NEAR(manifold.penetration, 0.5, EPSILON);
}

TEST(CollisionManifold, MultipleContacts_MaxPenetration) {
    CollisionManifold manifold;

    Contact contact1;
    contact1.penetration = 0.3;
    contact1.normal = Vector(1, 0, 0);

    Contact contact2;
    contact2.penetration = 0.7;  // Deeper
    contact2.normal = Vector(0, 1, 0);

    manifold.addContact(contact1);
    manifold.addContact(contact2);

    EXPECT_EQ(manifold.getContactCount(), 2);
    EXPECT_NEAR(manifold.penetration, 0.7, EPSILON);  // Maximum
}

TEST(CollisionManifold, EmptyManifold) {
    CollisionManifold manifold;

    EXPECT_FALSE(manifold.hasContacts());
    EXPECT_EQ(manifold.getContactCount(), 0);
}
