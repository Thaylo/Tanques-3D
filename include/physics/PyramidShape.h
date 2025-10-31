/*
 * PyramidShape.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Square pyramid shape for solid objects (base in XY plane, apex on +Z axis)
 */

#ifndef PYRAMIDSHAPE_H_
#define PYRAMIDSHAPE_H_

#include "Shape.h"
#include "Quaternion.h"

namespace std {

class PyramidShape : public Shape {
private:
    double baseWidth;   // Width of square base
    double height;      // Height from base to apex

public:
    PyramidShape(double base, double h);

    // Getters
    double getBaseWidth() const { return baseWidth; }
    double getHeight() const { return height; }

    // Shape interface implementation
    virtual Matrix3x3 calculateInertiaTensor(double mass) const;
    virtual AABB getAABB(const Vector &position, const Quaternion &orientation) const;
    virtual void render() const;
    virtual double getCharacteristicSize() const;

    // Collision support - get vertices in local coordinates
    // 5 vertices: 4 base corners + 1 apex
    void getVertices(Vector vertices[5]) const;

    virtual ~PyramidShape() {}
};

} /* namespace std */

#endif /* PYRAMIDSHAPE_H_ */
