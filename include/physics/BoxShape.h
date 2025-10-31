/*
 * BoxShape.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Box (cuboid) shape for solid objects
 */

#ifndef BOXSHAPE_H_
#define BOXSHAPE_H_

#include "Shape.h"
#include "Quaternion.h"

namespace std {

class BoxShape : public Shape {
private:
    double width;   // X dimension
    double height;  // Y dimension
    double depth;   // Z dimension

public:
    BoxShape(double w, double h, double d);

    // Getters
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    double getDepth() const { return depth; }

    // Shape interface implementation
    virtual Matrix3x3 calculateInertiaTensor(double mass) const;
    virtual AABB getAABB(const Vector &position, const Quaternion &orientation) const;
    virtual void render() const;
    virtual double getCharacteristicSize() const;

    // Collision support - get vertices in local coordinates
    void getVertices(Vector vertices[8]) const;

    virtual ~BoxShape() {}
};

} /* namespace std */

#endif /* BOXSHAPE_H_ */
