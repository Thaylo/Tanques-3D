/**
 * battle_royale_train.cpp - Visual Battle Royale training
 *
 * Watch 50 tanks fight from above while evolving AI.
 * Features:
 *   - 100x speed with 50 tanks, linear to 1x with 1 tank
 *   - Cumulative learning (auto-save/load best networks)
 */

#include "training/BattleRoyale.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

using namespace Training;
using namespace AI;

// Evolution parameters
constexpr int POPULATION_SIZE = 50;
constexpr int ELITE_COUNT = 5;
constexpr int RANDOM_INJECT = 5;
constexpr float MUTATION_RATE = 0.15f;
constexpr float MUTATION_STRENGTH = 0.4f;

// Speed scaling
constexpr float MAX_SIM_SPEED = 100.0f; // Speed when 50 tanks
constexpr float MIN_SIM_SPEED = 1.0f;   // Speed when 1 tank

// Visualization
constexpr int WINDOW_SIZE = 800;
constexpr float WORLD_SIZE = 400.0f;

// Save file
const char *SAVE_FILE = "battle_royale_best.bin";
const char *POPULATION_FILE = "battle_royale_population.bin";

class BattleRoyaleTrainer {
public:
  BattleRoyaleTrainer() {
    population_.resize(POPULATION_SIZE);

    // Try to load saved population for cumulative learning
    if (!loadPopulation()) {
      std::cout << "No saved population, starting fresh" << std::endl;
      for (auto &nn : population_) {
        nn.randomize(rng_);
      }
    } else {
      std::cout << "Loaded saved population - continuing training!"
                << std::endl;
    }
  }

  void run() {
    if (!initSDL())
      return;

    bool running = true;
    bool paused = false;
    bool watchMode = false; // W key: true = 1:1 real-time, false = max speed

    while (running) {
      // Handle events
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        } else if (event.type == SDL_KEYDOWN) {
          switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            running = false;
            break;
          case SDLK_SPACE:
            paused = !paused;
            break;
          case SDLK_w:
            watchMode = !watchMode;
            std::cout << (watchMode ? ">>> WATCH MODE (1:1 speed) <<<"
                                    : ">>> MAX SPEED (training) <<<")
                      << std::endl;
            break;
          case SDLK_s:
            saveBest();
            savePopulation();
            break;
          }
        }
      }

      if (!paused) {
        if (watchMode) {
          // WATCH MODE: 1:1 real-time, easy to observe
          float dt = 0.016f; // ~60 FPS real-time

          if (!arena_.isRoundOver()) {
            arena_.step(dt);
          } else {
            evolve();
            startNewRound();
            generation_++;
            savePopulation();
            std::cout << "Gen " << generation_ << " | Best: " << bestFitness_
                      << " | Avg: " << avgFitness_ << std::endl;
          }
          currentSpeed_ = 1.0f;

        } else {
          // MAX SPEED: As fast as possible
          float dt = 0.05f; // Large timestep

          // Run many steps per frame
          for (int step = 0; step < 500; ++step) {
            if (!arena_.isRoundOver()) {
              arena_.step(dt);
            } else {
              evolve();
              startNewRound();
              generation_++;

              // Save and log every 10 generations
              if (generation_ % 10 == 0) {
                savePopulation();
                std::cout << "Gen " << generation_
                          << " | Best: " << bestFitness_
                          << " | Avg: " << avgFitness_ << std::endl;
              }
            }
          }
          currentSpeed_ = 9999.0f;
        }
      }

      // Always render (with speed indicator)
      render();

      // Frame delay based on mode
      SDL_Delay(watchMode ? 16 : 1);
    }

    // Save on exit
    savePopulation();
    saveBest();
    std::cout << "Training saved. Resume anytime!" << std::endl;

    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }

