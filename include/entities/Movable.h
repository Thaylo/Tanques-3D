/**
 * Movable.h - Base class for moving entities
 *
 * Created: 14/10/2012
 * Author: thaylo
 *
 * C++23 modernization: constexpr, nodiscard, noexcept
 */

#ifndef MOVABLE_H
#define MOVABLE_H

#include "core/Constants.h"
#include "core/Vector.h"

/**
 * Base class for all entities that can move in 3D space.
 * Provides position, velocity, acceleration and orientation tracking.
 */
class Movable {
protected:
  Vector position{};
  Vector velocity{};
  Vector acceleration{};

  // Orientation vectors
  Vector up{0, 0, 1};
  Vector dir{1, 0, 0};  // Forward direction
  Vector side{0, 1, 0}; // Right side direction

  // Euler angles (radians)
  double roll{0}, pitch{0}, yaw{0};
  double v_roll{0}, v_pitch{0}, v_yaw{0}; // Angular velocities

public:
  constexpr Movable() noexcept = default;

  explicit constexpr Movable(const Vector &positionv) noexcept
      : position(positionv) {}

  virtual ~Movable() = default;

  // Rule of five with defaults
  constexpr Movable(const Movable &) = default;
  constexpr Movable(Movable &&) noexcept = default;
  Movable &operator=(const Movable &) = default;
  Movable &operator=(Movable &&) noexcept = default;

  virtual void iterate();

  // Position accessors
  [[nodiscard]] constexpr Vector getPosition() const noexcept {
    return position;
  }
  [[nodiscard]] constexpr Vector getVelocity() const noexcept {
    return velocity;
  }
  [[nodiscard]] constexpr Vector getAcceleration() const noexcept {
    return acceleration;
  }

  constexpr void setPosition(const Vector &pos) noexcept { position = pos; }
  constexpr void setVelocity(const Vector &vel) noexcept { velocity = vel; }
  constexpr void setAcceleration(const Vector &accel) noexcept {
    acceleration = accel;
  }

  // Orientation accessors
  [[nodiscard]] constexpr Vector getUp() const noexcept { return up; }
  [[nodiscard]] constexpr Vector getDir() const noexcept { return dir; }
  [[nodiscard]] constexpr Vector getSide() const noexcept { return side; }

  constexpr void setUp(const Vector &upSet) noexcept { up = upSet; }

  // Euler angle controls
  constexpr void setRoll(double rollRef) noexcept { roll = rollRef; }
  constexpr void setPitch(double pitchRef) noexcept { pitch = pitchRef; }
  constexpr void setYaw(double yawRef) noexcept { yaw = yawRef; }

  [[nodiscard]] constexpr double getRoll() const noexcept { return roll; }
  [[nodiscard]] constexpr double getPitch() const noexcept { return pitch; }
  [[nodiscard]] constexpr double getYaw() const noexcept { return yaw; }

  constexpr void setVRoll(double vrollRef) noexcept { v_roll = vrollRef; }
  constexpr void setVPitch(double vpitchRef) noexcept { v_pitch = vpitchRef; }
  constexpr void setVYaw(double vyawRef) noexcept { v_yaw = vyawRef; }
};

#endif // MOVABLE_H
