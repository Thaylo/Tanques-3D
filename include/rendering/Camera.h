/**
 * Camera.h - Camera positioning and tracking
 */

#ifndef CAMERA_H
#define CAMERA_H

#include "entities/Movable.h"
#include "rendering/GLDraw.h"

/**
 * Camera that tracks a Movable object.
 */
class Camera : public Movable {
private:
  Movable *tracked;

public:
  Camera();
  explicit Camera(Movable *track);
  virtual ~Camera();

  void draw();
  void applyTransform(); // Position the camera for rendering
  void iterate() override;
};

#endif // CAMERA_H
