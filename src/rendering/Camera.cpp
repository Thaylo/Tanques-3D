/**
 * Camera.cpp - Camera positioning and tracking implementation
 *
 * Coordinate System: OpenGL/GLUT with Z-up
 * - X: Forward/back direction
 * - Y: Left/right (side) direction
 * - Z: Up/down direction
 */

#include "rendering/Camera.h"

Camera::Camera() : Movable(), tracked(nullptr) {
  // Initialize with Z-up coordinate system (consistent with rest of codebase)
  Movable::position = Vector(0, 0, 1);
  velocity = Vector(0, 0, -1);
  dir = Vector(1, 0, 0);  // Forward is +X
  up = Vector(0, 0, 1);   // Up is +Z (Z-up coordinate system)
  side = Vector(0, 1, 0); // Side is +Y
}

Camera::Camera(Movable *track) : Movable(), tracked(track) {
  // Initialize with Z-up coordinate system (consistent with rest of codebase)
  Movable::position = Vector(0, 0, 1);
  velocity = Vector(0, 0, -1);
  dir = Vector(1, 0, 0);  // Forward is +X
  up = Vector(0, 0, 1);   // Up is +Z (Z-up coordinate system)
  side = Vector(0, 1, 0); // Side is +Y
}

void Camera::draw() {
  Vector pos = getPosition();
  glTranslatef(pos.getX(), pos.getY(), pos.getZ());
  glColor3f(0, 0, 1);
  drawRectangle(-0.1, -0.1, 0.1, 0.1, 0);
  glTranslatef(-pos.getX(), -pos.getY(), -pos.getZ());
}

void Camera::applyTransform() {
  Vector pos = getPosition();
  Vector targetPos = tracked->getPosition();
  gluLookAt(pos.getX(), pos.getY(), pos.getZ(), targetPos.getX(),
            targetPos.getY(), targetPos.getZ(), up.getX(), up.getY(),
            up.getZ());
}

void Camera::iterate() {
  constexpr double factor = 0.6;

  // Position camera behind and above the tracked object
  Vector trackedDir = tracked->getDir();
  trackedDir.setVectorLength(factor);

  Vector trackedVel = tracked->getVelocity();
  trackedVel.setVectorLength(factor);

  // Camera follows behind (-dir), offset by velocity, raised in Z (up)
  Movable::position = tracked->getPosition() - trackedDir - trackedVel +
                      Vector(0, 0, factor); // Z offset for height

  // Update camera orientation based on tracked object
  Vector newDir = tracked->getDir();
  newDir.setVectorLength(1.0);

  Vector velContrib = tracked->getVelocity();
  velContrib.setVectorLength(factor);

  dir = newDir + velContrib;
  up = tracked->getUp(); // Inherits Z-up from tracked object
  side = dir.crossProduct(up).setVectorLength(1.0);
}

Camera::~Camera() {}
