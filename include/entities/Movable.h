/**
 * Movable.h - Base class for moving entities
 *
 * Created: 14/10/2012
 * Author: thaylo
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
  Vector position;
  Vector velocity;
  Vector acceleration;

  // Orientation vectors
  Vector up;
  Vector dir;  // Forward direction
  Vector side; // Right side direction

  // Euler angles (radians)
  // See: http://www.toymaker.info/Games/assets/images/yawpitchroll.jpg
  double roll, pitch, yaw;
  double v_roll, v_pitch, v_yaw; // Angular velocities

public:
  Movable();
  explicit Movable(const Vector &positionv);
  virtual ~Movable() {}

  virtual void iterate();

  // Position getters/setters
  Vector getPosition() const;
  Vector getVelocity() const;
  Vector getAcceleration() const;
  void setPosition(const Vector &pos);
  void setVelocity(const Vector &vel);
  void setAcceleration(const Vector &accel);

  // Orientation getters/setters
  Vector getUp() const;
  Vector getDir() const;
  Vector getSide() const;
  void setUp(const Vector &upSet);

  // Euler angle controls
  void setRoll(double rollRef);
  void setPitch(double pitchRef);
  void setYaw(double yawRef);
  double getRoll() const;
  double getPitch() const;
  double getYaw() const;

  void setVRoll(double vrollRef);
  void setVPitch(double vpitchRef);
  void setVYaw(double vyawRef);
};

#endif // MOVABLE_H
