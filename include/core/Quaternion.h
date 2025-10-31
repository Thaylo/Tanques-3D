/*
 * Quaternion.h
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 *
 *  Quaternion class for 3D rotations (avoids gimbal lock)
 *  Quaternion: q = w + xi + yj + zk
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "Vector.h"
#include "Matrix3x3.h"

namespace std {

class Quaternion {
private:
    double w;  // Real part
    double x, y, z;  // Imaginary parts (i, j, k components)

public:
    // Constructors
    Quaternion();
    Quaternion(double w, double x, double y, double z);

    // Factory methods
    static Quaternion identity();
    static Quaternion fromAxisAngle(const Vector &axis, double angle);
    static Quaternion fromEuler(double roll, double pitch, double yaw);

    // Accessors
    double getW() const { return w; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }

    void set(double w_, double x_, double y_, double z_);

    // Quaternion operations
    Quaternion operator+(const Quaternion &q) const;
    Quaternion operator-(const Quaternion &q) const;
    Quaternion operator*(const Quaternion &q) const;  // Hamilton product
    Quaternion operator*(double scalar) const;

    double dot(const Quaternion &q) const;
    double norm() const;
    double length() const { return norm(); }

    Quaternion normalize() const;
    Quaternion conjugate() const;
    Quaternion inverse() const;

    // Rotation operations
    Vector rotate(const Vector &v) const;  // Rotate vector by this quaternion
    Matrix3x3 toRotationMatrix() const;
    void toAxisAngle(Vector &axis, double &angle) const;

    // Integration for physics (angular velocity integration)
    // dq/dt = 0.5 * [ω * q] where ω is angular velocity
    Quaternion integrate(const Vector &angularVelocity, double dt) const;

    // Utilities
    void print() const;

    ~Quaternion() {}
};

} /* namespace std */

#endif /* QUATERNION_H_ */
