/*
 * Octree.cpp
 *
 *  Created on: 2025-01-31
 *
 *  Implementation of Octree spatial acceleration structure
 */

#include "Octree.h"
#include "RigidBody.h"
#include <algorithm>
#include <cmath>

using namespace std;

//==============================================================================
// OctreeNode Implementation
//==============================================================================

OctreeNode::OctreeNode(const AABB& bounds, int maxObjects, int maxDepth, int currentDepth)
    : bounds(bounds), maxObjects(maxObjects), maxDepth(maxDepth),
      currentDepth(currentDepth), isLeaf(true) {
    for (int i = 0; i < 8; i++) {
        children[i] = nullptr;
    }
}

OctreeNode::~OctreeNode() {
    clear();
}

void OctreeNode::clear() {
    // Delete all children
    for (int i = 0; i < 8; i++) {
        if (children[i]) {
            delete children[i];
            children[i] = nullptr;
        }
    }
    objects.clear();
    isLeaf = true;
}

AABB OctreeNode::getOctantBounds(int octant) const {
    // Calculate center of this node
    Vector center(
        (bounds.min.getX() + bounds.max.getX()) * 0.5,
        (bounds.min.getY() + bounds.max.getY()) * 0.5,
        (bounds.min.getZ() + bounds.max.getZ()) * 0.5
    );

    // Determine min/max for this octant
    Vector octantMin = bounds.min;
    Vector octantMax = bounds.max;

    // Octant encoding: bit 0=X, bit 1=Y, bit 2=Z (0=negative half, 1=positive half)
    if (octant & 1) {  // X positive
        octantMin = Vector(center.getX(), octantMin.getY(), octantMin.getZ());
    } else {  // X negative
        octantMax = Vector(center.getX(), octantMax.getY(), octantMax.getZ());
    }

    if (octant & 2) {  // Y positive
        octantMin = Vector(octantMin.getX(), center.getY(), octantMin.getZ());
    } else {  // Y negative
        octantMax = Vector(octantMax.getX(), center.getY(), octantMax.getZ());
    }

    if (octant & 4) {  // Z positive
        octantMin = Vector(octantMin.getX(), octantMin.getY(), center.getZ());
    } else {  // Z negative
        octantMax = Vector(octantMax.getX(), octantMax.getY(), center.getZ());
    }

    return AABB(octantMin, octantMax);
}

int OctreeNode::getOctant(const AABB& objectBounds) const {
    // Calculate center of this node
    Vector center(
        (bounds.min.getX() + bounds.max.getX()) * 0.5,
        (bounds.min.getY() + bounds.max.getY()) * 0.5,
        (bounds.min.getZ() + bounds.max.getZ()) * 0.5
    );

    // Calculate object center
    Vector objCenter(
        (objectBounds.min.getX() + objectBounds.max.getX()) * 0.5,
        (objectBounds.min.getY() + objectBounds.max.getY()) * 0.5,
        (objectBounds.min.getZ() + objectBounds.max.getZ()) * 0.5
    );

    // Determine octant based on which side of center the object is on
    int octant = 0;
    if (objCenter.getX() > center.getX()) octant |= 1;  // X positive
    if (objCenter.getY() > center.getY()) octant |= 2;  // Y positive
    if (objCenter.getZ() > center.getZ()) octant |= 4;  // Z positive

    return octant;
}

bool OctreeNode::fitsInOctant(const AABB& objectBounds, int octant) const {
    AABB octantBounds = getOctantBounds(octant);

    // Check if object fits entirely within octant
    return objectBounds.min.getX() >= octantBounds.min.getX() &&
           objectBounds.min.getY() >= octantBounds.min.getY() &&
           objectBounds.min.getZ() >= octantBounds.min.getZ() &&
           objectBounds.max.getX() <= octantBounds.max.getX() &&
           objectBounds.max.getY() <= octantBounds.max.getY() &&
           objectBounds.max.getZ() <= octantBounds.max.getZ();
}

void OctreeNode::subdivide() {
    if (!isLeaf) return;  // Already subdivided

    // Create 8 child nodes
    for (int i = 0; i < 8; i++) {
        AABB childBounds = getOctantBounds(i);
        children[i] = new OctreeNode(childBounds, maxObjects, maxDepth, currentDepth + 1);
    }

    // Redistribute objects to children
    vector<RigidBody*> remainingObjects;
    for (RigidBody* obj : objects) {
        AABB objBounds = obj->getAABB();
        int octant = getOctant(objBounds);

        // Try to fit object in one octant
        if (fitsInOctant(objBounds, octant)) {
            children[octant]->insert(obj, objBounds);
        } else {
            // Object straddles multiple octants, keep at this level
            remainingObjects.push_back(obj);
        }
    }

    objects = remainingObjects;
    isLeaf = false;
}

