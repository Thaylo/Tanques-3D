/**
 * BattleRoyale.cpp - Battle Royale arena implementation
 */

#include "training/BattleRoyale.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace Training {

// Physics constants
constexpr float MAX_SPEED = 25.0f;
constexpr float ACCELERATION = 50.0f;
constexpr float TURN_RATE = 2.5f;
constexpr float PROJECTILE_SPEED = 150.0f;
constexpr float PROJECTILE_RANGE = 100.0f;
constexpr float PROJECTILE_DAMAGE = 25.0f;
constexpr float RELOAD_TIME = 1.0f;

// ============== BRAgent ==============

void BRAgent::reset(float startX, float startY, float startAngle) {
  x = startX;
  y = startY;
  angle = startAngle;
  vx = vy = 0;
  health = 100.0f;
  reloadTimer = 0.0f;
  damageDealt = 0.0f;
  kills = 0;
  shotsHit = 0;
  shotsFired = 0;
  timeAlive = 0.0f;
  placement = 0;
  alive = true;
}

float BRAgent::getFitness() const {
  float fitness = 0.0f;

  // Placement is king (inverse - lower is better)
  fitness += (50 - placement) * 100.0f;

  // Combat bonuses
  fitness += kills * 200.0f;
  fitness += damageDealt * 2.0f;

  // Accuracy matters (if they shot at all)
  if (shotsFired >= 5) {
    float accuracy = static_cast<float>(shotsHit) / shotsFired;
    fitness += accuracy * 150.0f;
  }

  // Survival time bonus
  fitness += timeAlive * 1.0f;

  return std::max(0.0f, fitness);
}

std::array<float, AI::BR_INPUT_SIZE>
BRAgent::getInputs(const std::vector<BRAgent> &allAgents, float zoneX,
                   float zoneY, float zoneRadius, float zoneShrinkTimer,
                   float maxZoneRadius) const {

  std::array<float, AI::BR_INPUT_SIZE> input;
  input.fill(0.0f);

  // Find 3 closest enemies
  std::vector<std::pair<float, size_t>> enemies;
  for (size_t i = 0; i < allAgents.size(); ++i) {
    if (&allAgents[i] == this || !allAgents[i].alive)
      continue;
    float dx = allAgents[i].x - x;
    float dy = allAgents[i].y - y;
    float dist = std::sqrt(dx * dx + dy * dy);
    enemies.push_back({dist, i});
  }
  std::sort(enemies.begin(), enemies.end());

  // Populate enemy inputs (3 closest)
  for (int e = 0; e < 3 && e < static_cast<int>(enemies.size()); ++e) {
    const auto &enemy = allAgents[enemies[e].second];
    float dx = enemy.x - x;
    float dy = enemy.y - y;
    float dist = enemies[e].first;

    // Angle to enemy (self-centered, CCW from facing direction)
    float angleToEnemy = std::atan2(dy, dx) - angle;
    while (angleToEnemy > M_PI)
      angleToEnemy -= 2 * M_PI;
    while (angleToEnemy < -M_PI)
      angleToEnemy += 2 * M_PI;

    int base = e * 6;
    input[base + 0] = std::min(dist / 200.0f, 1.0f);           // Distance
    input[base + 1] = angleToEnemy / static_cast<float>(M_PI); // Angle
    input[base + 2] = enemy.vx / MAX_SPEED;                    // Enemy vel X
    input[base + 3] = enemy.vy / MAX_SPEED;                    // Enemy vel Y
    input[base + 4] = enemy.health / 100.0f;                   // Enemy health
    input[base + 5] = enemy.reloadTimer / RELOAD_TIME;         // Enemy reload
  }

  // Self state
  input[AI::BRInput::SELF_AZIMUTH] = angle / static_cast<float>(M_PI);
  float speed = std::sqrt(vx * vx + vy * vy);
  input[AI::BRInput::SELF_SPEED] = speed / MAX_SPEED;
  input[AI::BRInput::SELF_HEADING] =
      std::atan2(vy, vx) / static_cast<float>(M_PI);
  input[AI::BRInput::SELF_HEALTH] = health / 100.0f;
  input[AI::BRInput::SELF_RELOAD] = reloadTimer / RELOAD_TIME;

  // Safe zone inputs
  float dzX = zoneX - x;
  float dzY = zoneY - y;
  float distToCenter = std::sqrt(dzX * dzX + dzY * dzY);
  float distToEdge = zoneRadius - distToCenter;

  input[AI::BRInput::ZONE_DIST] = std::clamp(distToEdge / 100.0f, -1.0f, 1.0f);
  input[AI::BRInput::ZONE_DIR] =
      std::atan2(dzY, dzX) / static_cast<float>(M_PI);
  input[AI::BRInput::ZONE_RADIUS] = zoneRadius / maxZoneRadius;
  input[AI::BRInput::ZONE_TIMER] = zoneShrinkTimer / 5.0f;

  return input;
}

// ============== SafeZone ==============

void SafeZone::update(float dt) {
  shrinkTimer -= dt;

  if (shrinkTimer <= 0 && radius > minRadius) {
    // Start shrinking
    targetRadius = std::max(minRadius, radius * 0.8f);
    shrinkTimer = shrinkInterval;
  }

  // Shrink toward target
  if (radius > targetRadius) {
    radius -= shrinkSpeed * dt;
    if (radius < targetRadius)
      radius = targetRadius;
  }
}

bool SafeZone::isInside(float px, float py) const {
  float dx = px - centerX;
  float dy = py - centerY;
  return (dx * dx + dy * dy) <= (radius * radius);
}

float SafeZone::distanceToEdge(float px, float py) const {
  float dx = px - centerX;
  float dy = py - centerY;
  float distToCenter = std::sqrt(dx * dx + dy * dy);
  return radius - distToCenter;
}

