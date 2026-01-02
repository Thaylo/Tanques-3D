/**
 * Vector.cpp - 3D Vector mathematics implementation
 *
 * Created: 09/10/2012
 * Author: thaylo
 *
 * C++23 modernization
 */

#include "core/Vector.h"

#include <cmath>
#include <iostream>
#include <numbers> // C++20 math constants

using std::cout;
using std::ostream;

Vector &Vector::setVectorLength(double length) noexcept {
  double currentLength = getLengthVector();
  if (currentLength > 0) {
    double scale = length / currentLength;
    x *= scale;
    y *= scale;
    z *= scale;
  }
  return *this;
}

Vector Vector::getNormalVector() const noexcept {
  double length = getLengthVector();
  if (length > 0) {
    return {x / length, y / length, z / length};
  }
  return {0, 0, 0};
}

double Vector::angleVectors(const Vector &v) const noexcept {
  double len1 = getLengthVector();
  double len2 = v.getLengthVector();

  if (len1 == 0 || len2 == 0) {
    return 0;
  }

  double cosAngle = dotProduct(v) / (len1 * len2);
  // Clamp to [-1, 1] to handle floating point errors
  cosAngle = std::clamp(cosAngle, -1.0, 1.0);
  return std::acos(cosAngle);
}

Vector Vector::rotateVector(const Vector &axis, double theta_rad) const {
  // Rodrigues' rotation formula
  Vector axisNorm = axis.getNormalVector();
  double cosTheta = std::cos(theta_rad);
  double sinTheta = std::sin(theta_rad);

  // v' = v*cos(θ) + (k × v)*sin(θ) + k*(k·v)*(1-cos(θ))
  Vector term1 = *this * cosTheta;
  Vector term2 = axisNorm.crossProduct(*this) * sinTheta;
  Vector term3 = axisNorm * (axisNorm.dotProduct(*this) * (1 - cosTheta));

  return term1 + term2 + term3;
}

ostream &operator<<(ostream &output, const Vector &p) {
  output << "(" << p.getX() << ", " << p.getY() << ", " << p.getZ() << ")";
  return output;
}
