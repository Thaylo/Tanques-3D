/**
 * Octree.h - Spatial partitioning for 3D (and 2D) worlds
 *
 * Template-based octree supporting:
 * - Fast nearest neighbor queries
 * - Range queries for collision detection
 * - Works in 2D (ignores Z) and 3D modes
 */

#ifndef OCTREE_H
#define OCTREE_H

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <vector>

namespace Spatial {

/**
 * AABB - Axis-Aligned Bounding Box
 */
struct AABB {
  float minX, minY, minZ;
  float maxX, maxY, maxZ;

  AABB() : minX(0), minY(0), minZ(0), maxX(0), maxY(0), maxZ(0) {}

  AABB(float x1, float y1, float z1, float x2, float y2, float z2)
      : minX(x1), minY(y1), minZ(z1), maxX(x2), maxY(y2), maxZ(z2) {}

  // 2D constructor (z = 0)
  AABB(float x1, float y1, float x2, float y2)
      : minX(x1), minY(y1), minZ(-1), maxX(x2), maxY(y2), maxZ(1) {}

  bool contains(float x, float y, float z = 0) const {
    return x >= minX && x <= maxX && y >= minY && y <= maxY && z >= minZ &&
           z <= maxZ;
  }

  bool intersects(const AABB &other) const {
    return !(maxX < other.minX || minX > other.maxX || maxY < other.minY ||
             minY > other.maxY || maxZ < other.minZ || minZ > other.maxZ);
  }

  float centerX() const { return (minX + maxX) * 0.5f; }
  float centerY() const { return (minY + maxY) * 0.5f; }
  float centerZ() const { return (minZ + maxZ) * 0.5f; }
};

/**
 * OctreeNode - Single node in the octree
 */
template <typename T> struct OctreeNode {
  AABB bounds;
  std::vector<std::pair<T, std::array<float, 3>>> items; // item + position
  std::array<OctreeNode *, 8> children = {nullptr};
  bool isLeaf = true;

  static constexpr int MAX_ITEMS = 8;
  static constexpr int MAX_DEPTH = 6;

  OctreeNode(const AABB &b) : bounds(b) {}

  ~OctreeNode() {
    for (auto *child : children) {
      delete child;
    }
  }

  void insert(const T &item, float x, float y, float z, int depth = 0) {
    if (!bounds.contains(x, y, z))
      return;

    if (isLeaf) {
      items.push_back({item, {x, y, z}});

      // Subdivide if too many items and not too deep
      if (items.size() > MAX_ITEMS && depth < MAX_DEPTH) {
        subdivide();
        // Re-insert items into children
        for (const auto &[it, pos] : items) {
          insertIntoChildren(it, pos[0], pos[1], pos[2], depth + 1);
        }
        items.clear();
        isLeaf = false;
      }
    } else {
      insertIntoChildren(item, x, y, z, depth + 1);
    }
  }

  void insertIntoChildren(const T &item, float x, float y, float z, int depth) {
    for (auto *child : children) {
      if (child && child->bounds.contains(x, y, z)) {
        child->insert(item, x, y, z, depth);
        return;
      }
    }
  }

  void subdivide() {
    float cx = bounds.centerX();
    float cy = bounds.centerY();
    float cz = bounds.centerZ();

    // 8 octants
    children[0] =
        new OctreeNode(AABB(bounds.minX, bounds.minY, bounds.minZ, cx, cy, cz));
    children[1] =
        new OctreeNode(AABB(cx, bounds.minY, bounds.minZ, bounds.maxX, cy, cz));
    children[2] =
        new OctreeNode(AABB(bounds.minX, cy, bounds.minZ, cx, bounds.maxY, cz));
    children[3] =
        new OctreeNode(AABB(cx, cy, bounds.minZ, bounds.maxX, bounds.maxY, cz));
    children[4] =
        new OctreeNode(AABB(bounds.minX, bounds.minY, cz, cx, cy, bounds.maxZ));
    children[5] =
        new OctreeNode(AABB(cx, bounds.minY, cz, bounds.maxX, cy, bounds.maxZ));
    children[6] =
        new OctreeNode(AABB(bounds.minX, cy, cz, cx, bounds.maxY, bounds.maxZ));
    children[7] =
        new OctreeNode(AABB(cx, cy, cz, bounds.maxX, bounds.maxY, bounds.maxZ));
  }

  // Query all items within radius of point
  void queryRadius(float x, float y, float z, float radius,
                   std::vector<std::pair<T, float>> &results) const {
    float radiusSq = radius * radius;

    // Check if query sphere intersects this node
    AABB queryBox(x - radius, y - radius, z - radius, x + radius, y + radius,
                  z + radius);
    if (!bounds.intersects(queryBox))
      return;

    if (isLeaf) {
      for (const auto &[item, pos] : items) {
        float dx = pos[0] - x;
        float dy = pos[1] - y;
        float dz = pos[2] - z;
        float distSq = dx * dx + dy * dy + dz * dz;
        if (distSq <= radiusSq) {
          results.push_back({item, distSq});
        }
      }
    } else {
      for (auto *child : children) {
        if (child)
          child->queryRadius(x, y, z, radius, results);
      }
    }
  }

  // Find K nearest neighbors
  void queryKNearest(float x, float y, float z, int k,
                     std::vector<std::pair<T, float>> &results) const {
    // Start with large radius, then shrink
    float searchRadius =
        std::max({bounds.maxX - bounds.minX, bounds.maxY - bounds.minY,
                  bounds.maxZ - bounds.minZ});

    std::vector<std::pair<T, float>> candidates;
    queryRadius(x, y, z, searchRadius, candidates);

    // Sort by distance and take top K
    std::partial_sort(candidates.begin(),
                      candidates.begin() + std::min(k, (int)candidates.size()),
                      candidates.end(), [](const auto &a, const auto &b) {
                        return a.second < b.second;
                      });

    int count = std::min(k, (int)candidates.size());
    results.insert(results.end(), candidates.begin(),
                   candidates.begin() + count);
  }
};

/**
 * Octree - Main spatial partitioning structure
 */
template <typename T> class Octree {
public:
  Octree(const AABB &worldBounds) { root_ = new OctreeNode<T>(worldBounds); }

  ~Octree() { delete root_; }

  // Clear and rebuild (call each frame with new positions)
  void clear() {
    AABB bounds = root_->bounds;
    delete root_;
    root_ = new OctreeNode<T>(bounds);
  }

  void insert(const T &item, float x, float y, float z = 0) {
    root_->insert(item, x, y, z);
  }

  // Find all items within radius
  std::vector<std::pair<T, float>> queryRadius(float x, float y, float z,
                                               float radius) const {
    std::vector<std::pair<T, float>> results;
    root_->queryRadius(x, y, z, radius, results);
    return results;
  }

  // Find K nearest items (returns item + distance squared)
  std::vector<std::pair<T, float>> queryKNearest(float x, float y, float z,
                                                 int k) const {
    std::vector<std::pair<T, float>> results;
    root_->queryKNearest(x, y, z, k, results);
    return results;
  }

  // 2D convenience methods
  void insert2D(const T &item, float x, float y) { insert(item, x, y, 0); }

  std::vector<std::pair<T, float>> queryRadius2D(float x, float y,
                                                 float radius) const {
    return queryRadius(x, y, 0, radius);
  }

  std::vector<std::pair<T, float>> queryKNearest2D(float x, float y,
                                                   int k) const {
    return queryKNearest(x, y, 0, k);
  }

private:
  OctreeNode<T> *root_;
};

} // namespace Spatial

#endif // OCTREE_H
