/**
 * train_ai.cpp - Standalone AI training executable
 *
 * Runs evolutionary training in headless mode.
 * Usage: ./TrainAI [generations] [population] [round_duration]
 */

#include "training/EvolutionManager.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

int main(int argc, char *argv[]) {
  // Default parameters
  int generations = 100;
  int population = 100;
  float roundDuration = 30.0f; // seconds

  if (argc > 1)
    generations = std::atoi(argv[1]);
  if (argc > 2)
    population = std::atoi(argv[2]);
  if (argc > 3)
    roundDuration = std::atof(argv[3]);

  std::cout << "=== Tanques3D AI Training ===" << std::endl;
  std::cout << "Generations: " << generations << std::endl;
  std::cout << "Population:  " << population << std::endl;
  std::cout << "Round time:  " << roundDuration << "s" << std::endl;
  std::cout << std::endl;

  Training::EvolutionManager evo(population);

  auto startTime = std::chrono::high_resolution_clock::now();

  for (int gen = 0; gen < generations; ++gen) {
    evo.runGeneration(roundDuration);

    // Progress output every 10 generations
    if (gen % 10 == 0 || gen == generations - 1) {
      std::cout << "Gen " << std::setw(4) << gen << " | Best: " << std::setw(8)
                << std::fixed << std::setprecision(1) << evo.getBestFitness()
                << " | Avg: " << std::setw(8) << evo.getAverageFitness()
                << std::endl;
    }
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

  std::cout << std::endl;
  std::cout << "Training complete in " << duration.count() << " seconds"
            << std::endl;
  std::cout << "Final best fitness: " << evo.getBestFitness() << std::endl;

  // Save best network
  evo.saveBest("best_weights.bin");

  return 0;
}
