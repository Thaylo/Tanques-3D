/**
 * EvolutionManager.h - Genetic algorithm for neural network evolution
 */

#ifndef EVOLUTION_MANAGER_H
#define EVOLUTION_MANAGER_H

#include "ai/NeuralNetwork.h"
#include "training/TrainingArena.h"
#include <memory>
#include <vector>

namespace Training {

/**
 * EvolutionManager - Manages population evolution across generations
 */
class EvolutionManager {
public:
  EvolutionManager(int populationSize = 100);

  // Run one generation (arena battles + evolution)
  void runGeneration(float roundDuration);

  // Get best network
  const AI::NeuralNetwork &getBest() const;

  // Save/load best network
  void saveBest(const std::string &path) const;
  bool loadBest(const std::string &path);

  // Generation stats
  int getGeneration() const { return generation_; }
  float getBestFitness() const { return bestFitness_; }
  float getAverageFitness() const { return avgFitness_; }

private:
  std::vector<AI::NeuralNetwork> population_;
  TrainingArena arena_;
  std::mt19937 rng_;

  int generation_ = 0;
  float bestFitness_ = 0;
  float avgFitness_ = 0;

  // Evolution parameters
  int eliteCount_ = 10;
  float mutationRate_ = 0.1f;
  float mutationStrength_ = 0.3f;

  void evolve();
  void sortByFitness();
};

} // namespace Training

#endif // EVOLUTION_MANAGER_H
