/**
 * BattleRoyale.cpp - Battle Royale arena implementation
 */

#include "training/BattleRoyale.h"
#include <algorithm>
#include <cmath>
#include <dispatch/dispatch.h> // GCD for M1 parallel processing

namespace Training {

// Physics constants
constexpr float MAX_SPEED = 25.0f;
constexpr float ACCELERATION = 50.0f;
constexpr float TURN_RATE = 2.5f;
constexpr float FRICTION =
    5.0f; // Ground friction - tanks stop when not accelerating
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
  // =============================================================
  // EXPONENTIAL SURVIVAL FITNESS
  // exp(timeAlive/30) - surviving longer matters exponentially more
  // =============================================================
  return std::exp(timeAlive / 30.0f);
}

std::array<float, AI::BR_INPUT_SIZE>
BRAgent::getInputs(const std::vector<BRAgent> &allAgents, float zoneX,
                   float zoneY, float zoneRadius, float zoneShrinkTimer,
                   float maxZoneRadius) const {

  std::array<float, AI::BR_INPUT_SIZE> input;
  input.fill(0.0f);

  // =====================================================
  // OPTIMIZED: Find 5 closest enemies using fixed array
  // =====================================================
  struct EnemyDist {
    float distSq;
    size_t idx;
  };

  // Fixed-size array on stack (no allocation)
  std::array<EnemyDist, 50> enemyDists;
  int enemyCount = 0;

  for (size_t i = 0; i < allAgents.size() && enemyCount < 50; ++i) {
    if (&allAgents[i] == this || !allAgents[i].alive)
      continue;
    float dx = allAgents[i].x - x;
    float dy = allAgents[i].y - y;
    float distSq = dx * dx + dy * dy; // Avoid sqrt!
    enemyDists[enemyCount++] = {distSq, i};
  }

  // Partial sort: find 5 closest (O(N) vs O(N log N))
  int topK = std::min(5, enemyCount);
  std::partial_sort(enemyDists.begin(), enemyDists.begin() + topK,
                    enemyDists.begin() + enemyCount,
                    [](const EnemyDist &a, const EnemyDist &b) {
                      return a.distSq < b.distSq;
                    });

  // Populate enemy inputs (5 closest) - ALL IN AGENT'S LOCAL FRAME
  // Use CACHED trig values (precomputed in step())
  float localCos = cosNegAngle;
  float localSin = sinNegAngle;

  for (int e = 0; e < topK; ++e) {
    const auto &enemy = allAgents[enemyDists[e].idx];

    // Position delta in world coords
    float dx = enemy.x - x;
    float dy = enemy.y - y;
    float dist = std::sqrt(enemyDists[e].distSq);

    // Transform position to local frame (forward = +X, left = +Y)
    float localX = dx * localCos - dy * localSin; // Forward/back
    float localY = dx * localSin + dy * localCos; // Left/right

    // Transform enemy velocity to local frame
    float localVx =
        enemy.vx * localCos - enemy.vy * localSin; // Forward component
    float localVy =
        enemy.vx * localSin + enemy.vy * localCos; // Sideways component

    int base = e * 6;
    // Position: localX > 0 means enemy is ahead, localY > 0 means to the left
    input[base + 0] = std::clamp(localX / 200.0f, -1.0f, 1.0f); // Forward dist
    input[base + 1] = std::clamp(localY / 200.0f, -1.0f, 1.0f); // Side dist
    input[base + 2] = localVx / MAX_SPEED;             // Enemy forward vel
    input[base + 3] = localVy / MAX_SPEED;             // Enemy side vel
    input[base + 4] = enemy.health / 100.0f;           // Enemy health
    input[base + 5] = enemy.reloadTimer / RELOAD_TIME; // Enemy reload
  }

  // Self state
  input[AI::BRInput::SELF_AZIMUTH] = angle / static_cast<float>(M_PI);
  float speed = std::sqrt(vx * vx + vy * vy);
  input[AI::BRInput::SELF_SPEED] = speed / MAX_SPEED;
  input[AI::BRInput::SELF_HEADING] =
      std::atan2(vy, vx) / static_cast<float>(M_PI);
  input[AI::BRInput::SELF_HEALTH] = health / 100.0f;
  input[AI::BRInput::SELF_RELOAD] = reloadTimer / RELOAD_TIME;

  // =============================================================
  // SAFE ZONE AWARENESS
  // Designed for intuitive learning:
  //   ZONE_DIST: +1.0 = at center (very safe)
  //               0.0 = at edge (danger imminent)
  //              -1.0 = far outside (taking damage!)
  //   ZONE_DIR:  Direction to flee toward safety (self-centered)
  // =============================================================

  float dzX = zoneX - x;
  float dzY = zoneY - y;
  float distToCenter = std::sqrt(dzX * dzX + dzY * dzY);
  float distToEdge = zoneRadius - distToCenter;

  // Normalized safety margin: scale-invariant as zone shrinks
  // +1 = at zone center, 0 = at edge, negative = outside
  float safetyMargin = (zoneRadius > 0) ? (distToEdge / zoneRadius) : 0.0f;
  input[AI::BRInput::ZONE_DIST] = std::clamp(safetyMargin, -1.0f, 1.0f);

  // Zone radius normalized
  input[AI::BRInput::ZONE_RADIUS] = zoneRadius / maxZoneRadius;

  // Zone center in local frame (X only for simplified input)
  float localZoneX = dzX * cosNegAngle - dzY * sinNegAngle;
  input[AI::BRInput::ZONE_CENTER_X] =
      std::clamp(localZoneX / 300.0f, -1.0f, 1.0f);

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
  // Initialize octree for 300x300 arena (with Z for future 3D)
  octree_ = std::make_unique<Spatial::Octree<size_t>>(
      Spatial::AABB(-150, -150, -100, 150, 150, 100));
}

