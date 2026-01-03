#include "training/CurriculumArena.h"
#include <cmath>

namespace Training {

// Physics constants (match BattleRoyale)
constexpr float MAX_SPEED = 40.0f;
constexpr float ACCELERATION = 80.0f;
constexpr float TURN_RATE = 3.5f;
constexpr float FRICTION = 50.0f;
constexpr float PROJECTILE_SPEED = 800.0f;
constexpr float PROJECTILE_RANGE = 150.0f;
constexpr float PROJECTILE_DAMAGE = 34.0f;
constexpr float RELOAD_TIME = 0.5f;
constexpr float HIT_RADIUS = 5.0f;

CurriculumArena::CurriculumArena() : rng_(std::random_device{}()) { reset(); }

void CurriculumArena::reset() {
  // Reset agent at center
  agent_.reset(0, 0, 0);
  agent_.stamina = 100.0f;

  // Reset stats
  shotsFired_ = 0;
  shotsHit_ = 0;
  timeInZone_ = 0.0f;
  timeOutsideZone_ = 0.0f;
  elapsedTime_ = 0.0f;
  trialOver_ = false;
  projectiles_.clear();

  // Phase-specific setup
  if (phase_ == Phase::SHOOTING || phase_ == Phase::COMBINED) {
    spawnTarget();
  }

  if (phase_ == Phase::NAVIGATION || phase_ == Phase::COMBINED) {
    // Zone starts centered, agent starts at edge
    zone_ = SafeZone();
    zone_.centerX = 0;
    zone_.centerY = 0;
    zone_.radius = 80.0f;
    zone_.targetRadius = 30.0f;
    zone_.shrinkSpeed = 5.0f;
    zone_.damagePerSecond = 10.0f;

    // Start agent outside zone for navigation challenge
    float angle = std::uniform_real_distribution<float>(0, 2 * M_PI)(rng_);
    agent_.x = std::cos(angle) * 90.0f;
    agent_.y = std::sin(angle) * 90.0f;
    agent_.angle = angle + M_PI; // Face toward center
  }
}

void CurriculumArena::spawnTarget() {
  // Spawn target at random position around agent
  float angle = std::uniform_real_distribution<float>(0, 2 * M_PI)(rng_);
  float dist = std::uniform_real_distribution<float>(30.0f, 80.0f)(rng_);

  target_.x = agent_.x + std::cos(angle) * dist;
  target_.y = agent_.y + std::sin(angle) * dist;
  target_.angle = std::uniform_real_distribution<float>(0, 2 * M_PI)(rng_);
  target_.health = 34.0f;
  target_.alive = true;
  target_.vx = target_.vy = 0;
  targetAlive_ = true;

  // In combined phase, target moves slowly
  if (phase_ == Phase::COMBINED) {
    float moveAngle = std::uniform_real_distribution<float>(0, 2 * M_PI)(rng_);
    target_.vx = std::cos(moveAngle) * 10.0f;
    target_.vy = std::sin(moveAngle) * 10.0f;
  }
}

bool CurriculumArena::step(float dt) {
  if (trialOver_)
    return false;

  elapsedTime_ += dt;
  if (elapsedTime_ >= TRIAL_TIME) {
    trialOver_ = true;
    return false;
  }

  // Update zone if applicable
  if (phase_ == Phase::NAVIGATION || phase_ == Phase::COMBINED) {
    zone_.update(dt);
    applyZoneDamage(dt);
  }

  // Update target position (for combined phase)
  if (phase_ == Phase::COMBINED && targetAlive_) {
    target_.x += target_.vx * dt;
    target_.y += target_.vy * dt;

    // Bounce off arena edges
    if (std::abs(target_.x) > ARENA_SIZE / 2)
      target_.vx = -target_.vx;
    if (std::abs(target_.y) > ARENA_SIZE / 2)
      target_.vy = -target_.vy;
  }

  // Get agent inputs
  std::vector<BRAgent> allAgents = {agent_};
  if (targetAlive_) {
    allAgents.push_back(target_);
  }

  // Prepare trig caches
  agent_.cosNegAngle = std::cos(-agent_.angle);
  agent_.sinNegAngle = std::sin(-agent_.angle);

  auto input = agent_.getInputs(allAgents, zone_.centerX, zone_.centerY,
                                zone_.radius, zone_.shrinkTimer, 100.0f);

  // Forward pass
  auto output = agent_.brain->forward(input);

  // Apply outputs (same as BattleRoyale)
  float turnDir = output[0] * 2.0f - 1.0f;
  float throttle = output[1] * 1.5f - 0.5f;
  float shouldShoot = output[2];

  // Stamina system
  float turnAmount = std::abs(turnDir);
  agent_.stamina -= turnAmount * 5.0f * dt;
  if (turnAmount < 0.7f) {
    agent_.stamina += 25.0f * dt;
  }
  agent_.stamina = std::clamp(agent_.stamina, 0.0f, 100.0f);

  // Movement
  agent_.angle += turnDir * TURN_RATE * dt;
  agent_.cosAngle = std::cos(agent_.angle);
  agent_.sinAngle = std::sin(agent_.angle);

  float ax = agent_.cosAngle * throttle * ACCELERATION;
  float ay = agent_.sinAngle * throttle * ACCELERATION;
  agent_.vx += ax * dt;
  agent_.vy += ay * dt;

  float speed = std::sqrt(agent_.vx * agent_.vx + agent_.vy * agent_.vy);
  if (speed > 0.01f) {
    float frictionDecel = FRICTION * dt;
    float newSpeed = std::max(0.0f, speed - frictionDecel);
    agent_.vx *= newSpeed / speed;
    agent_.vy *= newSpeed / speed;
  }

  if (speed > MAX_SPEED) {
    agent_.vx = agent_.vx / speed * MAX_SPEED;
    agent_.vy = agent_.vy / speed * MAX_SPEED;
  }

  agent_.x += agent_.vx * dt;
  agent_.y += agent_.vy * dt;
  agent_.reloadTimer = std::max(0.0f, agent_.reloadTimer - dt);

  // Clamp to arena
  agent_.x = std::clamp(agent_.x, -ARENA_SIZE / 2, ARENA_SIZE / 2);
  agent_.y = std::clamp(agent_.y, -ARENA_SIZE / 2, ARENA_SIZE / 2);

  // Shooting
  if (shouldShoot > 0.5f && agent_.reloadTimer <= 0) {
    agent_.reloadTimer = RELOAD_TIME;
    shotsFired_++;

    Projectile proj;
    proj.x = agent_.x;
    proj.y = agent_.y;
    proj.vx = std::cos(agent_.angle) * PROJECTILE_SPEED;
    proj.vy = std::sin(agent_.angle) * PROJECTILE_SPEED;
    proj.lifetime = PROJECTILE_RANGE / PROJECTILE_SPEED;
    proj.shooterId = 0;
    projectiles_.push_back(proj);
  }

  processProjectiles(dt);
  checkProjectileHits();

  // Respawn target if killed (for continuous practice)
  if (!targetAlive_ &&
      (phase_ == Phase::SHOOTING || phase_ == Phase::COMBINED)) {
    spawnTarget();
  }

  // Track zone time
  if (phase_ == Phase::NAVIGATION || phase_ == Phase::COMBINED) {
    if (zone_.isInside(agent_.x, agent_.y)) {
      timeInZone_ += dt;
    } else {
      timeOutsideZone_ += dt;
    }
  }

  // End trial if agent dies
  if (agent_.health <= 0) {
    trialOver_ = true;
    return false;
  }

  return true;
}

void CurriculumArena::processProjectiles(float dt) {
  for (auto &p : projectiles_) {
    if (!p.active)
      continue;
    p.x += p.vx * dt;
    p.y += p.vy * dt;
    p.lifetime -= dt;
    if (p.lifetime <= 0)
      p.active = false;
  }
}

void CurriculumArena::checkProjectileHits() {
  for (auto &p : projectiles_) {
    if (!p.active || !targetAlive_)
      continue;

    float dx = target_.x - p.x;
    float dy = target_.y - p.y;
    float distSq = dx * dx + dy * dy;

    if (distSq < HIT_RADIUS * HIT_RADIUS) {
      target_.health -= PROJECTILE_DAMAGE;
      p.active = false;
      shotsHit_++;

      if (target_.health <= 0) {
        target_.alive = false;
        targetAlive_ = false;
      }
    }
  }
}

void CurriculumArena::applyZoneDamage(float dt) {
  if (!zone_.isInside(agent_.x, agent_.y)) {
    agent_.health -= zone_.damagePerSecond * dt;
  }
}

float CurriculumArena::getShootingFitness() const {
  if (shotsFired_ == 0)
    return 0.0f;
  return static_cast<float>(shotsHit_) / shotsFired_;
}

float CurriculumArena::getNavigationFitness() const {
  // Reward time in zone, penalize time outside
  return timeInZone_ - timeOutsideZone_ * 0.5f;
}

float CurriculumArena::getCombinedFitness() const {
  float shootScore = getShootingFitness();
  float navScore = std::max(0.0f, getNavigationFitness()) / TRIAL_TIME;
  return shootScore * 0.5f + navScore * 0.5f;
}

float CurriculumArena::getFitness() const {
  switch (phase_) {
  case Phase::SHOOTING:
    return getShootingFitness();
  case Phase::NAVIGATION:
    return getNavigationFitness();
  case Phase::COMBINED:
    return getCombinedFitness();
  }
  return 0.0f;
}

} // namespace Training
