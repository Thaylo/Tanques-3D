/**
 * Camera.cpp - Camera positioning and tracking implementation
 */

#include "rendering/Camera.h"

Camera::Camera() : Movable(), tracked(nullptr) {
  Movable::position = Vector(0, 0, 1);
  velocity = Vector(0, 0, -1);
  dir = Vector(1, 0, 0);
  up = Vector(0, 1, 0);
  side = Vector(0, 0, 1);
}

Camera::Camera(Movable *track) : Movable(), tracked(track) {
  Movable::position = Vector(0, 0, 1);
  velocity = Vector(0, 0, -1);
  dir = Vector(1, 0, 0);
  up = Vector(0, 1, 0);
  side = Vector(0, 0, 1);
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
  double factor = 0.6;

  // Position camera behind and above the tracked object
  Vector trackedDir = tracked->getDir();
  trackedDir.setVectorLength(factor);

  Vector trackedVel = tracked->getVelocity();
  trackedVel.setVectorLength(factor);

  Movable::position =
      tracked->getPosition() - trackedDir - trackedVel + Vector(0, 0, factor);

  // Update camera orientation based on tracked object
  Vector newDir = tracked->getDir();
  newDir.setVectorLength(1.0);

  Vector velContrib = tracked->getVelocity();
  velContrib.setVectorLength(factor);

  dir = newDir + velContrib;
  up = tracked->getUp();
  side = dir.crossProduct(up).setVectorLength(1.0);
}

Camera::~Camera() {}
