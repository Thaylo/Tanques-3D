/*
 * CylinderShape.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Cylinder shape for solid objects (axis aligned with Z-axis in local coordinates)
 */

#ifndef CYLINDERSHAPE_H_
#define CYLINDERSHAPE_H_

#include "Shape.h"
#include "Quaternion.h"

namespace std {

class CylinderShape : public Shape {
private:
    double radius;
    double height;

public:
    CylinderShape(double r, double h);

    // Getters
    double getRadius() const { return radius; }
    double getHeight() const { return height; }

    // Shape interface implementation
    virtual Matrix3x3 calculateInertiaTensor(double mass) const;
    virtual AABB getAABB(const Vector &position, const Quaternion &orientation) const;
    virtual void render() const;
    virtual double getCharacteristicSize() const;

    // Collision support - get key vertices for AABB calculation
    // Returns 8 vertices: 4 on top circle, 4 on bottom circle
    void getKeyVertices(Vector vertices[8]) const;

    virtual ~CylinderShape() {}
};

} /* namespace std */

#endif /* CYLINDERSHAPE_H_ */
