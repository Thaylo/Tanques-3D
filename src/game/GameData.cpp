/**
 * GameData.cpp - Game state management implementation
 *
 * Uses modern C++ features: unique_ptr, vector, STL algorithms
 */

#include "game/GameData.h"

#include <algorithm>
#include <iostream>

// Level is set from command line
extern int level;

GameData::GameData() : player(nullptr), gameState(0) {
  control = initializeControl();

  insertPlayer();

  // Spawn enemies using generate_n pattern
  agents.reserve(level + 10); // Pre-allocate for efficiency

  std::generate_n(std::back_inserter(agents), level, [this]() {
    auto enemy = std::make_unique<Enemy>(player);
    enemy->setId(0);
    enemy->setPosition(
        Vector((rand() % 100) / 4.0 - 25.0, (rand() % 100) / 4.0 - 25.0, 0.0));
    return enemy;
  });
}

void GameData::insertPlayer() {
  auto playerAgent = std::make_unique<Agent>(Vector(0, 0, 0.0));
  playerAgent->setController(&control);
  playerAgent->setId(PLAYER_ID);
  player = playerAgent.get(); // Store raw pointer before moving
  agents.push_back(std::move(playerAgent));

  camera = std::make_unique<Camera>(player);
  ground = std::make_unique<Ground>(player);
}

Control *GameData::getControl() { return &control; }

std::vector<Agent *> GameData::getAgentPointers() const {
  std::vector<Agent *> ptrs;
  ptrs.reserve(agents.size());

  std::transform(
      agents.begin(), agents.end(), std::back_inserter(ptrs),
      [](const std::unique_ptr<Agent> &agent) { return agent.get(); });

  return ptrs;
}

void GameData::iterateGameData() {
  ground->iterate();
  camera->iterate();

  // Collect new projectiles to add after iteration (avoid modifying during
  // iteration)
  std::vector<std::unique_ptr<Agent>> newProjectiles;

  // Iterate all agents
  std::for_each(agents.begin(), agents.end(), [&newProjectiles](auto &agent) {
    agent->iterate();

    // Check if agent fired a projectile
    if (agent->checkFired()) {
      auto projectile = std::make_unique<Projectile>(agent.get());
      projectile->setId(agent->getId());
      newProjectiles.push_back(std::move(projectile));
    }
  });

  // Add new projectiles
  std::move(newProjectiles.begin(), newProjectiles.end(),
            std::back_inserter(agents));

  // Check for player death before removing agents
  auto playerDied =
      std::any_of(agents.begin(), agents.end(), [](const auto &agent) {
        if (agent->isMarkedForDestruction() && agent->getId() == PLAYER_ID) {
          // Check if it's not a projectile
          return dynamic_cast<Projectile *>(agent.get()) == nullptr;
        }
        return false;
      });

  if (playerDied) {
    std::cout << "Game Over: LOSER!" << std::endl;
    getControl()->keyEsc = true;
  }

  // Remove destroyed agents using erase-remove idiom
  agents.erase(std::remove_if(agents.begin(), agents.end(),
                              [](const std::unique_ptr<Agent> &agent) {
                                return agent->isMarkedForDestruction();
                              }),
               agents.end());

  // Check win condition: only player remains
  bool playerOnly =
      agents.size() == 1 &&
      std::all_of(agents.begin(), agents.end(), [](const auto &agent) {
        return agent->getId() == PLAYER_ID &&
               dynamic_cast<Projectile *>(agent.get()) == nullptr;
      });

  if (playerOnly) {
    std::cout << "Game Over: WINNER!" << std::endl;
    getControl()->keyEsc = true;
  }
}

void GameData::drawGame() {
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  // Position camera
  camera->applyTransform();

  // Draw world
  ground->draw();

  // Draw all agents using range-based for
  for (const auto &agent : agents) {
    agent->draw();
  }

  glPopMatrix();
}
