/*
 * SphereShape.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Sphere shape for solid objects
 */

#ifndef SPHERESHAPE_H_
#define SPHERESHAPE_H_

#include "Shape.h"
#include "Quaternion.h"

namespace std {

class SphereShape : public Shape {
private:
    double radius;

public:
    SphereShape(double r);

    // Getter
    double getRadius() const { return radius; }

    // Shape interface implementation
    virtual Matrix3x3 calculateInertiaTensor(double mass) const;
    virtual AABB getAABB(const Vector &position, const Quaternion &orientation) const;
    virtual void render() const;
    virtual double getCharacteristicSize() const;

    virtual ~SphereShape() {}
};

} /* namespace std */

#endif /* SPHERESHAPE_H_ */
