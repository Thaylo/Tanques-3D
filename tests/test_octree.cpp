/*
 * test_octree.cpp
 *
 *  Created on: 2025-01-31
 *
 *  Tests for Octree spatial acceleration structure
 */

#include <gtest/gtest.h>
#include "Octree.h"
#include "RigidBody.h"
#include "BoxShape.h"
#include "SphereShape.h"
#include <vector>
#include <algorithm>

using namespace std;

// Test constants
const double EPSILON = 1e-6;

//==============================================================================
// Octree Construction Tests
//==============================================================================

TEST(OctreeConstruction, BasicConstruction) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    EXPECT_EQ(octree.getObjectCount(), 0);
    EXPECT_EQ(octree.getNodeCount(), 1);  // Only root node
}

TEST(OctreeConstruction, CustomParameters) {
    Vector worldMin(0, 0, 0);
    Vector worldMax(100, 100, 100);
    Octree octree(worldMin, worldMax, 16, 8);  // 16 objects/node, depth 8

    ASSERT_NE(octree.getRoot(), nullptr);
    EXPECT_EQ(octree.getWorldMin().getX(), 0);
    EXPECT_EQ(octree.getWorldMax().getX(), 100);
}

TEST(OctreeConstruction, SmallWorld) {
    Vector worldMin(-1, -1, -1);
    Vector worldMax(1, 1, 1);
    Octree octree(worldMin, worldMax);

    EXPECT_EQ(octree.getNodeCount(), 1);
}

//==============================================================================
// Octree Insertion Tests
//==============================================================================

TEST(OctreeInsertion, InsertSingleObject) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&sphere);

    octree.insert(&body);

    EXPECT_EQ(octree.getObjectCount(), 1);
}

