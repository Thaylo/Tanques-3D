/**
 * Vector.h - 3D Vector mathematics
 *
 * Created: 09/10/2012
 * Author: thaylo
 *
 * C++23 modernization: constexpr, nodiscard, spaceship operator, concepts
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>
#include <compare>
#include <iostream>

/**
 * A 3D vector class with common mathematical operations.
 * Fully constexpr-enabled for compile-time computations.
 */
class Vector {
public:
  // Constructors
  constexpr Vector() noexcept : x(0), y(0), z(0) {}
  constexpr Vector(double xv, double yv, double zv) noexcept
      : x(xv), y(yv), z(zv) {}
  constexpr ~Vector() = default;

  // Copy/move
  constexpr Vector(const Vector &) = default;
  constexpr Vector(Vector &&) noexcept = default;
  constexpr Vector &operator=(const Vector &) = default;
  constexpr Vector &operator=(Vector &&) noexcept = default;

  // Getters (nodiscard encourages using return values)
  [[nodiscard]] constexpr double getX() const noexcept { return x; }
  [[nodiscard]] constexpr double getY() const noexcept { return y; }
  [[nodiscard]] constexpr double getZ() const noexcept { return z; }

  // Setters
  constexpr void setX(double xv) noexcept { x = xv; }
  constexpr void setY(double yv) noexcept { y = yv; }
  constexpr void setZ(double zv) noexcept { z = zv; }

  // Arithmetic operators
  [[nodiscard]] constexpr Vector operator+(const Vector &v2) const noexcept {
    return {x + v2.x, y + v2.y, z + v2.z};
  }

  [[nodiscard]] constexpr Vector operator-(const Vector &v2) const noexcept {
    return {x - v2.x, y - v2.y, z - v2.z};
  }

  [[nodiscard]] constexpr Vector operator*(double k) const noexcept {
    return {x * k, y * k, z * k};
  }

  [[nodiscard]] constexpr Vector operator/(double k) const noexcept {
    return {x / k, y / k, z / k};
  }

  // Compound assignment
  constexpr Vector &operator+=(const Vector &v2) noexcept {
    x += v2.x;
    y += v2.y;
    z += v2.z;
    return *this;
  }

  constexpr Vector &operator-=(const Vector &v2) noexcept {
    x -= v2.x;
    y -= v2.y;
    z -= v2.z;
    return *this;
  }

  constexpr Vector &operator*=(double k) noexcept {
    x *= k;
    y *= k;
    z *= k;
    return *this;
  }

  // Spaceship operator for comparisons (C++20/23)
  [[nodiscard]] constexpr auto operator<=>(const Vector &v2) const noexcept {
    // Compare by length squared for ordering
    double len1 = x * x + y * y + z * z;
    double len2 = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
    return len1 <=> len2;
  }

  // Equality (exact component match)
  [[nodiscard]] constexpr bool operator==(const Vector &v2) const noexcept {
    return x == v2.x && y == v2.y && z == v2.z;
  }

  // Assign scalar (zero vector)
  constexpr Vector &operator=(double k) noexcept {
    x = y = z = k;
    return *this;
  }

  // Vector operations
  [[nodiscard]] double getLengthVector() const noexcept {
    return std::sqrt(x * x + y * y + z * z);
  }

  [[nodiscard]] constexpr double getLengthSquared() const noexcept {
    return x * x + y * y + z * z;
  }

  Vector &setVectorLength(double length) noexcept;

  [[nodiscard]] double distanceVector(const Vector &v2) const noexcept {
    return (*this - v2).getLengthVector();
  }

  // Products
  [[nodiscard]] constexpr double dotProduct(const Vector &b) const noexcept {
    return x * b.x + y * b.y + z * b.z;
  }

  [[nodiscard]] constexpr Vector crossProduct(const Vector &v) const noexcept {
    return {y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x};
  }

  [[nodiscard]] Vector getNormalVector() const noexcept;
  [[nodiscard]] double angleVectors(const Vector &v) const noexcept;

  // Transformations
  [[nodiscard]] Vector rotateVector(const Vector &axis, double theta_rad) const;

private:
  double x, y, z;
};

// Non-member operators
[[nodiscard]] inline constexpr Vector operator*(double k,
                                                const Vector &v) noexcept {
  return v * k;
}

std::ostream &operator<<(std::ostream &output, const Vector &p);

#endif // VECTOR_H
