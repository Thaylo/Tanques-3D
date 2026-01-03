/**
 * EvolutionManager.cpp - Genetic algorithm implementation
 */

#include "training/EvolutionManager.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace Training {

EvolutionManager::EvolutionManager(int populationSize)
    : arena_(populationSize), rng_(std::random_device{}()) {

  population_.resize(populationSize);

  // Initialize random population
  for (auto &nn : population_) {
    nn.randomize(rng_);
  }
}

void EvolutionManager::runGeneration(float roundDuration) {
  auto &agents = arena_.getAgents();

  // Assign brains to agents
  for (size_t i = 0; i < agents.size(); ++i) {
    agents[i].brain = &population_[i];
  }

  // Run arena simulation
  arena_.runRound(roundDuration);

  // Copy fitness scores back to networks
  for (size_t i = 0; i < agents.size(); ++i) {
    population_[i].fitness = agents[i].getFitness();
  }

  // Evolution
  evolve();

  // Update stats
  bestFitness_ = arena_.getBestFitness();
  avgFitness_ = arena_.getAverageFitness();
  generation_++;
}

void EvolutionManager::sortByFitness() {
  std::sort(population_.begin(), population_.end(),
            [](const AI::NeuralNetwork &a, const AI::NeuralNetwork &b) {
              return a.fitness > b.fitness;
            });
}

void EvolutionManager::evolve() {
  sortByFitness();

  int popSize = population_.size();
  std::vector<AI::NeuralNetwork> newPop;
  newPop.reserve(popSize);

  // Elite preservation (top performers survive unchanged)
  for (int i = 0; i < eliteCount_ && i < popSize; ++i) {
    newPop.push_back(population_[i]);
  }

  // Tournament selection + crossover for the rest
  std::uniform_int_distribution<int> tournamentDist(0, popSize / 2);

  while (newPop.size() < static_cast<size_t>(popSize)) {
    // Select two parents via tournament
    int p1 = tournamentDist(rng_);
    int p2 = tournamentDist(rng_);

    // Crossover
    AI::NeuralNetwork child =
        AI::NeuralNetwork::crossover(population_[p1], population_[p2], rng_);

    // Mutation
    child.mutate(mutationRate_, mutationStrength_, rng_);

    newPop.push_back(std::move(child));
  }

  population_ = std::move(newPop);
}

const AI::NeuralNetwork &EvolutionManager::getBest() const {
  return population_.front();
}

void EvolutionManager::saveBest(const std::string &path) const {
  std::ofstream file(path, std::ios::binary);
  if (!file)
    return;

  auto data = getBest().serialize();
  file.write(reinterpret_cast<const char *>(data.data()),
             data.size() * sizeof(float));

  std::cout << "Saved best network to " << path << std::endl;
}

bool EvolutionManager::loadBest(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file)
    return false;

  std::vector<float> data(AI::TOTAL_WEIGHTS);
  file.read(reinterpret_cast<char *>(data.data()), data.size() * sizeof(float));

  population_[0].deserialize(data);
  return true;
}

} // namespace Training
