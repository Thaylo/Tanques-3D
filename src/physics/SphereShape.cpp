/*
 * SphereShape.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "SphereShape.h"
#include <cmath>
#include <stdexcept>

namespace std {

SphereShape::SphereShape(double r)
    : Shape(), radius(r) {
    // Validate radius
    if (r <= 0.0) {
        throw std::invalid_argument("SphereShape: radius must be positive");
    }
}

Matrix3x3 SphereShape::calculateInertiaTensor(double mass) const {
    // Inertia tensor for a solid sphere about its center of mass
    // For a sphere, the inertia tensor is isotropic (same in all directions)
    // I = (2/5) * m * r^2 for all three principal axes

    double inertia = (2.0 / 5.0) * mass * radius * radius;

    // Sphere is symmetric, so all diagonal elements are equal
    // Off-diagonal elements are zero (principal axes aligned with coordinate axes)
    return Matrix3x3::diagonal(inertia, inertia, inertia);
}

AABB SphereShape::getAABB(const Vector &position, const Quaternion &orientation) const {
    // For a sphere, the AABB is independent of orientation
    // The bounding box extends radius in all directions from center
    // Orientation parameter is ignored since sphere is rotationally symmetric
    (void)orientation;  // Suppress unused parameter warning

    Vector minV(
        position.getX() - radius,
        position.getY() - radius,
        position.getZ() - radius
    );

    Vector maxV(
        position.getX() + radius,
        position.getY() + radius,
        position.getZ() + radius
    );

    return AABB(minV, maxV);
}

void SphereShape::render() const {
    // Draw a solid sphere using OpenGL
    glColor3f(color.getX(), color.getY(), color.getZ());

    // glutSolidSphere parameters: radius, slices (longitude), stacks (latitude)
    // More slices and stacks = smoother sphere but more polygons
    int slices = 20;
    int stacks = 20;

    glutSolidSphere(radius, slices, stacks);
}

double SphereShape::getCharacteristicSize() const {
    // Return the diameter of the sphere
    // This represents the maximum extent of the shape
    return 2.0 * radius;
}

} /* namespace std */
