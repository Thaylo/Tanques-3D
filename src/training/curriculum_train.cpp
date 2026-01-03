/**
 * Curriculum Training - Single Agent Skill Development
 *
 * Trains ONE agent through 3 phases of increasing complexity:
 *   Phase 1: Shooting accuracy (hit stationary targets)
 *   Phase 2: Zone navigation (survive shrinking zone)
 *   Phase 3: Combined skills (shoot + navigate)
 *
 * Exports baseline brain for seeding Battle Royale population.
 */

#include "ai/BattleRoyaleNN.h"
#include "training/CurriculumArena.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <random>

using namespace Training;
using namespace AI;

// Evolution parameters for single-agent training
constexpr int TRIALS_PER_GENERATION = 20;
constexpr float MUTATION_RATE = 0.15f;
constexpr float MUTATION_STRENGTH = 0.3f;

// Phase graduation thresholds
constexpr float PHASE1_THRESHOLD = 0.5f; // 50% accuracy
constexpr float PHASE2_THRESHOLD = 5.0f; // 5 seconds net in zone
constexpr float PHASE3_THRESHOLD = 0.4f; // 40% combined score

// Training speed
constexpr float SIM_SPEED = 50.0f;
constexpr float DT = 1.0f / 60.0f;

// Window
constexpr int WINDOW_SIZE = 500;

// Export file
const char *BASELINE_FILE = "curriculum_baseline.bin";

class CurriculumTrainer {
public:
  void run() {
    if (!initSDL())
      return;

    brain_.randomize(rng_);
    bestBrain_ = brain_;

    std::cout << "=== Curriculum Training ===" << std::endl;
    std::cout << "Phase 1: Shooting Accuracy" << std::endl;
    std::cout << "Press W to watch, ESC to exit" << std::endl;
    std::cout << std::endl;

    bool running = true;
    bool watchMode = false;

    while (running) {
      SDL_Event e;
      while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
          running = false;
        if (e.type == SDL_KEYDOWN) {
          if (e.key.keysym.sym == SDLK_ESCAPE)
            running = false;
          if (e.key.keysym.sym == SDLK_w) {
            watchMode = !watchMode;
            std::cout << (watchMode ? ">>> WATCH MODE <<<"
                                    : ">>> FAST TRAINING <<<")
                      << std::endl;
          }
          if (e.key.keysym.sym == SDLK_s) {
            saveBaseline();
            std::cout << "Baseline saved!" << std::endl;
          }
        }
      }

      // Run one generation
      float fitness = runGeneration(watchMode);

      // Check if current brain is better
      if (fitness > bestFitness_) {
        bestFitness_ = fitness;
        bestBrain_ = brain_;
        consecutiveNoImprovement_ = 0;
      } else {
        consecutiveNoImprovement_++;
        // Revert to best and try new mutation
        brain_ = bestBrain_;
        brain_.mutate(MUTATION_RATE, MUTATION_STRENGTH, rng_);
      }

      generation_++;

      // Check phase graduation
      checkPhaseAdvance();

      // Update window title
      char title[256];
      snprintf(title, sizeof(title),
               "Phase %d | Gen %d | Fitness: %.3f | Best: %.3f",
               static_cast<int>(arena_.getPhase()), generation_, fitness,
               bestFitness_);
      SDL_SetWindowTitle(window_, title);

      // Log progress every 100 generations
      if (generation_ % 100 == 0) {
        std::cout << "Gen " << generation_ << " | Phase "
                  << static_cast<int>(arena_.getPhase())
                  << " | Best: " << bestFitness_ << std::endl;
      }

      if (!watchMode)
        SDL_Delay(1);
    }

    saveBaseline();
    std::cout << "Training complete! Baseline saved to " << BASELINE_FILE
              << std::endl;

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }

private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;

  CurriculumArena arena_;
  BattleRoyaleNN brain_;
  BattleRoyaleNN bestBrain_;
  std::mt19937 rng_{std::random_device{}()};

  int generation_ = 0;
  float bestFitness_ = -999.0f;
  int consecutiveNoImprovement_ = 0;

  bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
      return false;
    window_ = SDL_CreateWindow("Curriculum Training", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE,
                               SDL_WINDOW_SHOWN);
    if (!window_)
      return false;
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    return renderer_ != nullptr;
  }

  float runGeneration(bool render) {
    float totalFitness = 0.0f;

    for (int trial = 0; trial < TRIALS_PER_GENERATION; trial++) {
      arena_.reset();
      arena_.getAgent().brain = &brain_;

      while (arena_.step(DT)) {
        if (render) {
          renderFrame();
          SDL_Delay(16);
        }
      }

      totalFitness += arena_.getFitness();
    }

    return totalFitness / TRIALS_PER_GENERATION;
  }

  void renderFrame() {
    // Dark background
    SDL_SetRenderDrawColor(renderer_, 20, 20, 30, 255);
    SDL_RenderClear(renderer_);

    float scale = WINDOW_SIZE / CurriculumArena::ARENA_SIZE;
    float offset = WINDOW_SIZE / 2.0f;

    // Draw zone (if applicable)
    auto phase = arena_.getPhase();
    if (phase == CurriculumArena::Phase::NAVIGATION ||
        phase == CurriculumArena::Phase::COMBINED) {
      // Zone center and radius would need accessors - skip for now
    }

    // Draw agent
    auto &agent = arena_.getAgent();
    int ax = static_cast<int>(agent.x * scale + offset);
    int ay = static_cast<int>(-agent.y * scale + offset);

    SDL_SetRenderDrawColor(renderer_, 0, 200, 100, 255);
    SDL_Rect agentRect = {ax - 5, ay - 5, 10, 10};
    SDL_RenderFillRect(renderer_, &agentRect);

    // Draw direction
    int dx = static_cast<int>(std::cos(agent.angle) * 12);
    int dy = static_cast<int>(-std::sin(agent.angle) * 12);
    SDL_RenderDrawLine(renderer_, ax, ay, ax + dx, ay + dy);

    SDL_RenderPresent(renderer_);
  }

  void checkPhaseAdvance() {
    auto phase = arena_.getPhase();

    if (phase == CurriculumArena::Phase::SHOOTING &&
        bestFitness_ >= PHASE1_THRESHOLD) {
      std::cout << "\n*** PHASE 1 COMPLETE! Moving to Zone Navigation ***\n"
                << std::endl;
      arena_.setPhase(CurriculumArena::Phase::NAVIGATION);
      bestFitness_ = -999.0f;
      generation_ = 0;
    } else if (phase == CurriculumArena::Phase::NAVIGATION &&
               bestFitness_ >= PHASE2_THRESHOLD) {
      std::cout << "\n*** PHASE 2 COMPLETE! Moving to Combined Skills ***\n"
                << std::endl;
      arena_.setPhase(CurriculumArena::Phase::COMBINED);
      bestFitness_ = -999.0f;
      generation_ = 0;
    } else if (phase == CurriculumArena::Phase::COMBINED &&
               bestFitness_ >= PHASE3_THRESHOLD) {
      std::cout << "\n*** ALL PHASES COMPLETE! Baseline ready. ***\n"
                << std::endl;
      saveBaseline();
    }
  }

  void saveBaseline() {
    std::ofstream file(BASELINE_FILE, std::ios::binary);
    if (!file) {
      std::cerr << "Failed to save baseline!" << std::endl;
      return;
    }

    auto data = bestBrain_.serialize();
    file.write(reinterpret_cast<const char *>(data.data()),
               data.size() * sizeof(float));
  }
};

int main() {
  CurriculumTrainer trainer;
  trainer.run();
  return 0;
}
