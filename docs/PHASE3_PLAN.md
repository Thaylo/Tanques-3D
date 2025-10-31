# Phase 3 Planning - Collision Detection

## Overview

Phase 3 will implement collision detection for the physics system, enabling rigid bodies to detect when they intersect and generate contact information for collision response.

**Status:** Planning
**Prerequisites:** ✅ Phase 1 Complete, ✅ Phase 2 Complete
**Estimated Duration:** 2-3 weeks

## Architecture

Collision detection is divided into two phases:

### 1. Broad Phase (Spatial Acceleration)
**Goal:** Quickly eliminate impossible collisions, reduce N² problem to manageable pairs

### 2. Narrow Phase (Precise Detection)
**Goal:** Compute exact collision information for candidate pairs from broad phase

## Broad Phase: Spatial Acceleration Structure

### Recommended Approach: **Octree**

An octree is a tree data structure where each internal node has exactly 8 children, subdividing 3D space into octants.

#### Why Octree for This Project?

**Advantages:**
1. ✅ **3D Native** - Natural fit for 3D games (tanks moving in 3D space)
2. ✅ **Dynamic Scenes** - Handles moving objects well (tanks, projectiles, debris)
3. ✅ **Hierarchical** - O(log n) query time for collision detection
4. ✅ **Spatial Locality** - Objects near each other in space are near in tree
5. ✅ **Flexible Granularity** - Adaptive subdivision based on object density
6. ✅ **Destructible Buildings** - Debris fragments naturally cluster in space
7. ✅ **Simple Queries** - "Find all objects in region" is straightforward

**Disadvantages (Acceptable for This Project):**
- ⚠️ Requires rebuild/rebalancing as objects move (mitigated by loose octree)
- ⚠️ More complex than spatial hashing (but better performance)
- ⚠️ Memory overhead for tree structure (acceptable for game scale)

#### Octree Design

```cpp
class OctreeNode {
private:
    AABB bounds;                    // This node's spatial bounds
    OctreeNode* children[8];        // 8 octants (null if leaf)
    std::vector<RigidBody*> objects; // Objects in this node
    int maxObjects;                 // Threshold to subdivide
    int maxDepth;                   // Maximum tree depth
    int currentDepth;               // This node's depth

public:
    void insert(RigidBody* obj);
    void remove(RigidBody* obj);
    void update(RigidBody* obj);    // Re-insert after movement
    void subdivide();
    void query(const AABB& region, std::vector<RigidBody*>& results);
    void getPotentialCollisions(std::vector<CollisionPair>& pairs);
};

class Octree {
private:
    OctreeNode* root;
    Vector worldMin, worldMax;      // Total world bounds

public:
    Octree(const Vector& min, const Vector& max, int maxObjectsPerNode = 8, int maxDepth = 8);
    void clear();
    void rebuild(const std::vector<RigidBody*>& objects);
    void getPotentialCollisions(std::vector<CollisionPair>& pairs);
};
```

#### Octree Configuration

**Recommended Parameters:**
- **Max Objects Per Node:** 8-16 objects
  - Too few: Excessive subdivisions, deep tree
  - Too many: Poor culling, many false positives

- **Max Depth:** 6-8 levels
  - Prevents infinite subdivision for overlapping objects
  - Depth 6 = 8^6 = 262,144 leaf nodes (maximum)
  - Depth 8 = 8^8 = 16,777,216 leaf nodes (probably too deep)

- **World Bounds:** Based on game arena size
  - Example: Arena 100x100x50 units → root bounds (-50, -50, 0) to (50, 50, 50)

#### Octree Subdivision Strategy

```
Root Node (bounds: entire world)
├─ Octant 0 (---) : x<0, y<0, z<0
├─ Octant 1 (+--) : x>0, y<0, z<0
├─ Octant 2 (-+-) : x<0, y>0, z<0
├─ Octant 3 (++-)  : x>0, y>0, z<0
├─ Octant 4 (--+) : x<0, y<0, z>0
├─ Octant 5 (+-+) : x>0, y<0, z>0
├─ Octant 6 (-++) : x<0, y>0, z>0
└─ Octant 7 (+++) : x>0, y>0, z>0
```

Each octant subdivides when object count > maxObjects.

#### Loose Octree Optimization

**Problem:** Standard octree requires frequent updates when objects move between nodes.

**Solution:** Loose octree - each node's bounds are enlarged by a factor (e.g., 2x):
- Object stays in node even if it moves slightly outside strict bounds
- Reduces update frequency
- Slight increase in false positives (acceptable tradeoff)

```cpp
class LooseOctreeNode : public OctreeNode {
private:
    AABB strictBounds;   // Actual octant bounds
    AABB looseBounds;    // Enlarged bounds (e.g., 2x size)
    double looseFactor;  // Enlargement factor (typically 2.0)

public:
    bool contains(const AABB& objectBounds) const {
        return looseBounds.contains(objectBounds);
    }
};
```

