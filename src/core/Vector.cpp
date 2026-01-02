/**
 * Vector.cpp - 3D Vector mathematics implementation
 *
 * Created: 09/10/2012
 * Author: thaylo
 */

#include "core/Vector.h"

using std::cout;
using std::ostream;

Vector::Vector() : x(0), y(0), z(0) {}

Vector::Vector(double xv, double yv, double zv) : x(xv), y(yv), z(zv) {}

Vector::~Vector() {}

ostream &operator<<(ostream &output, const Vector &p) {
  output << "(" << p.getX() << ", " << p.getY() << ", " << p.getZ() << ")";
  return output;
}

double Vector::getX() const { return x; }
double Vector::getY() const { return y; }
double Vector::getZ() const { return z; }

double Vector::setX(double xv) { return (x = xv); }
double Vector::setY(double yv) { return (y = yv); }
double Vector::setZ(double zv) { return (z = zv); }

Vector Vector::operator=(const Vector &v2) {
  x = v2.x;
  y = v2.y;
  z = v2.z;
  return *this;
}

Vector Vector::operator=(double k) {
  if (k != 0) {
    cout << "Invalid assignment of double -> Vector\n";
  }
  x = y = z = 0;
  return *this;
}

Vector Vector::operator+(const Vector &v2) const {
  return Vector(x + v2.x, y + v2.y, z + v2.z);
}

Vector Vector::operator-(const Vector &v2) const {
  return Vector(x - v2.x, y - v2.y, z - v2.z);
}

Vector Vector::operator*(double k) const { return Vector(x * k, y * k, z * k); }

Vector Vector::operator/(double k) const { return Vector(x / k, y / k, z / k); }

double Vector::getLengthVector() const { return sqrt(x * x + y * y + z * z); }

Vector Vector::setVectorLength(double length) {
  double oldLength = getLengthVector();
  if (oldLength != 0) {
    double scale = length / oldLength;
    x *= scale;
    y *= scale;
    z *= scale;
  }
  return *this;
}

double Vector::distanceVector(const Vector &v2) const {
  return (*this - v2).getLengthVector();
}

double Vector::dotProduct(const Vector &b) const {
  return x * b.x + y * b.y + z * b.z;
}

Vector Vector::getNormalVector() const {
  Vector r(-y, x, 0);
  r.setVectorLength(1.0);
  return r;
}

Vector Vector::crossProduct(const Vector &v) const {
  double i = y * v.z - z * v.y;
  double j = z * v.x - x * v.z;
  double k = x * v.y - y * v.x;
  return Vector(i, j, k);
}

double Vector::angleVectors(const Vector &v) const {
  return atan2(crossProduct(v).getLengthVector(), dotProduct(v));
}

Vector Vector::rotateVector(const Vector &axis, double phi) {
  // Rotation matrix from: http://en.wikipedia.org/wiki/Rotation_matrix
  double R[3][3];
  Vector u = axis;
  u.setVectorLength(1.0);

  double cosPhi = cos(phi);
  double sinPhi = sin(phi);
  double oneMinusCos = 1 - cosPhi;

  R[0][0] = cosPhi + u.x * u.x * oneMinusCos;
  R[0][1] = u.x * u.y * oneMinusCos - u.z * sinPhi;
  R[0][2] = u.x * u.z * oneMinusCos + u.y * sinPhi;

  R[1][0] = u.y * u.x * oneMinusCos + u.z * sinPhi;
  R[1][1] = cosPhi + u.y * u.y * oneMinusCos;
  R[1][2] = u.y * u.z * oneMinusCos - u.x * sinPhi;

  R[2][0] = u.z * u.x * oneMinusCos - u.y * sinPhi;
  R[2][1] = u.z * u.y * oneMinusCos + u.x * sinPhi;
  R[2][2] = cosPhi + u.z * u.z * oneMinusCos;

  double unrotated[3] = {x, y, z};
  double rotated[3] = {0.0, 0.0, 0.0};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      rotated[i] += R[i][j] * unrotated[j];
    }
  }

  return Vector(rotated[0], rotated[1], rotated[2]);
}
