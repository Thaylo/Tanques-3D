/**
 * Ground.h - Ground/terrain rendering
 */

#ifndef GROUND_H
#define GROUND_H

#include "entities/Agent.h"
#include "rendering/GLDraw.h"

/**
 * Ground plane that follows a tracked agent.
 */
class Ground : public Movable {
private:
  Agent *tracked;

public:
  Ground();
  explicit Ground(Agent *a);
  virtual ~Ground();

  void draw();
  void iterate() override;
};

#endif // GROUND_H
