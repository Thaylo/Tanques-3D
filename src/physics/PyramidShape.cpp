/*
 * PyramidShape.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "PyramidShape.h"
#include <cmath>

namespace std {

PyramidShape::PyramidShape(double base, double h)
    : Shape(), baseWidth(base), height(h) {
}

Matrix3x3 PyramidShape::calculateInertiaTensor(double mass) const {
    // Inertia tensor for a square pyramid (uniform density) about its center of mass
    // Base in XY plane (Z=0), apex at (0, 0, height)
    // For a square pyramid with base width w and height h:
    //
    // I_xx = I_yy = (1/20) * m * (w^2 + 4*h^2)  (perpendicular to base)
    // I_zz = (1/10) * m * w^2  (around vertical axis)
    //
    // Note: These formulas assume the center of mass is at the centroid,
    // which is at height h/4 from the base for a uniform pyramid

    double w2 = baseWidth * baseWidth;
    double h2 = height * height;

    double Ixx = (mass / 20.0) * (w2 + 4.0 * h2);
    double Iyy = (mass / 20.0) * (w2 + 4.0 * h2);
    double Izz = (mass / 10.0) * w2;

    return Matrix3x3::diagonal(Ixx, Iyy, Izz);
}

AABB PyramidShape::getAABB(const Vector &position, const Quaternion &orientation) const {
    // Get all 5 vertices and find min/max
    Vector vertices[5];
    getVertices(vertices);

    // Transform vertices to world space and find bounds
    Vector minV = position;
    Vector maxV = position;

    for (int i = 0; i < 5; i++) {
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

void PyramidShape::render() const {
    // Draw a solid pyramid using OpenGL
    // Base in XY plane (Z=0), apex at (0, 0, height)
    double w2 = baseWidth * 0.5;

    glColor3f(color.getX(), color.getY(), color.getZ());

    // Define vertices
    Vector base[4];
    base[0] = Vector(-w2, -w2, 0);  // Bottom-left
    base[1] = Vector( w2, -w2, 0);  // Bottom-right
    base[2] = Vector( w2,  w2, 0);  // Top-right
    base[3] = Vector(-w2,  w2, 0);  // Top-left
    Vector apex(0, 0, height);

    // Draw 4 triangular faces (base to apex)
    glBegin(GL_TRIANGLES);

    // Front face (Y-)
    Vector normal1 = (base[1] - base[0]).crossProduct(apex - base[0]).getNormalVector();
    glNormal3f(normal1.getX(), normal1.getY(), normal1.getZ());
    glVertex3f(base[0].getX(), base[0].getY(), base[0].getZ());
    glVertex3f(base[1].getX(), base[1].getY(), base[1].getZ());
    glVertex3f(apex.getX(), apex.getY(), apex.getZ());

    // Right face (X+)
    Vector normal2 = (base[2] - base[1]).crossProduct(apex - base[1]).getNormalVector();
    glNormal3f(normal2.getX(), normal2.getY(), normal2.getZ());
    glVertex3f(base[1].getX(), base[1].getY(), base[1].getZ());
    glVertex3f(base[2].getX(), base[2].getY(), base[2].getZ());
    glVertex3f(apex.getX(), apex.getY(), apex.getZ());

    // Back face (Y+)
    Vector normal3 = (base[3] - base[2]).crossProduct(apex - base[2]).getNormalVector();
    glNormal3f(normal3.getX(), normal3.getY(), normal3.getZ());
    glVertex3f(base[2].getX(), base[2].getY(), base[2].getZ());
    glVertex3f(base[3].getX(), base[3].getY(), base[3].getZ());
    glVertex3f(apex.getX(), apex.getY(), apex.getZ());

    // Left face (X-)
    Vector normal4 = (base[0] - base[3]).crossProduct(apex - base[3]).getNormalVector();
    glNormal3f(normal4.getX(), normal4.getY(), normal4.getZ());
    glVertex3f(base[3].getX(), base[3].getY(), base[3].getZ());
    glVertex3f(base[0].getX(), base[0].getY(), base[0].getZ());
    glVertex3f(apex.getX(), apex.getY(), apex.getZ());

    glEnd();

    // Draw square base
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);  // Normal pointing down (base is at Z=0)
    glVertex3f(base[0].getX(), base[0].getY(), base[0].getZ());
    glVertex3f(base[3].getX(), base[3].getY(), base[3].getZ());
    glVertex3f(base[2].getX(), base[2].getY(), base[2].getZ());
    glVertex3f(base[1].getX(), base[1].getY(), base[1].getZ());
    glEnd();
}

double PyramidShape::getCharacteristicSize() const {
    // Return the distance from base center to apex (height)
    // Alternatively, could use diagonal from base corner to apex
    // Using height as it's the most characteristic dimension
    return height;
}

void PyramidShape::getVertices(Vector vertices[5]) const {
    // Get the 5 vertices of the pyramid in local coordinates
    // Base in XY plane (Z=0), centered at origin
    // Apex at (0, 0, height)
    double w2 = baseWidth * 0.5;

    // 4 base corners
    vertices[0] = Vector(-w2, -w2, 0);  // Bottom-left
    vertices[1] = Vector( w2, -w2, 0);  // Bottom-right
    vertices[2] = Vector( w2,  w2, 0);  // Top-right
    vertices[3] = Vector(-w2,  w2, 0);  // Top-left

    // Apex
    vertices[4] = Vector(0, 0, height);
}

} /* namespace std */