private:
  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;

  BattleRoyaleArena arena_;
  std::vector<BattleRoyaleNN> population_;
  std::mt19937 rng_{std::random_device{}()};

  int generation_ = 0;
  float bestFitness_ = 0;
  float avgFitness_ = 0;
  float currentSpeed_ = 1.0f;

  float calculateSpeedFactor(int aliveCount) {
    // Linear interpolation: 50 tanks = 100x, 1 tank = 1x
    // speed = 1 + (100 - 1) * (alive - 1) / (50 - 1)
    if (aliveCount <= 1)
      return MIN_SIM_SPEED;
    if (aliveCount >= POPULATION_SIZE)
      return MAX_SIM_SPEED;

    float t = static_cast<float>(aliveCount - 1) / (POPULATION_SIZE - 1);
    return MIN_SIM_SPEED + (MAX_SIM_SPEED - MIN_SIM_SPEED) * t;
  }

  bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
      return false;
    }

    window_ = SDL_CreateWindow("Battle Royale Training", SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED, WINDOW_SIZE, WINDOW_SIZE,
                               SDL_WINDOW_SHOWN);
    if (!window_)
      return false;

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_)
      return false;

    startNewRound();
    return true;
  }

  void startNewRound() {
    arena_.reset();

    // Assign brains to agents
    auto &agents = arena_.getAgentsMutable();
    for (size_t i = 0; i < agents.size(); ++i) {
      agents[i].brain = &population_[i];
      agents[i].color = getColorFromNN(population_[i]);
    }
  }

  uint32_t getColorFromNN(const BattleRoyaleNN &nn) {
    float w1 = std::abs(nn.weights1[0]) * 255;
    float w2 = std::abs(nn.weights2[0]) * 255;
    float w3 = std::abs(nn.weights3[0]) * 255;

    uint8_t r = static_cast<uint8_t>(std::clamp(w1, 50.0f, 255.0f));
    uint8_t g = static_cast<uint8_t>(std::clamp(w2, 50.0f, 255.0f));
    uint8_t b = static_cast<uint8_t>(std::clamp(w3, 50.0f, 255.0f));

    return (r << 16) | (g << 8) | b;
  }

  void evolve() {
    auto &agents = arena_.getAgentsMutable();
    for (size_t i = 0; i < agents.size(); ++i) {
      population_[i].fitness = agents[i].getFitness();
    }

    std::sort(population_.begin(), population_.end(),
              [](const BattleRoyaleNN &a, const BattleRoyaleNN &b) {
                return a.fitness > b.fitness;
              });

    bestFitness_ = population_[0].fitness;
    avgFitness_ = 0;
    for (const auto &nn : population_)
      avgFitness_ += nn.fitness;
    avgFitness_ /= population_.size();

    std::vector<BattleRoyaleNN> newPop;
    newPop.reserve(POPULATION_SIZE);

    // Elites survive unchanged
    for (int i = 0; i < ELITE_COUNT; ++i) {
      newPop.push_back(population_[i]);
    }

    // Random injection for diversity
    for (int i = 0; i < RANDOM_INJECT; ++i) {
      BattleRoyaleNN fresh;
      fresh.randomize(rng_);
      newPop.push_back(fresh);
    }

    // TOURNAMENT SELECTION + Crossover + Mutation
    // Any network can be a parent, but fitter ones are more likely to win
    std::uniform_int_distribution<int> popDist(0, POPULATION_SIZE - 1);

    while (newPop.size() < POPULATION_SIZE) {
      // Tournament selection: pick 2 random, choose the fitter one
      auto tournament = [&]() -> int {
        int a = popDist(rng_);
        int b = popDist(rng_);
        return (population_[a].fitness > population_[b].fitness) ? a : b;
      };

      int p1 = tournament();
      int p2 = tournament();

      // Ensure different parents
      while (p2 == p1)
        p2 = tournament();

      BattleRoyaleNN child =
          BattleRoyaleNN::crossover(population_[p1], population_[p2], rng_);
      child.mutate(MUTATION_RATE, MUTATION_STRENGTH, rng_);
      newPop.push_back(std::move(child));
    }

    population_ = std::move(newPop);
  }

  void saveBest() {
    auto data = population_[0].serialize();
    std::ofstream file(SAVE_FILE, std::ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()),
               data.size() * sizeof(float));
    std::cout << "Saved best to " << SAVE_FILE << std::endl;
  }

  void savePopulation() {
    std::ofstream file(POPULATION_FILE, std::ios::binary);
    if (!file)
      return;

    // Write generation number
    file.write(reinterpret_cast<const char *>(&generation_),
               sizeof(generation_));

    // Write all networks
    for (const auto &nn : population_) {
      auto data = nn.serialize();
      file.write(reinterpret_cast<const char *>(data.data()),
                 data.size() * sizeof(float));
    }
  }

  bool loadPopulation() {
    std::ifstream file(POPULATION_FILE, std::ios::binary);
    if (!file)
      return false;

    // Read generation number
    file.read(reinterpret_cast<char *>(&generation_), sizeof(generation_));

    // Read all networks
    std::vector<float> data(BR_TOTAL_WEIGHTS);
    for (auto &nn : population_) {
      file.read(reinterpret_cast<char *>(data.data()),
                data.size() * sizeof(float));
      if (!file)
        return false;
      nn.deserialize(data);
    }

    return true;
  }

  void render() {
    SDL_SetRenderDrawColor(renderer_, 20, 20, 30, 255);
    SDL_RenderClear(renderer_);

    float scale = WINDOW_SIZE / WORLD_SIZE;
    float offset = WINDOW_SIZE / 2.0f;

    // Draw safe zone
    const auto &zone = arena_.getZone();
    drawCircle(zone.centerX * scale + offset, zone.centerY * scale + offset,
               zone.radius * scale, 50, 200, 50);

    if (zone.radius > zone.targetRadius) {
      drawCircle(zone.centerX * scale + offset, zone.centerY * scale + offset,
                 zone.targetRadius * scale, 200, 100, 50);
    }

    // Draw projectiles
    SDL_SetRenderDrawColor(renderer_, 255, 255, 100, 255);
    for (const auto &p : arena_.getProjectiles()) {
      int px = static_cast<int>(p.x * scale + offset);
      int py = static_cast<int>(p.y * scale + offset);
      SDL_Rect rect = {px - 2, py - 2, 4, 4};
      SDL_RenderFillRect(renderer_, &rect);
    }

    // Draw agents
    for (const auto &agent : arena_.getAgents()) {
      if (!agent.alive)
        continue;

      int ax = static_cast<int>(agent.x * scale + offset);
      int ay = static_cast<int>(agent.y * scale + offset);

      uint8_t r = (agent.color >> 16) & 0xFF;
      uint8_t g = (agent.color >> 8) & 0xFF;
      uint8_t b = agent.color & 0xFF;

      float hpFactor = agent.health / 100.0f;
      r = static_cast<uint8_t>(r * hpFactor);
      g = static_cast<uint8_t>(g * hpFactor);
      b = static_cast<uint8_t>(b * hpFactor);

      SDL_SetRenderDrawColor(renderer_, r, g, b, 255);

      SDL_Rect rect = {ax - 4, ay - 4, 8, 8};
      SDL_RenderFillRect(renderer_, &rect);

      int fx = ax + static_cast<int>(std::cos(agent.angle) * 8);
      int fy = ay + static_cast<int>(std::sin(agent.angle) * 8);
      SDL_RenderDrawLine(renderer_, ax, ay, fx, fy);
    }

    renderHUD();
    SDL_RenderPresent(renderer_);
  }

  void drawCircle(float cx, float cy, float r, int red, int green, int blue) {
    SDL_SetRenderDrawColor(renderer_, red, green, blue, 100);
    for (int i = 0; i < 60; ++i) {
      float a1 = i * 2 * M_PI / 60;
      float a2 = (i + 1) * 2 * M_PI / 60;
      int x1 = static_cast<int>(cx + std::cos(a1) * r);
      int y1 = static_cast<int>(cy + std::sin(a1) * r);
      int x2 = static_cast<int>(cx + std::cos(a2) * r);
      int y2 = static_cast<int>(cy + std::sin(a2) * r);
      SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
    }
  }

  void renderHUD() {
    char title[256];
    snprintf(title, sizeof(title),
             "Gen %d | Alive: %d/50 | Speed: %.0fx | Best: %.0f | Time: %.1fs "
             "| [SPACE]=Pause [S]=Save",
             generation_, arena_.getAliveCount(), currentSpeed_, bestFitness_,
             arena_.getElapsedTime());
    SDL_SetWindowTitle(window_, title);
  }
};

int main() {
  std::cout << "=== Battle Royale AI Training ===" << std::endl;
  std::cout << "Features:" << std::endl;
  std::cout << "  - 100x speed at start, 1x when 1 tank left" << std::endl;
  std::cout << "  - Cumulative learning (auto-saves, resumes)" << std::endl;
  std::cout << std::endl;
  std::cout << "Controls:" << std::endl;
  std::cout << "  SPACE: Pause/Resume" << std::endl;
  std::cout << "  S: Save progress" << std::endl;
  std::cout << "  ESC: Exit (auto-saves)" << std::endl;
  std::cout << std::endl;

  BattleRoyaleTrainer trainer;
  trainer.run();

  return 0;
}
