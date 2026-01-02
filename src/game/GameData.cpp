/**
 * GameData.cpp - Game state management implementation
 */

#include "game/GameData.h"
#include <iostream>

// Level is set from command line
extern int level;

GameData::GameData() : player(nullptr), agentCount(0), gameState(0) {
  control = initializeControl();

  insertPlayer();

  // Spawn enemies based on level
  int enemyCount = level;
  for (int i = 0; i < enemyCount; i++) {
    Enemy *e = new Enemy(player);
    e->setId(0);
    // Random spawn positions
    e->setPosition(
        Vector((rand() % 100) / 4.0 - 25.0, (rand() % 100) / 4.0 - 25.0, 0.0));
    agents[agentCount++] = e;
  }
}

void GameData::insertPlayer() {
  player = new Agent(Vector(0, 0, 0.0));
  player->setController(&control);
  player->setId(PLAYER_ID);
  agents[agentCount++] = player;

  camera = Camera(player);
  ground = Ground(player);
}

Control *GameData::getControl() { return &control; }

void GameData::iterateGameData() {
  Agent *aux;

  ground.iterate();
  camera.iterate();

  for (int i = 0; i < agentCount; i++) {
    aux = agents[i];
    aux->iterate();

    // Check if agent fired a projectile
    if (aux->checkFired()) {
      Projectile *projectile = new Projectile(aux);
      projectile->setId(aux->getId());
      agents[agentCount++] = projectile;
    }

    // Remove destroyed agents
    if (aux->isMarkedForDestruction()) {
      if (aux->getId() == PLAYER_ID) {
        Projectile *isProjectile = dynamic_cast<Projectile *>(aux);
        if (!isProjectile) {
          std::cout << "Game Over: LOSER!" << std::endl;
          getControl()->keyEsc = TRUE;
        }
      }
      delete aux;
      agents[i] = agents[agentCount - 1];
      --agentCount;
      --i;
    }
  }

  // Check win condition
  if (1 == agentCount && player == agents[0]) {
    std::cout << "Game Over: WINNER!" << std::endl;
    getControl()->keyEsc = TRUE;
  }
}

GameData::~GameData() {
  for (int i = 0; i < agentCount; i++) {
    delete agents[i];
  }
}

void GameData::drawGame() {
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  // Position camera
  camera.applyTransform();

  // Draw world
  ground.draw();

  // Draw all agents
  for (int i = 0; i < agentCount; i++) {
    agents[i]->draw();
  }

  glPopMatrix();
}