void BattleRoyaleArena::reset() {
  zone_ = SafeZone();
  zone_.radius = 150.0f; // Smaller zone for 25 agents
  zone_.targetRadius = 150.0f;
  projectiles_.clear();
  elapsedTime_ = 0;
  roundOver_ = false;
  nextPlacement_ = AGENT_COUNT;

  // Reset hidden states for all brains at start of round
  for (auto &agent : agents_) {
    if (agent.brain) {
      agent.brain->resetHiddenState();
    }
  }

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

  // =====================================================
  // OCTREE REBUILD - O(N) insert for O(log N) queries
  // =====================================================
  octree_->clear();
  for (size_t i = 0; i < agents_.size(); ++i) {
    if (agents_[i].alive) {
      octree_->insert(i, agents_[i].x, agents_[i].y, 0);
    }
  }

  // =====================================================
  // GCD PARALLEL AGENT PROCESSING (M1 8-core optimized)
  // Each agent: getInputs + NN forward + movement
  // =====================================================
  dispatch_queue_t queue =
      dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);

  // Capture variables for block
  float zoneX = zone_.centerX;
  float zoneY = zone_.centerY;
  float zoneRadius = zone_.radius;
  float zoneShrinkTimer = zone_.shrinkTimer;
  float maxZoneRadius = 400.0f;
  float elapsed = elapsedTime_;

  dispatch_apply(agents_.size(), queue, ^(size_t i) {
    BRAgent &agent = agents_[i];
    if (!agent.alive)
      return;

    agent.timeAlive = elapsed;
    agent.wantsToShoot = false;

    // Get inputs (uses cached trig from previous step, runs in parallel)
    auto input = agent.getInputs(agents_, zoneX, zoneY, zoneRadius,
                                 zoneShrinkTimer, maxZoneRadius);

    // NN forward pass (BLAS accelerated)
    auto output = agent.brain->forward(input);
    float turnDir = output[0];
    float throttle = output[1];
    float shouldShoot = output[2];

    // Apply turning
    agent.angle += turnDir * TURN_RATE * dt;

    // CACHE TRIG VALUES (precompute once, use many times)
    agent.cosAngle = std::cos(agent.angle);
    agent.sinAngle = std::sin(agent.angle);
    agent.cosNegAngle = agent.cosAngle;  // cos(-x) = cos(x)
    agent.sinNegAngle = -agent.sinAngle; // sin(-x) = -sin(x)

    // Apply acceleration using CACHED values
    float ax = agent.cosAngle * throttle * ACCELERATION;
    float ay = agent.sinAngle * throttle * ACCELERATION;
    agent.vx += ax * dt;
    agent.vy += ay * dt;

    // Apply ground friction (tanks stop when not accelerating)
    float speed = std::sqrt(agent.vx * agent.vx + agent.vy * agent.vy);
    if (speed > 0.01f) {
      float frictionDecel = FRICTION * dt;
      float newSpeed = std::max(0.0f, speed - frictionDecel);
      float factor = newSpeed / speed;
      agent.vx *= factor;
      agent.vy *= factor;
      speed = newSpeed;
    }

    // Clamp max speed
    if (speed > MAX_SPEED) {
      agent.vx = agent.vx / speed * MAX_SPEED;
      agent.vy = agent.vy / speed * MAX_SPEED;
    }

    // Update position
    agent.x += agent.vx * dt;
    agent.y += agent.vy * dt;

    // Reload timer
    agent.reloadTimer = std::max(0.0f, agent.reloadTimer - dt);

    // Mark for shooting (collected sequentially later)
    if (shouldShoot > 0.5f && agent.reloadTimer <= 0) {
      agent.wantsToShoot = true;
    }
  });

  // =====================================================
  // SEQUENTIAL: Collect projectiles (not thread-safe)
  // =====================================================
  for (size_t i = 0; i < agents_.size(); ++i) {
    BRAgent &agent = agents_[i];
    if (agent.alive && agent.wantsToShoot) {
      agent.shotsFired++;
      agent.reloadTimer = RELOAD_TIME;

      Projectile proj;
      proj.x = agent.x;
      proj.y = agent.y;
      proj.vx = std::cos(agent.angle) * PROJECTILE_SPEED;
      proj.vy = std::sin(agent.angle) * PROJECTILE_SPEED;
      proj.lifetime = PROJECTILE_RANGE / PROJECTILE_SPEED;
      proj.shooterId = static_cast<int>(i);
      projectiles_.push_back(proj);
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
