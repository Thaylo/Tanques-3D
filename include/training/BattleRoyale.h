/**
 * BattleRoyale.h - Battle Royale training arena with shrinking safe zone
 */

#ifndef BATTLE_ROYALE_H
#define BATTLE_ROYALE_H

#include "ai/BattleRoyaleNN.h"
#include "core/Octree.h"
#include <array>
#include <cmath>
#include <memory>
#include <random>
#include <vector>

namespace Training {

/**
 * BRAgent - Battle Royale agent with expanded perception
 */
struct BRAgent {
  // Position and movement
  float x = 0, y = 0;
  float vx = 0, vy = 0;
  float angle = 0; // Facing direction (radians, CCW from +X)

  // Cached trig values (precomputed once per step to avoid repeated cos/sin)
  float cosAngle = 1.0f;    // cos(angle) for forward direction
  float sinAngle = 0.0f;    // sin(angle) for forward direction
  float cosNegAngle = 1.0f; // cos(-angle) for local frame rotation
  float sinNegAngle = 0.0f; // sin(-angle) for local frame rotation

  // Combat state
  float health = 100.0f;
  float reloadTimer = 0.0f;

  // Pending projectile for thread-safe parallel processing
  bool wantsToShoot = false; // Set by parallel NN, collected sequentially

  // Neural network brain
  AI::BattleRoyaleNN *brain = nullptr;

  // Stats for fitness
  float damageDealt = 0.0f;
  int kills = 0;
  int shotsHit = 0;
  int shotsFired = 0;
  float timeAlive = 0.0f;
  int placement = 0; // Final placement (1 = winner)
  bool alive = true;

  // Visual
  uint32_t color = 0xFFFFFF;

  void reset(float startX, float startY, float startAngle);
  float getFitness() const;

  // Calculate inputs for neural network
  std::array<float, AI::BR_INPUT_SIZE>
  getInputs(const std::vector<BRAgent> &allAgents, float zoneX, float zoneY,
            float zoneRadius, float zoneShrinkTimer, float maxZoneRadius) const;
};

/**
 * Safe Zone - Shrinking circle that damages players outside
 */
struct SafeZone {
  float centerX = 0, centerY = 0;
  float radius = 200.0f;
  float targetRadius = 200.0f;
  float shrinkSpeed = 10.0f; // meters per second when shrinking
  float shrinkTimer = 5.0f;  // seconds until next shrink
  float shrinkInterval = 5.0f;
  float damagePerSecond = 2.0f;
  float minRadius = 20.0f;

  void update(float dt);
  bool isInside(float x, float y) const;
  float distanceToEdge(float x, float y) const;
  float angleToCenter(float fromX, float fromY) const;
};

/**
 * Projectile for visual feedback
 */
struct Projectile {
  float x, y;
  float vx, vy;
  float lifetime;
  int shooterId;
  bool active = true;
};

/**
 * BattleRoyaleArena - 25-tank battle with visualization
 * Uses octree for efficient spatial queries
 */
class BattleRoyaleArena {
public:
  static constexpr int AGENT_COUNT = 25;
  static constexpr float ARENA_SIZE = 300.0f;    // Smaller for 25 agents
  static constexpr float MAX_ROUND_TIME = 90.0f; // Shorter rounds

  BattleRoyaleArena();

  // Run one simulation step (call repeatedly for visual mode)
  bool step(float dt);

  // Reset for new round
  void reset();

  // Accessors for visualization
  const std::vector<BRAgent> &getAgents() const { return agents_; }
  std::vector<BRAgent> &getAgentsMutable() { return agents_; }
  const SafeZone &getZone() const { return zone_; }
  const std::vector<Projectile> &getProjectiles() const { return projectiles_; }

  // Round state
  bool isRoundOver() const { return roundOver_; }
  float getElapsedTime() const { return elapsedTime_; }
  int getAliveCount() const;

private:
  std::vector<BRAgent> agents_;
  SafeZone zone_;
  std::vector<Projectile> projectiles_;
  std::mt19937 rng_;
  std::unique_ptr<Spatial::Octree<size_t>> octree_; // Spatial partitioning

  float elapsedTime_ = 0;
  bool roundOver_ = false;
  int nextPlacement_ = AGENT_COUNT;

  void spawnAgents();
  void processAgent(BRAgent &agent, float dt);
  void processProjectiles(float dt);
  void applyZoneDamage(float dt);
  void checkProjectileHits();

  // Find N closest enemies to an agent
  std::vector<size_t> findClosestEnemies(size_t agentIdx, int count) const;
};

} // namespace Training

#endif // BATTLE_ROYALE_H
