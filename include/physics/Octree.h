/*
 * Octree.h
 *
 *  Created on: 2025-01-31
 *
 *  Octree spatial acceleration structure for broad-phase collision detection.
 *  Provides O(log n) query time for finding potential collision pairs.
 */

#ifndef OCTREE_H_
#define OCTREE_H_

#include "Vector.h"
#include "Shape.h"
#include <vector>

namespace std {

// Forward declarations
class RigidBody;

// Collision pair for narrow-phase detection
struct CollisionPair {
    RigidBody* bodyA;
    RigidBody* bodyB;

    CollisionPair(RigidBody* a, RigidBody* b) : bodyA(a), bodyB(b) {}

    // Equality check to avoid duplicate pairs
    bool operator==(const CollisionPair& other) const {
        return (bodyA == other.bodyA && bodyB == other.bodyB) ||
               (bodyA == other.bodyB && bodyB == other.bodyA);
    }
};

// Octree node representing a region of 3D space
class OctreeNode {
private:
    AABB bounds;                     // This node's spatial bounds
    OctreeNode* children[8];         // 8 octants (null if leaf)
    vector<RigidBody*> objects;      // Objects in this node
    int maxObjects;                  // Threshold to subdivide
    int maxDepth;                    // Maximum tree depth
    int currentDepth;                // This node's depth
    bool isLeaf;                     // True if this is a leaf node

    // Subdivision helpers
    void subdivide();
    int getOctant(const AABB& objectBounds) const;
    AABB getOctantBounds(int octant) const;

    // Check if object fits entirely within an octant
    bool fitsInOctant(const AABB& objectBounds, int octant) const;

public:
    OctreeNode(const AABB& bounds, int maxObjects, int maxDepth, int currentDepth);
    ~OctreeNode();

    // Insert/remove objects
    void insert(RigidBody* obj, const AABB& objBounds);
    void remove(RigidBody* obj, const AABB& objBounds);

    // Query operations
    void query(const AABB& region, vector<RigidBody*>& results) const;
    void getPotentialCollisions(vector<CollisionPair>& pairs) const;

    // Utilities
    void clear();
    int getObjectCount() const;
    int getTotalNodeCount() const;

    // Getters for testing
    bool getIsLeaf() const { return isLeaf; }
    const vector<RigidBody*>& getObjects() const { return objects; }
    const AABB& getBounds() const { return bounds; }
};

// Main octree class for spatial acceleration
class Octree {
private:
    OctreeNode* root;
    Vector worldMin;
    Vector worldMax;
    int maxObjectsPerNode;
    int maxDepth;

public:
    // Constructor: define world bounds and octree parameters
    Octree(const Vector& worldMin, const Vector& worldMax,
           int maxObjectsPerNode = 8, int maxDepth = 6);
    ~Octree();

    // Build/rebuild octree
    void clear();
    void rebuild(const vector<RigidBody*>& objects);

    // Query operations
    void insert(RigidBody* obj);
    void remove(RigidBody* obj);
    void update(RigidBody* obj);  // Remove and re-insert after movement

    // Get all potential collision pairs (broad-phase output)
    void getPotentialCollisions(vector<CollisionPair>& pairs);

    // Query for objects in a region
    void queryRegion(const AABB& region, vector<RigidBody*>& results) const;

    // Statistics
    int getObjectCount() const;
    int getNodeCount() const;

    // Getters for testing
    const OctreeNode* getRoot() const { return root; }
    Vector getWorldMin() const { return worldMin; }
    Vector getWorldMax() const { return worldMax; }
};

} /* namespace std */

#endif /* OCTREE_H_ */
