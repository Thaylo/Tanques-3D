/**
 * TrainingArena.cpp - Headless arena simulation implementation
 */

#include "training/TrainingArena.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace Training {

// Physics constants (matching game)
constexpr float MAX_SPEED = 25.0f;
constexpr float ACCELERATION = 50.0f;
constexpr float TURN_RATE = 2.0f;
constexpr float PROJECTILE_SPEED = 200.0f;
constexpr float PROJECTILE_RANGE = 300.0f;
constexpr float RELOAD_TIME = 1.0f;
constexpr float PROJECTILE_DAMAGE = 30.0f;

void TrainingAgent::reset(float startX, float startY, float startAngle) {
  x = startX;
  y = startY;
  angle = startAngle;
  vx = vy = 0;
  health = 100.0f;
  reloadTimer = 0.0f;
  damageDealt = 0.0f;
  kills = 0;
  survivalTime = 0.0f;
  alive = true;
}

float TrainingAgent::getFitness() const {
  // Aggressive fitness function: KILL OR BE KILLED
  float fitness = 0.0f;

  fitness += kills * 500.0f;      // HUGE kill bonus
  fitness += damageDealt * 5.0f;  // Damage dealt bonus
  fitness += survivalTime * 0.5f; // Minor survival bonus

  if (alive) {
    fitness += 200.0f; // Alive bonus
  } else {
    fitness -= 200.0f; // Death penalty
  }

  // Bonus for efficient kills (kills per survival time)
  if (survivalTime > 0) {
    fitness += (kills / survivalTime) * 100.0f;
  }

  return std::max(0.0f, fitness);
}

TrainingArena::TrainingArena(int populationSize)
    : rng_(std::random_device{}()) {
  agents_.resize(populationSize);
}

void TrainingArena::spawnAgents() {
  std::uniform_real_distribution<float> posDist(-arenaSize_ / 2,
                                                arenaSize_ / 2);
  std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);

  for (auto &agent : agents_) {
    agent.reset(posDist(rng_), posDist(rng_), angleDist(rng_));
  }
}

void TrainingArena::runRound(float maxDuration) {
  spawnAgents();

  constexpr float dt = 0.02f; // 50 Hz (matching game)
  float elapsed = 0;
  int aliveCount = agents_.size();

  while (elapsed < maxDuration && aliveCount > 1) {
    simulateStep(dt);
    elapsed += dt;

    // Count alive agents
    aliveCount = 0;
    for (auto &agent : agents_) {
      if (agent.alive) {
        agent.survivalTime = elapsed;
        aliveCount++;
      }
    }
  }
}

void TrainingArena::simulateStep(float dt) {
  // Process each agent's neural network and movement
  for (size_t i = 0; i < agents_.size(); ++i) {
    auto &agent = agents_[i];
    if (!agent.alive || !agent.brain)
      continue;

    // Find nearest enemy
    float nearestDist = 1e9f;
    TrainingAgent *nearest = nullptr;

    for (size_t j = 0; j < agents_.size(); ++j) {
      if (i == j || !agents_[j].alive)
        continue;
      float dx = agents_[j].x - agent.x;
      float dy = agents_[j].y - agent.y;
      float dist = std::sqrt(dx * dx + dy * dy);
      if (dist < nearestDist) {
        nearestDist = dist;
        nearest = &agents_[j];
      }
    }

    if (!nearest)
      continue;

    // Prepare neural network input
    float dx = nearest->x - agent.x;
    float dy = nearest->y - agent.y;
    float angleToTarget = std::atan2(dy, dx) - agent.angle;

    // Normalize angle to [-π, π]
    while (angleToTarget > M_PI)
      angleToTarget -= 2 * M_PI;
    while (angleToTarget < -M_PI)
      angleToTarget += 2 * M_PI;

    std::array<float, AI::INPUT_SIZE> input = {
        nearestDist / arenaSize_,                 // Normalized distance
        angleToTarget / static_cast<float>(M_PI), // Normalized angle
        nearest->vx / MAX_SPEED,                  // Target velocity X
        nearest->vy / MAX_SPEED,                  // Target velocity Y
        agent.vx / MAX_SPEED,                     // My velocity X
        agent.vy / MAX_SPEED,                     // My velocity Y
        agent.health / 100.0f,                    // My health
        nearest->health / 100.0f                  // Target health
    };

    // Run neural network
    auto output = agent.brain->forward(input);
    float turnDir = output[0];     // -1 to 1
    float throttle = output[1];    // 0 to 1
    float shouldShoot = output[2]; // 0 to 1

    // Apply decisions
    agent.angle += turnDir * TURN_RATE * dt;

    // Acceleration in facing direction
    float ax = std::cos(agent.angle) * throttle * ACCELERATION;
    float ay = std::sin(agent.angle) * throttle * ACCELERATION;
    agent.vx += ax * dt;
    agent.vy += ay * dt;

    // Clamp speed
    float speed = std::sqrt(agent.vx * agent.vx + agent.vy * agent.vy);
    if (speed > MAX_SPEED) {
      agent.vx = agent.vx / speed * MAX_SPEED;
      agent.vy = agent.vy / speed * MAX_SPEED;
    }

    // Update position
    agent.x += agent.vx * dt;
    agent.y += agent.vy * dt;

    // Arena bounds
    agent.x = std::max(-arenaSize_ / 2, std::min(arenaSize_ / 2, agent.x));
    agent.y = std::max(-arenaSize_ / 2, std::min(arenaSize_ / 2, agent.y));

    // Shooting
    agent.reloadTimer -= dt;
    if (shouldShoot > 0.5f && agent.reloadTimer <= 0) {
      if (checkHit(agent, *nearest)) {
        nearest->health -= PROJECTILE_DAMAGE;
        agent.damageDealt += PROJECTILE_DAMAGE;

        if (nearest->health <= 0) {
          nearest->alive = false;
          agent.kills++;
        }
      }
      agent.reloadTimer = RELOAD_TIME;
    }
  }
}

bool TrainingArena::checkHit(const TrainingAgent &shooter,
                             const TrainingAgent &target) {
  // Simple hitscan with accuracy check
  float dx = target.x - shooter.x;
  float dy = target.y - shooter.y;
  float dist = std::sqrt(dx * dx + dy * dy);

  if (dist > PROJECTILE_RANGE)
    return false;

  float angleToTarget = std::atan2(dy, dx);
  float angleDiff = std::abs(angleToTarget - shooter.angle);
  while (angleDiff > M_PI)
    angleDiff = 2 * M_PI - angleDiff;

  // Must be aimed within ~10 degrees
  return angleDiff < 0.17f;
}

float TrainingArena::getBestFitness() const {
  float best = 0;
  for (const auto &agent : agents_) {
    best = std::max(best, agent.getFitness());
  }
  return best;
}

float TrainingArena::getAverageFitness() const {
  float sum = 0;
  for (const auto &agent : agents_) {
    sum += agent.getFitness();
  }
  return sum / agents_.size();
}

} // namespace Training