### Alternative Approaches (for Comparison)

#### Option 2: AABB Tree (BVH - Bounding Volume Hierarchy)
**Pros:**
- ✅ No fixed world bounds needed
- ✅ Very efficient for static geometry
- ✅ Industry standard (used in many physics engines)

**Cons:**
- ⚠️ More complex to implement
- ⚠️ Rebuilding for dynamic scenes is expensive
- ⚠️ Better suited for static/kinematic objects

**Verdict:** Good for static buildings, less ideal for dynamic tank game

#### Option 3: Spatial Hashing
**Pros:**
- ✅ Extremely simple to implement
- ✅ O(1) insertion and lookup
- ✅ No tree maintenance

**Cons:**
- ⚠️ Fixed cell size (hard to tune for varied object sizes)
- ⚠️ Hash collisions can degrade performance
- ⚠️ No hierarchical culling

**Verdict:** Good as a fallback or for prototyping, but octree is better

#### Option 4: Sweep and Prune
**Pros:**
- ✅ Excellent for mostly-stationary objects with some movement
- ✅ Cache-friendly
- ✅ Good for 2D-like games

**Cons:**
- ⚠️ Less effective when objects move in all 3 dimensions
- ⚠️ Degrades with highly dynamic scenes

**Verdict:** Better for 2D platformers than 3D tank combat

### **Final Recommendation: Octree (with Loose optimization)**

## Narrow Phase: Shape-Specific Collision Detection

Once the octree provides candidate pairs, we need precise collision detection.

### Implementation Order (Simplest → Most Complex)

#### 1. Sphere-Sphere (Simplest - START HERE)
```cpp
bool detectCollision(const SphereShape& s1, const SphereShape& s2,
                     const Vector& p1, const Vector& p2,
                     CollisionInfo& info) {
    Vector delta = p2 - p1;
    double distance = delta.getLengthVector();
    double radiusSum = s1.getRadius() + s2.getRadius();

    if (distance < radiusSum) {
        info.normal = delta.normalized();
        info.penetration = radiusSum - distance;
        info.contactPoint = p1 + info.normal * s1.getRadius();
        return true;
    }
    return false;
}
```
**Complexity:** O(1), ~10 FLOPs
**Tests Required:** ~15 tests

#### 2. Sphere-Box (Medium Complexity)
- Find closest point on box to sphere center
- Check if distance < sphere radius
- **Algorithm:** Clamp sphere center to box bounds

**Complexity:** O(1), ~20 FLOPs
**Tests Required:** ~25 tests

#### 3. Sphere-Cylinder (Medium Complexity)
- Project sphere onto cylinder axis
- Check radial distance from axis
- Handle cylinder caps

**Complexity:** O(1), ~30 FLOPs
**Tests Required:** ~30 tests

#### 4. Sphere-Pyramid (Medium-High Complexity)
- Test against 5 faces (4 triangular + 1 square base)
- Find closest point on pyramid surface

**Complexity:** O(1), ~40 FLOPs
**Tests Required:** ~35 tests

#### 5. Box-Box (High Complexity - SAT Algorithm)
- **SAT (Separating Axis Theorem):** 15 potential separating axes
  - 3 face normals from box A
  - 3 face normals from box B
  - 9 edge-edge cross products
- If no separating axis found → collision

**Complexity:** O(1), ~100 FLOPs (worst case)
**Tests Required:** ~40 tests

#### 6. Box-Cylinder (High Complexity)
- Combine SAT for box faces with cylinder axis tests
- Handle edge cases (cylinder caps vs box edges)

**Complexity:** O(1), ~120 FLOPs
**Tests Required:** ~40 tests

#### 7-10. Remaining Pairs (Very High Complexity)
- Box-Pyramid
- Cylinder-Cylinder
- Cylinder-Pyramid
- Pyramid-Pyramid

**Note:** These can be implemented using GJK algorithm (general convex collision)

### GJK Algorithm (General Solution)

**Gilbert-Johnson-Keerthi (GJK):** Works for any convex shape

**Advantages:**
- ✅ Single algorithm for all convex shapes
- ✅ Very efficient for separated objects
- ✅ Industry standard

**Disadvantages:**
- ⚠️ More complex to implement than specialized algorithms
- ⚠️ Requires support function for each shape

**Implementation:**
```cpp
class ConvexShape {
public:
    virtual Vector support(const Vector& direction) const = 0;
};

bool GJK(const ConvexShape& shapeA, const ConvexShape& shapeB) {
    // Iteratively build simplex in Minkowski difference
    // Returns true if origin is inside simplex (collision)
}
```

## Contact Generation

After detecting a collision, we need contact information for physics response.

### Contact Information
```cpp
struct Contact {
    Vector point;           // World-space contact point
    Vector normal;          // Contact normal (from A to B)
    double penetration;     // Penetration depth
    RigidBody* bodyA;       // First body
    RigidBody* bodyB;       // Second body

    // For collision response (Phase 4)
    double restitution;     // Bounciness (0=inelastic, 1=elastic)
    double friction;        // Surface friction
};

struct CollisionManifold {
    std::vector<Contact> contacts;  // Multiple contact points
    Vector normal;                  // Average contact normal
};
```

