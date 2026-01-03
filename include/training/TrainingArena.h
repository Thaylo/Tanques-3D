/**
 * TrainingArena.h - Headless simulation for training AI
 *
 * Fast arena simulation without graphics for evolutionary training.
 */

#ifndef TRAINING_ARENA_H
#define TRAINING_ARENA_H

#include "ai/NeuralNetwork.h"
#include <cstdint>
#include <random>
#include <vector>

namespace Training {

/**
 * TrainingAgent - Simplified agent for arena battles
 */
struct TrainingAgent {
  // Position and movement
  float x = 0, y = 0;
  float vx = 0, vy = 0;
  float angle = 0; // Facing direction (radians)

  // Combat
  float health = 100.0f;
  float reloadTimer = 0.0f;

  // Neural network brain
  AI::NeuralNetwork *brain = nullptr;

  // Fitness tracking
  float damageDealt = 0.0f;
  float rangedDamage = 0.0f; // Damage from distance > 20m
  int kills = 0;
  int shotsFired = 0;
  int shotsHit = 0;
  float survivalTime = 0.0f;
  bool alive = true;

  void reset(float startX, float startY, float startAngle);
  float getFitness() const;
};

/**
 * TrainingArena - Headless battle simulation
 */
class TrainingArena {
public:
  TrainingArena(int populationSize);

  // Run one training round
  void runRound(float maxDuration);

  // Get agents for evolution
  std::vector<TrainingAgent> &getAgents() { return agents_; }

  // Stats
  float getBestFitness() const;
  float getAverageFitness() const;

private:
  std::vector<TrainingAgent> agents_;
  std::mt19937 rng_;
  float arenaSize_ = 200.0f; // meters

  void spawnAgents();
  void simulateStep(float dt);
  void processMovement(TrainingAgent &agent, float dt);
  void processProjectiles(float dt);
  bool checkHit(const TrainingAgent &shooter, const TrainingAgent &target);
};

} // namespace Training

#endif // TRAINING_ARENA_H
