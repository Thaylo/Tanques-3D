/**
 * GameData.h - Game state management
 *
 * Uses modern C++ features: unique_ptr, vector, STL algorithms
 * Renderer-agnostic (no OpenGL dependencies)
 */

#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "core/Constants.h"
#include "entities/Agent.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include "game/Control.h"

#include <algorithm>
#include <memory>
#include <vector>

// Forward declarations to avoid OpenGL dependencies
class Ground;
class Camera;

/**
 * Main game state container managing all entities, input, and rendering.
 * Uses smart pointers for automatic memory management.
 */
class GameData {
private:
  Control control;
  Agent *player; // Non-owning pointer to player in agents vector
  std::vector<std::unique_ptr<Agent>> agents;
  int gameState;
  int numEnemies;

public:
  GameData();
  ~GameData() = default; // unique_ptr handles cleanup

  void initializeGame(int enemies);
  void setControl(const Control &ctrl);
  Control *getControl();
  void iterateGameData();

  size_t getAgentCount() const { return agents.size(); }

  // Provide read-only access to agents for iteration
  const std::vector<std::unique_ptr<Agent>> &getAgentsVector() const {
    return agents;
  }

  // Returns raw pointers for Agent iteration
  std::vector<Agent *> getAgentPointers() const;

  // Get player position for camera
  Agent *getPlayer() const { return player; }

  // Game state
  [[nodiscard]] int getGameState() const { return gameState; }
};

#endif // GAMEDATA_H