### Contact Point Generation Strategies

**For Sphere-Sphere:**
- Single contact point: midpoint between surfaces

**For Face-Face Contacts (Box-Box):**
- 4 contact points: clipped face polygon intersection

**For Edge-Edge Contacts:**
- 1 contact point: closest point between edges

**For Vertex-Face Contacts:**
- 1 contact point: vertex position

## Implementation Plan

### Week 1: Broad Phase (Octree)
**Day 1-2:** Basic Octree Structure
- [ ] Implement OctreeNode class
- [ ] Implement subdivision logic
- [ ] Test with static objects

**Day 3-4:** Dynamic Octree
- [ ] Implement insert/remove/update
- [ ] Handle moving objects
- [ ] Test with dynamic objects

**Day 5-7:** Octree Optimization
- [ ] Implement loose octree
- [ ] Optimize query performance
- [ ] Write comprehensive tests (~40 tests)

### Week 2: Narrow Phase (Basic Shapes)
**Day 1-2:** Sphere-Sphere Collision
- [ ] Implement detection algorithm
- [ ] Generate contact information
- [ ] Test edge cases (~15 tests)

**Day 3-4:** Sphere-Box Collision
- [ ] Implement closest-point algorithm
- [ ] Handle box rotation with quaternions
- [ ] Test all orientations (~25 tests)

**Day 5-7:** Sphere-Cylinder & Sphere-Pyramid
- [ ] Implement both algorithms
- [ ] Generate proper contact normals
- [ ] Test edge cases (~65 tests combined)

### Week 3: Advanced Collision (SAT/GJK)
**Day 1-3:** Box-Box (SAT)
- [ ] Implement 15-axis SAT test
- [ ] Optimize with early-out
- [ ] Test all collision configurations (~40 tests)

**Day 4-7:** Remaining Pairs or GJK
- [ ] Either implement specialized algorithms OR
- [ ] Implement GJK for general convex collision
- [ ] Test all shape combinations (~60 tests)

### Testing Strategy

**Total Expected Tests:** ~300 new tests
- Octree tests: 40
- Sphere-Sphere: 15
- Sphere-Box: 25
- Sphere-Cylinder: 30
- Sphere-Pyramid: 35
- Box-Box: 40
- Remaining pairs: ~115

**Test Categories:**
1. Collision detection (yes/no)
2. Contact point accuracy
3. Contact normal correctness
4. Penetration depth calculation
5. Edge cases (touching, grazing, deep penetration)
6. Performance tests (1000+ objects)

## Performance Targets

### Broad Phase (Octree)
- **Query time:** O(log n) average, O(n) worst case
- **Update time:** O(log n) per object
- **Target:** 1000+ objects at 60 FPS

### Narrow Phase
- **Sphere-Sphere:** < 0.001 ms per pair
- **Sphere-Box:** < 0.002 ms per pair
- **Box-Box (SAT):** < 0.005 ms per pair
- **Target:** 100+ collision checks per frame at 60 FPS

## Integration Points

### With Phase 2 (Shapes)
- ✅ AABB already implemented for all shapes
- ✅ Shape polymorphism ready
- [ ] Add support functions for GJK (if using)

### With RigidBody
- ✅ Position and orientation available
- ✅ AABB can be queried
- [ ] Add collision callback hooks

### Future Phase 4 (Collision Response)
- [ ] Contact information format designed for response
- [ ] Restitution and friction parameters prepared

## Success Criteria

Phase 3 is complete when:
- [ ] Octree correctly identifies all potential collision pairs
- [ ] All shape-shape collision detectors implemented
- [ ] Contact information is accurate (point, normal, penetration)
- [ ] 100% test pass rate maintained (634+ tests total)
- [ ] Performance targets met (1000+ objects, 60 FPS)
- [ ] No false negatives (missed collisions)
- [ ] False positives < 5% (acceptable from broad phase)
- [ ] Documentation complete
- [ ] Code committed and pushed

## References

**Octree:**
- "Real-Time Collision Detection" by Christer Ericson (Chapter 7)
- "Game Engine Architecture" by Jason Gregory (Chapter 12.4)

**SAT Algorithm:**
- "Real-Time Collision Detection" Chapter 4.4
- Separating Axis Theorem explained: http://www.dyn4j.org/2010/01/sat/

**GJK Algorithm:**
- "A Fast and Robust GJK Implementation" - Casey Muratori
- "Implementing GJK" - Tonic Games blog

**Collision Detection Overview:**
- "Game Physics Engine Development" by Ian Millington (Chapter 12-14)

---

**Phase 3 Status: PLANNING**
**Recommended Next Step: Begin Octree implementation**
**Estimated Completion: 2-3 weeks**
