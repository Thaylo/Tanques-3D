/**
 * GameData.h - Game state management
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

#include <cstdlib>
#include <list>
#include <vector>

// OpenGL material/lighting globals
extern GLfloat mat_specular[];
extern GLfloat mat_shininess[];
extern GLfloat light_position[];

/**
 * Main game state container managing all entities, input, and rendering.
 */
class GameData {
private:
  Control control;
  Agent *player;
  Agent *agents[4000];
  int agentCount;
  Ground ground;
  Camera camera;
  int gameState;

public:
  GameData();
  ~GameData();

  void insertPlayer();
  Control *getControl();
  void iterateGameData();
  void drawGame();

  int getAgentCount() { return agentCount; }
  Agent **getAgents() { return agents; }
};

#endif // GAMEDATA_H
