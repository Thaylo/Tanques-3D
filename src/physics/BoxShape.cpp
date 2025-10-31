/*
 * BoxShape.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "BoxShape.h"
#include <cmath>

namespace std {

BoxShape::BoxShape(double w, double h, double d)
    : Shape(), width(w), height(h), depth(d) {
}

Matrix3x3 BoxShape::calculateInertiaTensor(double mass) const {
    // Inertia tensor for a box (cuboid) about its center of mass
    // Ixx = (1/12) * m * (h^2 + d^2)
    // Iyy = (1/12) * m * (w^2 + d^2)
    // Izz = (1/12) * m * (w^2 + h^2)

    double Ixx = (mass / 12.0) * (height * height + depth * depth);
    double Iyy = (mass / 12.0) * (width * width + depth * depth);
    double Izz = (mass / 12.0) * (width * width + height * height);

    return Matrix3x3::diagonal(Ixx, Iyy, Izz);
}

AABB BoxShape::getAABB(const Vector &position, const Quaternion &orientation) const {
    // Get all 8 vertices and find min/max
    Vector vertices[8];
    getVertices(vertices);

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

void BoxShape::render() const {
    // Draw a solid box using OpenGL
    double w2 = width * 0.5;
    double h2 = height * 0.5;
    double d2 = depth * 0.5;

    glColor3f(color.getX(), color.getY(), color.getZ());

    glBegin(GL_QUADS);

    // Front face
    glNormal3f(0, 0, 1);
    glVertex3f(-w2, -h2,  d2);
    glVertex3f( w2, -h2,  d2);
    glVertex3f( w2,  h2,  d2);
    glVertex3f(-w2,  h2,  d2);

    // Back face
    glNormal3f(0, 0, -1);
    glVertex3f(-w2, -h2, -d2);
    glVertex3f(-w2,  h2, -d2);
    glVertex3f( w2,  h2, -d2);
    glVertex3f( w2, -h2, -d2);

    // Top face
    glNormal3f(0, 1, 0);
    glVertex3f(-w2,  h2, -d2);
    glVertex3f(-w2,  h2,  d2);
    glVertex3f( w2,  h2,  d2);
    glVertex3f( w2,  h2, -d2);

    // Bottom face
    glNormal3f(0, -1, 0);
    glVertex3f(-w2, -h2, -d2);
    glVertex3f( w2, -h2, -d2);
    glVertex3f( w2, -h2,  d2);
    glVertex3f(-w2, -h2,  d2);

    // Right face
    glNormal3f(1, 0, 0);
    glVertex3f( w2, -h2, -d2);
    glVertex3f( w2,  h2, -d2);
    glVertex3f( w2,  h2,  d2);
    glVertex3f( w2, -h2,  d2);

    // Left face
    glNormal3f(-1, 0, 0);
    glVertex3f(-w2, -h2, -d2);
    glVertex3f(-w2, -h2,  d2);
    glVertex3f(-w2,  h2,  d2);
    glVertex3f(-w2,  h2, -d2);

    glEnd();
}

double BoxShape::getCharacteristicSize() const {
    // Return the diagonal of the box (maximum distance between any two vertices)
    return sqrt(width * width + height * height + depth * depth);
}

void BoxShape::getVertices(Vector vertices[8]) const {
    // Get the 8 vertices of the box in local coordinates (centered at origin)
    double w2 = width * 0.5;
    double h2 = height * 0.5;
    double d2 = depth * 0.5;

    vertices[0] = Vector(-w2, -h2, -d2);  // Bottom-left-back
    vertices[1] = Vector( w2, -h2, -d2);  // Bottom-right-back
    vertices[2] = Vector( w2,  h2, -d2);  // Top-right-back
    vertices[3] = Vector(-w2,  h2, -d2);  // Top-left-back
    vertices[4] = Vector(-w2, -h2,  d2);  // Bottom-left-front
    vertices[5] = Vector( w2, -h2,  d2);  // Bottom-right-front
    vertices[6] = Vector( w2,  h2,  d2);  // Top-right-front
    vertices[7] = Vector(-w2,  h2,  d2);  // Top-left-front
}

} /* namespace std */
