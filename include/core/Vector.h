/**
 * Vector.h - 3D Vector mathematics
 *
 * Created: 09/10/2012
 * Author: thaylo
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <iostream>

/**
 * A 3D vector class with common mathematical operations.
 */
class Vector {
public:
  Vector();
  Vector(double xv, double yv, double zv);
  ~Vector();

  // Getters
  double getX() const;
  double getY() const;
  double getZ() const;

  // Setters
  double setX(double xv);
  double setY(double yv);
  double setZ(double zv);

  // Operators
  Vector operator+(const Vector &v2) const;
  Vector operator-(const Vector &v2) const;
  Vector operator*(double k) const;
  Vector operator/(double k) const;
  Vector operator=(const Vector &v2);
  Vector operator=(double k);

  // Vector operations
  double getLengthVector() const;
  Vector setVectorLength(double length);
  double distanceVector(const Vector &v2) const;

  // Products
  double dotProduct(const Vector &b) const;
  Vector crossProduct(const Vector &v) const;
  Vector getNormalVector() const;
  double angleVectors(const Vector &v) const;

  // Transformations
  Vector rotateVector(const Vector &axis, double theta_rad);

private:
  double x, y, z;
};

std::ostream &operator<<(std::ostream &output, const Vector &p);

#endif // VECTOR_H
