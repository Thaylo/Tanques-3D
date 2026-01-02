/**
 * Agent.h - Tank agent entity
 *
 * Created: 15/10/2012
 * Author: thaylo
 *
 * Renderer-agnostic: draw() is a stub for future Vulkan implementation.
 */

#ifndef AGENT_H
#define AGENT_H

#include "entities/Controlable.h"
#include "entities/Matter.h"
#include "entities/Movable.h"
#include "rendering/Drawable.h"

/**
 * Tank agent entity that can be controlled and rendered.
 * Supports radar display for tracking nearest enemy.
 */
class Agent : public Controlable, public Drawable, public Matter {
private:
  int id;
  bool markedForDestruction;

public:
  bool isFiring;
  int reloadTimer;
  Agent *nearestEnemy;

  Agent();
  explicit Agent(Vector pos);
  virtual ~Agent();

  // Identification
  void setId(int idx);
  int getId();

  // Rendering (stub - Vulkan implementation pending)
  void draw() override;
  void drawRadar();

  // Game logic
  void iterate() override;
  void controlAction() override;

  // Combat
  virtual void shoot();
  bool checkFired();

  // Destruction
  void markForDestruction();
  bool isMarkedForDestruction();

  // Accessors for rendering
  [[nodiscard]] const Vector &getDir() const { return dir; }
  [[nodiscard]] const Vector &getUp() const { return up; }
  [[nodiscard]] const Vector &getSide() const { return side; }
};

#endif // AGENT_H
