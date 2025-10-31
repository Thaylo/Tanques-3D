/*
 * CylinderShape.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "CylinderShape.h"
#include <cmath>

namespace std {

CylinderShape::CylinderShape(double r, double h)
    : Shape(), radius(r), height(h) {
    // Validate inputs
    if (radius <= 0.0 || height <= 0.0) {
        // In production code, this would throw an exception
        // For now, we'll just ensure minimum values
        if (radius <= 0.0) radius = 0.1;
        if (height <= 0.0) height = 0.1;
    }
}

Matrix3x3 CylinderShape::calculateInertiaTensor(double mass) const {
    // Inertia tensor for a solid cylinder with axis aligned along Z-axis
    // I_x = I_y = (1/12) * m * (3*r^2 + h^2)  (perpendicular to axis)
    // I_z = (1/2) * m * r^2  (around axis)

    double r2 = radius * radius;
    double h2 = height * height;

    double Ixx = (mass / 12.0) * (3.0 * r2 + h2);
    double Iyy = (mass / 12.0) * (3.0 * r2 + h2);
    double Izz = (mass / 2.0) * r2;

    return Matrix3x3::diagonal(Ixx, Iyy, Izz);
}

AABB CylinderShape::getAABB(const Vector &position, const Quaternion &orientation) const {
    // Get key vertices (8 points on top and bottom circles)
    Vector vertices[8];
    getKeyVertices(vertices);

    // Transform vertices to world space and find bounds
    Vector minV = position;
    Vector maxV = position;

    for (int i = 0; i < 8; i++) {
        Vector worldVertex = position + orientation.rotate(vertices[i]);

        if (i == 0) {
            minV = worldVertex;
            maxV = worldVertex;
        } else {
            minV = Vector(
                fmin(minV.getX(), worldVertex.getX()),
                fmin(minV.getY(), worldVertex.getY()),
                fmin(minV.getZ(), worldVertex.getZ())
            );
            maxV = Vector(
                fmax(maxV.getX(), worldVertex.getX()),
                fmax(maxV.getY(), worldVertex.getY()),
                fmax(maxV.getZ(), worldVertex.getZ())
            );
        }
    }

    return AABB(minV, maxV);
}

void CylinderShape::render() const {
    // Draw a solid cylinder using OpenGL
    // Cylinder is aligned with Z-axis, centered at origin

    glColor3f(color.getX(), color.getY(), color.getZ());

    // Number of slices (segments around the circumference)
    const int slices = 32;
    const int stacks = 1;

    double h2 = height * 0.5;

    // Save current matrix
    glPushMatrix();

    // OpenGL's glutSolidCylinder draws along positive Z from origin
    // We want it centered, so translate down by half height first
    glTranslated(0.0, 0.0, -h2);

    // Create a quadric object for rendering
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Draw cylinder body
    gluCylinder(quad, radius, radius, height, slices, stacks);

    // Draw bottom cap (at z = 0 in local cylinder coordinates, which is -h2 in our frame)
    gluQuadricOrientation(quad, GLU_INSIDE);
    gluDisk(quad, 0.0, radius, slices, 1);

    // Draw top cap (at z = height in local cylinder coordinates, which is +h2 in our frame)
    glTranslated(0.0, 0.0, height);
    gluQuadricOrientation(quad, GLU_OUTSIDE);
    gluDisk(quad, 0.0, radius, slices, 1);

    // Clean up
    gluDeleteQuadric(quad);
    glPopMatrix();
}

double CylinderShape::getCharacteristicSize() const {
    // Return the distance from center to the farthest point (top edge)
    // This is the diagonal from center to the top circle edge
    // diagonal = sqrt(r^2 + (h/2)^2)
    // But we want the full diagonal, so it's sqrt(r^2 + (h/2)^2) * 2 from one side to another
    // Actually, the farthest two points are: (r, 0, h/2) and (-r, 0, -h/2)
    // Distance = sqrt((2r)^2 + h^2) = sqrt(4r^2 + h^2)

    return sqrt(4.0 * radius * radius + height * height);
}

void CylinderShape::getKeyVertices(Vector vertices[8]) const {
    // Get 8 key vertices for AABB calculation
    // 4 vertices on top circle (at z = +h/2)
    // 4 vertices on bottom circle (at z = -h/2)
    // Vertices are placed at 0, 90, 180, 270 degrees around the circle

    double h2 = height * 0.5;

    // Bottom circle vertices (z = -h/2)
    vertices[0] = Vector(radius, 0.0, -h2);      // 0 degrees
    vertices[1] = Vector(0.0, radius, -h2);      // 90 degrees
    vertices[2] = Vector(-radius, 0.0, -h2);     // 180 degrees
    vertices[3] = Vector(0.0, -radius, -h2);     // 270 degrees

    // Top circle vertices (z = +h/2)
    vertices[4] = Vector(radius, 0.0, h2);       // 0 degrees
    vertices[5] = Vector(0.0, radius, h2);       // 90 degrees
    vertices[6] = Vector(-radius, 0.0, h2);      // 180 degrees
    vertices[7] = Vector(0.0, -radius, h2);      // 270 degrees
}

} /* namespace std */
