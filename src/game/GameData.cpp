/**
 * GameData.cpp - Game state management implementation
 *
 * Uses modern C++ features: unique_ptr, vector, STL algorithms
 * Renderer-agnostic: no OpenGL/Vulkan dependencies
 */

#include "game/GameData.h"

#include <algorithm>
#include <cmath>
#include <iostream>

GameData::GameData() : player(nullptr), gameState(0), numEnemies(0) {
  control = initializeControl();
}

void GameData::initializeGame(int enemies) {
  numEnemies = enemies;

  // Insert player
  auto playerAgent = std::make_unique<Agent>(Vector(0, 0, 0.0));
  playerAgent->setController(&control);
  playerAgent->setId(PLAYER_ID);
  player = playerAgent.get();
  agents.push_back(std::move(playerAgent));

  // Spawn enemies using generate_n pattern
  agents.reserve(enemies + 10);

  std::generate_n(std::back_inserter(agents), enemies, [this]() {
    auto enemy = std::make_unique<Enemy>(player);
    enemy->setId(0);
    // Spawn enemies 50-150 meters from center (SI units)
    double angle = (rand() % 360) * 3.14159 / 180.0;
    double dist = 50.0 + (rand() % 100); // 50-150 meters
    enemy->setPosition(Vector(dist * cos(angle), dist * sin(angle), 0.0));
    return enemy;
  });

  std::cout << "Game initialized with " << enemies << " enemies" << std::endl;
}

void GameData::setControl(const Control &ctrl) { control = ctrl; }

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
  // Collect new projectiles to add after iteration
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
          return dynamic_cast<Projectile *>(agent.get()) == nullptr;
        }
        return false;
      });

  if (playerDied) {
    std::cout << "Game Over: LOSER!" << std::endl;
    gameState = -1; // Loss state
    getControl()->keyEsc = true;
  }

  // Remove destroyed agents using erase-remove idiom
  agents.erase(std::remove_if(agents.begin(), agents.end(),
                              [](const std::unique_ptr<Agent> &agent) {
                                return agent->isMarkedForDestruction();
                              }),
               agents.end());

  // Check win condition: only player remains (no enemies, ignore projectiles)
  size_t nonProjectileCount =
      std::count_if(agents.begin(), agents.end(), [](const auto &agent) {
        return dynamic_cast<Projectile *>(agent.get()) == nullptr;
      });

  if (nonProjectileCount == 1 && player != nullptr) {
    std::cout << "Game Over: WINNER!" << std::endl;
    gameState = 1; // Win state
    getControl()->keyEsc = true;
  }
}