void OctreeNode::insert(RigidBody* obj, const AABB& objBounds) {
    // If not a leaf, try to insert into children
    if (!isLeaf) {
        int octant = getOctant(objBounds);
        if (fitsInOctant(objBounds, octant)) {
            children[octant]->insert(obj, objBounds);
            return;
        }
        // Doesn't fit in any single child, store at this level
        objects.push_back(obj);
        return;
    }

    // Leaf node: add object
    objects.push_back(obj);

    // Check if we should subdivide
    if (objects.size() > (size_t)maxObjects && currentDepth < maxDepth) {
        subdivide();
    }
}

void OctreeNode::remove(RigidBody* obj, const AABB& objBounds) {
    // Remove from this node's object list
    auto it = find(objects.begin(), objects.end(), obj);
    if (it != objects.end()) {
        objects.erase(it);
        return;
    }

    // Try to remove from children
    if (!isLeaf) {
        int octant = getOctant(objBounds);
        if (fitsInOctant(objBounds, octant)) {
            children[octant]->remove(obj, objBounds);
        }
    }
}

void OctreeNode::query(const AABB& region, vector<RigidBody*>& results) const {
    // Check if region intersects this node's bounds
    if (!bounds.intersects(region)) {
        return;
    }

    // Add objects at this level that intersect the region
    for (RigidBody* obj : objects) {
        AABB objBounds = obj->getAABB();
        if (objBounds.intersects(region)) {
            results.push_back(obj);
        }
    }

    // Recursively query children
    if (!isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (children[i]) {
                children[i]->query(region, results);
            }
        }
    }
}

void OctreeNode::getPotentialCollisions(vector<CollisionPair>& pairs) const {
    // Check all objects in this node against each other
    for (size_t i = 0; i < objects.size(); i++) {
        for (size_t j = i + 1; j < objects.size(); j++) {
            pairs.push_back(CollisionPair(objects[i], objects[j]));
        }
    }

    // If not a leaf, check objects at this level against all objects in children
    if (!isLeaf) {
        vector<RigidBody*> childObjects;

        // Collect all objects from children
        for (int i = 0; i < 8; i++) {
            if (children[i]) {
                children[i]->query(bounds, childObjects);
            }
        }

        // Check objects at this level against child objects
        for (RigidBody* obj1 : objects) {
            for (RigidBody* obj2 : childObjects) {
                if (obj1 != obj2) {
                    pairs.push_back(CollisionPair(obj1, obj2));
                }
            }
        }

        // Recursively get collisions from children
        for (int i = 0; i < 8; i++) {
            if (children[i]) {
                children[i]->getPotentialCollisions(pairs);
            }
        }
    }
}

int OctreeNode::getObjectCount() const {
    int count = objects.size();
    if (!isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (children[i]) {
                count += children[i]->getObjectCount();
            }
        }
    }
    return count;
}

int OctreeNode::getTotalNodeCount() const {
    int count = 1;  // Count this node
    if (!isLeaf) {
        for (int i = 0; i < 8; i++) {
            if (children[i]) {
                count += children[i]->getTotalNodeCount();
            }
        }
    }
    return count;
}

//==============================================================================
// Octree Implementation
//==============================================================================

Octree::Octree(const Vector& worldMin, const Vector& worldMax,
               int maxObjectsPerNode, int maxDepth)
    : worldMin(worldMin), worldMax(worldMax),
      maxObjectsPerNode(maxObjectsPerNode), maxDepth(maxDepth) {
    AABB rootBounds(worldMin, worldMax);
    root = new OctreeNode(rootBounds, maxObjectsPerNode, maxDepth, 0);
}

Octree::~Octree() {
    delete root;
}

void Octree::clear() {
    root->clear();
}

void Octree::rebuild(const vector<RigidBody*>& objects) {
    clear();
    for (RigidBody* obj : objects) {
        insert(obj);
    }
}

void Octree::insert(RigidBody* obj) {
    AABB objBounds = obj->getAABB();
    root->insert(obj, objBounds);
}

void Octree::remove(RigidBody* obj) {
    AABB objBounds = obj->getAABB();
    root->remove(obj, objBounds);
}

void Octree::update(RigidBody* obj) {
    remove(obj);
    insert(obj);
}

void Octree::getPotentialCollisions(vector<CollisionPair>& pairs) {
    pairs.clear();
    root->getPotentialCollisions(pairs);
}

void Octree::queryRegion(const AABB& region, vector<RigidBody*>& results) const {
    results.clear();
    root->query(region, results);
}

int Octree::getObjectCount() const {
    return root->getObjectCount();
}

int Octree::getNodeCount() const {
    return root->getTotalNodeCount();
}
