/*
 * Quaternion.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Claude (Anthropic)
 */

#include "Quaternion.h"
#include <cmath>
#include <iostream>

namespace std {

// Constructors
Quaternion::Quaternion() : w(1.0), x(0.0), y(0.0), z(0.0) {
}

Quaternion::Quaternion(double w_, double x_, double y_, double z_)
    : w(w_), x(x_), y(y_), z(z_) {
}

// Factory methods
Quaternion Quaternion::identity() {
    return Quaternion(1.0, 0.0, 0.0, 0.0);
}

Quaternion Quaternion::fromAxisAngle(const Vector &axis, double angle) {
    // Normalize the axis
    Vector normAxis = axis;
    normAxis.setVectorLength(1.0);

    double halfAngle = angle * 0.5;
    double sinHalf = sin(halfAngle);

    return Quaternion(
        cos(halfAngle),
        normAxis.getX() * sinHalf,
        normAxis.getY() * sinHalf,
        normAxis.getZ() * sinHalf
    );
}

Quaternion Quaternion::fromEuler(double roll, double pitch, double yaw) {
    // Convert Euler angles (in radians) to quaternion
    // Order: yaw (Z), pitch (Y), roll (X)
    double cy = cos(yaw * 0.5);
    double sy = sin(yaw * 0.5);
    double cp = cos(pitch * 0.5);
    double sp = sin(pitch * 0.5);
    double cr = cos(roll * 0.5);
    double sr = sin(roll * 0.5);

    return Quaternion(
        cr * cp * cy + sr * sp * sy,
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy
    );
}

void Quaternion::set(double w_, double x_, double y_, double z_) {
    w = w_;
    x = x_;
    y = y_;
    z = z_;
}

// Quaternion operations
Quaternion Quaternion::operator+(const Quaternion &q) const {
    return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z);
}

Quaternion Quaternion::operator-(const Quaternion &q) const {
    return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z);
}

Quaternion Quaternion::operator*(const Quaternion &q) const {
    // Hamilton product: q1 * q2
    return Quaternion(
        w * q.w - x * q.x - y * q.y - z * q.z,
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w
    );
}

Quaternion Quaternion::operator*(double scalar) const {
    return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
}

double Quaternion::dot(const Quaternion &q) const {
    return w * q.w + x * q.x + y * q.y + z * q.z;
}

double Quaternion::norm() const {
    return sqrt(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::normalize() const {
    double n = norm();
    if (n < 1e-10) {
        return Quaternion::identity();
    }
    return Quaternion(w / n, x / n, y / n, z / n);
}

Quaternion Quaternion::conjugate() const {
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::inverse() const {
    double n2 = w * w + x * x + y * y + z * z;
    if (n2 < 1e-10) {
        return Quaternion::identity();
    }
    return conjugate() * (1.0 / n2);
}

// Rotation operations
Vector Quaternion::rotate(const Vector &v) const {
    // Rotate vector v by this quaternion using: v' = q * v * q^-1
    // More efficient formula: v' = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v)

    Vector qVec(x, y, z);
    Vector cross1 = qVec.crossProduct(v);
    Vector cross2 = qVec.crossProduct(cross1 + v * w);

    return v + cross2 * 2.0;
}

Matrix3x3 Quaternion::toRotationMatrix() const {
    // Convert quaternion to 3x3 rotation matrix
    double xx = x * x;
    double xy = x * y;
    double xz = x * z;
    double xw = x * w;

    double yy = y * y;
    double yz = y * z;
    double yw = y * w;

    double zz = z * z;
    double zw = z * w;

    return Matrix3x3(
        1 - 2 * (yy + zz),     2 * (xy - zw),     2 * (xz + yw),
            2 * (xy + zw), 1 - 2 * (xx + zz),     2 * (yz - xw),
            2 * (xz - yw),     2 * (yz + xw), 1 - 2 * (xx + yy)
    );
}

void Quaternion::toAxisAngle(Vector &axis, double &angle) const {
    // Convert quaternion to axis-angle representation
    Quaternion q = normalize();

    angle = 2.0 * acos(q.w);
    double sinHalf = sin(angle * 0.5);

    if (fabs(sinHalf) < 1e-6) {
        // Angle is very small, axis is arbitrary
        axis = Vector(1, 0, 0);
    } else {
        axis = Vector(q.x / sinHalf, q.y / sinHalf, q.z / sinHalf);
    }
}

Quaternion Quaternion::integrate(const Vector &angularVelocity, double dt) const {
    // Integrate angular velocity: dq/dt = 0.5 * [ω * q]
    // where [ω * q] is the quaternion product with ω as a pure quaternion (0, ωx, ωy, ωz)

    Quaternion omega(0.0, angularVelocity.getX(), angularVelocity.getY(), angularVelocity.getZ());
    Quaternion dq = (omega * (*this)) * 0.5;

    // Euler integration: q(t+dt) = q(t) + dq/dt * dt
    Quaternion result = (*this) + dq * dt;

    // Normalize to prevent drift
    return result.normalize();
}

void Quaternion::print() const {
    cout << "Quaternion(" << w << ", " << x << ", " << y << ", " << z << ")" << endl;
}

} /* namespace std */
