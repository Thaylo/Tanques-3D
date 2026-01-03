#pragma once

#include "BattleRoyale.h"
#include <random>

namespace Training {

/**
 * CurriculumArena - Controlled training environment
 * Single-agent skill development in 3 phases:
 *   Phase 1: Shooting accuracy (stationary targets)
 *   Phase 2: Zone navigation (survive shrinking zone)
 *   Phase 3: Combined skills (shoot + navigate)
 */
class CurriculumArena {
public:
  enum class Phase { SHOOTING = 1, NAVIGATION = 2, COMBINED = 3 };

  // Training arena is smaller for focused practice
  static constexpr float ARENA_SIZE = 200.0f;
  static constexpr float TRIAL_TIME = 10.0f; // Max seconds per trial

  CurriculumArena();

  void setPhase(Phase phase) { phase_ = phase; }
  Phase getPhase() const { return phase_; }

  // Reset for new trial
  void reset();

  // Run one simulation step, returns false when trial ends
  bool step(float dt);

  // Get agent reference for brain assignment
  BRAgent &getAgent() { return agent_; }

  // Spawn a target for shooting practice
  void spawnTarget();

  // Fitness functions for each phase
  float getShootingFitness() const;   // Phase 1: accuracy
  float getNavigationFitness() const; // Phase 2: zone survival
  float getCombinedFitness() const;   // Phase 3: both

  // Get current fitness based on phase
  float getFitness() const;

  // Stats for fitness calculation
  int getShotsFired() const { return shotsFired_; }
  int getShotsHit() const { return shotsHit_; }
  float getTimeInZone() const { return timeInZone_; }
  float getElapsedTime() const { return elapsedTime_; }

private:
  Phase phase_ = Phase::SHOOTING;
  BRAgent agent_;

  // Target for shooting practice (dummy agent)
  BRAgent target_;
  bool targetAlive_ = true;

  // Zone for navigation practice
  SafeZone zone_;

  // Projectiles
  std::vector<Projectile> projectiles_;

  // Stats
  int shotsFired_ = 0;
  int shotsHit_ = 0;
  float timeInZone_ = 0.0f;
  float timeOutsideZone_ = 0.0f;
  float elapsedTime_ = 0.0f;
  bool trialOver_ = false;

  std::mt19937 rng_;

  void processProjectiles(float dt);
  void checkProjectileHits();
  void applyZoneDamage(float dt);
};

} // namespace Training
