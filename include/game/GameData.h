/**
 * GameData.h - Game state management
 *
 * Uses modern C++ features: unique_ptr, vector, STL algorithms
 */

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "core/Constants.h"
#include "entities/Agent.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include "game/Control.h"
#include "rendering/Camera.h"
#include "rendering/GLDraw.h"
#include "rendering/Ground.h"

#include <algorithm>
#include <memory>
#include <vector>

// OpenGL material/lighting globals
extern GLfloat mat_specular[];
extern GLfloat mat_shininess[];
extern GLfloat light_position[];

/**
 * Main game state container managing all entities, input, and rendering.
 * Uses smart pointers for automatic memory management.
 */
class GameData {
private:
  Control control;
  Agent *player; // Non-owning pointer to player in agents vector
  std::vector<std::unique_ptr<Agent>> agents;
  std::unique_ptr<Ground> ground;
  std::unique_ptr<Camera> camera;
  int gameState;

public:
  GameData();
  ~GameData() = default; // unique_ptr handles cleanup

  void insertPlayer();
  Control *getControl();
  void iterateGameData();
  void drawGame();

  size_t getAgentCount() const { return agents.size(); }

  // Provide read-only access to agents for iteration
  const std::vector<std::unique_ptr<Agent>> &getAgentsVector() const {
    return agents;
  }

  // Legacy compatibility - returns raw pointers for Agent iteration
  std::vector<Agent *> getAgentPointers() const;
};

#endif // GAMEDATA_H
