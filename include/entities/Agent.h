/**
 * Agent.h - Tank agent entity
 *
 * Created: 15/10/2012
 * Author: thaylo
 */

#ifndef AGENT_H
#define AGENT_H

#include "entities/Controlable.h"
#include "entities/Matter.h"
#include "entities/Movable.h"
#include "rendering/Drawable.h"
#include "rendering/GLDraw.h"

// Tank textures (loaded externally)
extern GLuint tankTextures[5];

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

  // Rendering
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

protected:
  void glVectorT(const Vector &v);
};

#endif // AGENT_H