float SafeZone::angleToCenter(float fromX, float fromY) const {
  return std::atan2(centerY - fromY, centerX - fromX);
}

// ============== BattleRoyaleArena ==============

BattleRoyaleArena::BattleRoyaleArena() : rng_(std::random_device{}()) {
  agents_.resize(AGENT_COUNT);
}

void BattleRoyaleArena::reset() {
  zone_ = SafeZone();
  zone_.radius = 200.0f;
  zone_.targetRadius = 200.0f;
  projectiles_.clear();
  elapsedTime_ = 0;
  roundOver_ = false;
  nextPlacement_ = AGENT_COUNT;

  spawnAgents();
}

void BattleRoyaleArena::spawnAgents() {
  std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
  std::uniform_real_distribution<float> radiusDist(50.0f, zone_.radius * 0.8f);

  for (auto &agent : agents_) {
    float spawnAngle = angleDist(rng_);
    float spawnRadius = radiusDist(rng_);
    float startX = std::cos(spawnAngle) * spawnRadius;
    float startY = std::sin(spawnAngle) * spawnRadius;
    float facing = angleDist(rng_);

    agent.reset(startX, startY, facing);

    // Random color for visualization
    agent.color = (rng_() & 0xFFFFFF) | 0x404040; // Ensure visible
  }
}

int BattleRoyaleArena::getAliveCount() const {
  int count = 0;
  for (const auto &a : agents_) {
    if (a.alive)
      count++;
  }
  return count;
}

bool BattleRoyaleArena::step(float dt) {
  if (roundOver_)
    return false;

  elapsedTime_ += dt;

  // Check win conditions
  int alive = getAliveCount();
  if (alive <= 1 || elapsedTime_ >= MAX_ROUND_TIME) {
    // Assign final placements
    for (auto &agent : agents_) {
      if (agent.alive && agent.placement == 0) {
        agent.placement = 1; // Winner
      }
    }
    roundOver_ = true;
    return false;
  }

  // Update zone
  zone_.update(dt);

  // Apply zone damage
  applyZoneDamage(dt);

  // Process each agent
  for (auto &agent : agents_) {
    if (agent.alive) {
      agent.timeAlive = elapsedTime_;
      processAgent(agent, dt);
    }
  }

  // Process projectiles
  processProjectiles(dt);
  checkProjectileHits();

  return true;
}

void BattleRoyaleArena::applyZoneDamage(float dt) {
  for (auto &agent : agents_) {
    if (!agent.alive)
      continue;

    if (!zone_.isInside(agent.x, agent.y)) {
      agent.health -= zone_.damagePerSecond * dt;

      if (agent.health <= 0) {
        agent.alive = false;
        agent.placement = nextPlacement_--;
      }
    }
  }
}

void BattleRoyaleArena::processAgent(BRAgent &agent, float dt) {
  if (!agent.brain)
    return;

  // Get neural network inputs
  auto input = agent.getInputs(agents_, zone_.centerX, zone_.centerY,
                               zone_.radius, zone_.shrinkTimer, 200.0f);

  // Forward pass
  auto output = agent.brain->forward(input);
  float turnDir = output[0];     // -1 to 1
  float throttle = output[1];    // 0 to 1
  float shouldShoot = output[2]; // 0 to 1

  // Apply turning
  agent.angle += turnDir * TURN_RATE * dt;

  // Apply acceleration
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

  // Reload timer
  agent.reloadTimer = std::max(0.0f, agent.reloadTimer - dt);

  // Shooting
  if (shouldShoot > 0.5f && agent.reloadTimer <= 0) {
    agent.shotsFired++;
    agent.reloadTimer = RELOAD_TIME;

    // Create projectile
    Projectile p;
    p.x = agent.x;
    p.y = agent.y;
    p.vx = std::cos(agent.angle) * PROJECTILE_SPEED;
    p.vy = std::sin(agent.angle) * PROJECTILE_SPEED;
    p.lifetime = PROJECTILE_RANGE / PROJECTILE_SPEED;
    p.shooterId = std::distance(agents_.data(), &agent);
    projectiles_.push_back(p);
  }
}

void BattleRoyaleArena::processProjectiles(float dt) {
  for (auto &p : projectiles_) {
    if (!p.active)
      continue;

    p.x += p.vx * dt;
    p.y += p.vy * dt;
    p.lifetime -= dt;

    if (p.lifetime <= 0) {
      p.active = false;
    }
  }

  // Remove dead projectiles
  projectiles_.erase(
      std::remove_if(projectiles_.begin(), projectiles_.end(),
                     [](const Projectile &p) { return !p.active; }),
      projectiles_.end());
}

void BattleRoyaleArena::checkProjectileHits() {
  constexpr float HIT_RADIUS = 3.0f;

  for (auto &p : projectiles_) {
    if (!p.active)
      continue;

    for (size_t i = 0; i < agents_.size(); ++i) {
      auto &target = agents_[i];
      if (!target.alive || static_cast<int>(i) == p.shooterId)
        continue;

      float dx = target.x - p.x;
      float dy = target.y - p.y;
      float dist = std::sqrt(dx * dx + dy * dy);

      if (dist < HIT_RADIUS) {
        target.health -= PROJECTILE_DAMAGE;
        agents_[p.shooterId].damageDealt += PROJECTILE_DAMAGE;
        agents_[p.shooterId].shotsHit++;
        p.active = false;

        if (target.health <= 0) {
          target.alive = false;
          target.placement = nextPlacement_--;
          agents_[p.shooterId].kills++;
        }
        break;
      }
    }
  }
}

} // namespace Training
