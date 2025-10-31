/*
 * Shape.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Abstract base class for 3D shapes with collision geometry and inertia
 */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "Vector.h"
#include "Matrix3x3.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

namespace std {

// Axis-Aligned Bounding Box for broad-phase collision detection
struct AABB {
    Vector min;
    Vector max;

    AABB() : min(0, 0, 0), max(0, 0, 0) {}
    AABB(const Vector &minV, const Vector &maxV) : min(minV), max(maxV) {}

    bool intersects(const AABB &other) const {
        return (min.getX() <= other.max.getX() && max.getX() >= other.min.getX()) &&
               (min.getY() <= other.max.getY() && max.getY() >= other.min.getY()) &&
               (min.getZ() <= other.max.getZ() && max.getZ() >= other.min.getZ());
    }
};

class Shape {
protected:
    Vector color;  // RGB color for rendering (0-1 range)

public:
    Shape() : color(1.0, 1.0, 1.0) {}
    virtual ~Shape() {}

    // Set rendering color
    void setColor(const Vector &c) { color = c; }
    Vector getColor() const { return color; }

    // Calculate inertia tensor for this shape with given mass
    // Returns inertia tensor in local (body) coordinates
    virtual Matrix3x3 calculateInertiaTensor(double mass) const = 0;

    // Get axis-aligned bounding box (AABB) for broad-phase collision
    virtual AABB getAABB(const Vector &position, const class Quaternion &orientation) const = 0;

    // Render the shape at origin (transformation is done by caller)
    virtual void render() const = 0;

    // Get characteristic size (for collision detection)
    virtual double getCharacteristicSize() const = 0;
};

} /* namespace std */

#endif /* SHAPE_H_ */