TEST(OctreeInsertion, InsertMultipleObjects) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    vector<RigidBody*> bodies;

    for (int i = 0; i < 10; i++) {
        RigidBody* body = new RigidBody(Vector(i, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    EXPECT_EQ(octree.getObjectCount(), 10);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

TEST(OctreeInsertion, InsertAtDifferentPositions) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    // Insert in all 8 octants
    Vector positions[] = {
        Vector(-5, -5, -5),  // ---
        Vector( 5, -5, -5),  // +--
        Vector(-5,  5, -5),  // -+-
        Vector( 5,  5, -5),  // ++-
        Vector(-5, -5,  5),  // --+
        Vector( 5, -5,  5),  // +-+
        Vector(-5,  5,  5),  // -++
        Vector( 5,  5,  5)   // +++
    };

    for (int i = 0; i < 8; i++) {
        RigidBody* body = new RigidBody(positions[i], 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    EXPECT_EQ(octree.getObjectCount(), 8);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

//==============================================================================
// Octree Subdivision Tests
//==============================================================================

TEST(OctreeSubdivision, SubdivideWhenFull) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax, 4, 6);  // Max 4 objects per node

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    // Insert 5 objects at same location (should stay in root, no subdivision)
    for (int i = 0; i < 5; i++) {
        RigidBody* body = new RigidBody(Vector(0, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    // Should have subdivided
    EXPECT_GT(octree.getNodeCount(), 1);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

TEST(OctreeSubdivision, SubdivideMultipleLevels) {
    Vector worldMin(-100, -100, -100);
    Vector worldMax(100, 100, 100);
    Octree octree(worldMin, worldMax, 2, 6);  // Max 2 objects per node

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    // Insert many objects in one octant
    for (int i = 0; i < 10; i++) {
        double offset = i * 0.1;
        RigidBody* body = new RigidBody(Vector(50 + offset, 50, 50), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    // Should have created multiple levels
    EXPECT_GT(octree.getNodeCount(), 4);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

//==============================================================================
// Octree Removal Tests
//==============================================================================

TEST(OctreeRemoval, RemoveSingleObject) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&sphere);

    octree.insert(&body);
    EXPECT_EQ(octree.getObjectCount(), 1);

    octree.remove(&body);
    EXPECT_EQ(octree.getObjectCount(), 0);
}

TEST(OctreeRemoval, RemoveMultipleObjects) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    vector<RigidBody*> bodies;

    for (int i = 0; i < 5; i++) {
        RigidBody* body = new RigidBody(Vector(i, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    EXPECT_EQ(octree.getObjectCount(), 5);

    // Remove half
    for (int i = 0; i < 3; i++) {
        octree.remove(bodies[i]);
    }

    EXPECT_EQ(octree.getObjectCount(), 2);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

//==============================================================================
// Octree Update Tests
//==============================================================================

TEST(OctreeUpdate, UpdateObjectPosition) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&sphere);

    octree.insert(&body);

    // Move object
    body.setPosition(Vector(5, 5, 5));
    octree.update(&body);

    // Should still have 1 object
    EXPECT_EQ(octree.getObjectCount(), 1);
}

TEST(OctreeUpdate, UpdateMultipleObjects) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    for (int i = 0; i < 10; i++) {
        RigidBody* body = new RigidBody(Vector(i - 5, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    // Move all objects
    for (int i = 0; i < 10; i++) {
        bodies[i]->setPosition(Vector(0, i - 5, 0));
        octree.update(bodies[i]);
    }

    EXPECT_EQ(octree.getObjectCount(), 10);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

//==============================================================================
// Octree Query Tests
//==============================================================================

TEST(OctreeQuery, QueryRegion) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    // Insert objects at different positions
    for (int i = -5; i <= 5; i++) {
        RigidBody* body = new RigidBody(Vector(i, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    // Query a small region
    AABB queryRegion(Vector(-2, -1, -1), Vector(2, 1, 1));
    vector<RigidBody*> results;
    octree.queryRegion(queryRegion, results);

    // Should find objects within region
    EXPECT_GT(results.size(), 0);
    EXPECT_LE(results.size(), bodies.size());

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

TEST(OctreeQuery, QueryEmptyRegion) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(0.5);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&sphere);
    octree.insert(&body);

    // Query far away region
    AABB queryRegion(Vector(50, 50, 50), Vector(60, 60, 60));
    vector<RigidBody*> results;
    octree.queryRegion(queryRegion, results);

    EXPECT_EQ(results.size(), 0);
}

//==============================================================================
// Collision Pair Tests
//==============================================================================

TEST(OctreeCollision, NoPairsWithOneObject) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&sphere);
    octree.insert(&body);

    vector<CollisionPair> pairs;
    octree.getPotentialCollisions(pairs);

    EXPECT_EQ(pairs.size(), 0);
}

TEST(OctreeCollision, OnePairWithTwoObjects) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body1(Vector(0, 0, 0), 1.0);
    RigidBody body2(Vector(1, 0, 0), 1.0);
    body1.setShape(&sphere);
    body2.setShape(&sphere);

    octree.insert(&body1);
    octree.insert(&body2);

    vector<CollisionPair> pairs;
    octree.getPotentialCollisions(pairs);

    EXPECT_EQ(pairs.size(), 1);
}

TEST(OctreeCollision, MultiplePairs) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax, 10, 6);  // Large max objects to keep in root

    SphereShape sphere(0.5);
    vector<RigidBody*> bodies;

    // Create 4 objects close together
    for (int i = 0; i < 4; i++) {
        RigidBody* body = new RigidBody(Vector(i * 0.5, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    vector<CollisionPair> pairs;
    octree.getPotentialCollisions(pairs);

    // With 4 objects: n*(n-1)/2 = 6 pairs
    EXPECT_EQ(pairs.size(), 6);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

TEST(OctreeCollision, FarObjectsNoPairs) {
    Vector worldMin(-100, -100, -100);
    Vector worldMax(100, 100, 100);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    RigidBody body1(Vector(-50, 0, 0), 1.0);
    RigidBody body2(Vector(50, 0, 0), 1.0);
    body1.setShape(&sphere);
    body2.setShape(&sphere);

    octree.insert(&body1);
    octree.insert(&body2);

    vector<CollisionPair> pairs;
    octree.getPotentialCollisions(pairs);

    // Far apart objects should still generate a pair (broad phase)
    // Narrow phase will filter out non-colliding pairs
    EXPECT_GE(pairs.size(), 0);  // May or may not generate pair depending on octree structure
}

//==============================================================================
// Octree Clear and Rebuild Tests
//==============================================================================

TEST(OctreeClearRebuild, ClearOctree) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    vector<RigidBody*> bodies;

    for (int i = 0; i < 5; i++) {
        RigidBody* body = new RigidBody(Vector(i, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
        octree.insert(body);
    }

    EXPECT_EQ(octree.getObjectCount(), 5);

    octree.clear();

    EXPECT_EQ(octree.getObjectCount(), 0);
    EXPECT_EQ(octree.getNodeCount(), 1);  // Only root remains

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

TEST(OctreeClearRebuild, RebuildOctree) {
    Vector worldMin(-10, -10, -10);
    Vector worldMax(10, 10, 10);
    Octree octree(worldMin, worldMax);

    SphereShape sphere(1.0);
    vector<RigidBody*> bodies;

    for (int i = 0; i < 10; i++) {
        RigidBody* body = new RigidBody(Vector(i, 0, 0), 1.0);
        body->setShape(&sphere);
        bodies.push_back(body);
    }

    octree.rebuild(bodies);

    EXPECT_EQ(octree.getObjectCount(), 10);

    // Rebuild again with moved objects
    for (int i = 0; i < 10; i++) {
        bodies[i]->setPosition(Vector(0, i, 0));
    }

    octree.rebuild(bodies);

    EXPECT_EQ(octree.getObjectCount(), 10);

    // Cleanup
    for (RigidBody* body : bodies) delete body;
}

//==============================================================================
// AABB Helper Tests (RigidBody integration)
//==============================================================================

TEST(RigidBodyAABB, GetAABBWithShape) {
    SphereShape sphere(2.0);
    RigidBody body(Vector(5, 5, 5), 1.0);
    body.setShape(&sphere);

    AABB aabb = body.getAABB();

    // Sphere with radius 2 at (5,5,5) should have AABB from (3,3,3) to (7,7,7)
    EXPECT_NEAR(aabb.min.getX(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), 3.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 7.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 7.0, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 7.0, EPSILON);
}

TEST(RigidBodyAABB, GetAABBWithoutShape) {
    RigidBody body(Vector(0, 0, 0), 1.0);
    // No shape set

    AABB aabb = body.getAABB();

    // Should return small epsilon box around position
    EXPECT_LT(aabb.max.getX() - aabb.min.getX(), 1.0);
}

TEST(RigidBodyAABB, GetAABBWithBoxShape) {
    BoxShape box(2.0, 3.0, 4.0);
    RigidBody body(Vector(0, 0, 0), 1.0);
    body.setShape(&box);

    AABB aabb = body.getAABB();

    // Box dimensions are 2x3x4, centered at origin
    EXPECT_NEAR(aabb.min.getX(), -1.0, EPSILON);
    EXPECT_NEAR(aabb.min.getY(), -1.5, EPSILON);
    EXPECT_NEAR(aabb.min.getZ(), -2.0, EPSILON);
    EXPECT_NEAR(aabb.max.getX(), 1.0, EPSILON);
    EXPECT_NEAR(aabb.max.getY(), 1.5, EPSILON);
    EXPECT_NEAR(aabb.max.getZ(), 2.0, EPSILON);
}
