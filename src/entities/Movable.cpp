/**
 * Movable.cpp - Base class for moving entities implementation
 *
 * Created: 14/10/2012
 * Author: thaylo
 */

#include "entities/Movable.h"

Movable::Movable()
    : position(0, 0, 0), velocity(0, 0, 0), acceleration(0, 0, 0), dir(1, 0, 0),
      side(0, 1, 0), up(0, 0, 1), roll(0), pitch(0), yaw(0), v_roll(0),
      v_pitch(0), v_yaw(0) {}

Movable::Movable(const Vector &positionv)
    : position(positionv), velocity(0, 0, 0), acceleration(0, 0, 0),
      dir(1, 0, 0), side(0, 1, 0), up(0, 0, 1), roll(0), pitch(0), yaw(0),
      v_roll(0), v_pitch(0), v_yaw(0) {}

Vector Movable::getPosition() const { return position; }
Vector Movable::getVelocity() const { return velocity; }
Vector Movable::getAcceleration() const { return acceleration; }

Vector Movable::getUp() const { return up; }
Vector Movable::getDir() const { return dir; }
Vector Movable::getSide() const { return side; }

void Movable::setPosition(const Vector &pos) { position = pos; }
void Movable::setVelocity(const Vector &vel) { velocity = vel; }
void Movable::setAcceleration(const Vector &accel) { acceleration = accel; }
void Movable::setUp(const Vector &upSet) { up = upSet; }

void Movable::setRoll(double rollRef) { roll = rollRef; }
void Movable::setPitch(double pitchRef) { pitch = pitchRef; }
void Movable::setYaw(double yawRef) { yaw = yawRef; }

double Movable::getRoll() const { return roll; }
double Movable::getPitch() const { return pitch; }
double Movable::getYaw() const { return yaw; }

void Movable::setVRoll(double vrollRef) { v_roll = vrollRef; }
void Movable::setVPitch(double vpitchRef) { v_pitch = vpitchRef; }
void Movable::setVYaw(double vyawRef) { v_yaw = vyawRef; }

void Movable::iterate() {
  // Clamp velocity to maximum
  if (velocity.getLengthVector() > MOVABLE_MAX_VELOCITY) {
    velocity.setVectorLength(MOVABLE_MAX_VELOCITY);
  }

  // Update angular state
  double timeScale = TIME_STEP / 500.0;
  roll += v_roll * timeScale;
  pitch += v_pitch * timeScale;
  yaw += v_yaw * timeScale;

  // Update position and velocity
  double deltaTime = TIME_STEP / 1000.0;
  position = position + velocity * deltaTime;
  velocity = velocity + acceleration * deltaTime -
             velocity * MOVABLE_LINEAR_FRICTION * deltaTime;

  // Update orientation vectors
  up = Vector(0, 0, 1);
  side = side.rotateVector(up, v_yaw * deltaTime).setVectorLength(1.0);
  dir = up.crossProduct(side).setVectorLength(1.0);
}
