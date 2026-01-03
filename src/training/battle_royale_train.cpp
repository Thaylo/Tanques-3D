/**
 * battle_royale_train.cpp - Visual Battle Royale training
 *
 * Watch 50 tanks fight from above while evolving AI.
 */

#include "training/BattleRoyale.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <chrono>
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

// Visualization
constexpr int WINDOW_SIZE = 800;
constexpr float WORLD_SIZE = 400.0f;

class BattleRoyaleTrainer {
public:
  BattleRoyaleTrainer() {
    population_.resize(POPULATION_SIZE);
    for (auto &nn : population_) {
      nn.randomize(rng_);
    }
  }

  void run() {
    if (!initSDL())
      return;

    bool running = true;
    bool paused = false;
    float simSpeed = 1.0f;

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
          case SDLK_EQUALS:
            simSpeed = std::min(10.0f, simSpeed * 2.0f);
            break;
          case SDLK_MINUS:
            simSpeed = std::max(0.25f, simSpeed / 2.0f);
            break;
          case SDLK_s:
            saveBest();
            break;
          }
        }
      }

      if (!paused) {
        // Run simulation steps
        float dt = 0.02f * simSpeed;

        if (!arena_.isRoundOver()) {
          arena_.step(dt);
        } else {
          // Evolve and start new round
          evolve();
          startNewRound();
          generation_++;

          std::cout << "Gen " << generation_ << " | Best: " << bestFitness_
                    << " | Avg: " << avgFitness_ << std::endl;
        }
      }

      // Render
      render();

      // Cap frame rate
      SDL_Delay(16);
    }

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
    // Color based on some weight characteristics
    float w1 = std::abs(nn.weights1[0]) * 255;
    float w2 = std::abs(nn.weights2[0]) * 255;
    float w3 = std::abs(nn.weights3[0]) * 255;

    uint8_t r = static_cast<uint8_t>(std::clamp(w1, 50.0f, 255.0f));
    uint8_t g = static_cast<uint8_t>(std::clamp(w2, 50.0f, 255.0f));
    uint8_t b = static_cast<uint8_t>(std::clamp(w3, 50.0f, 255.0f));

    return (r << 16) | (g << 8) | b;
  }

  void evolve() {
    // Copy fitness from agents to networks
    auto &agents = arena_.getAgentsMutable();
    for (size_t i = 0; i < agents.size(); ++i) {
      population_[i].fitness = agents[i].getFitness();
    }

    // Sort by fitness
    std::sort(population_.begin(), population_.end(),
              [](const BattleRoyaleNN &a, const BattleRoyaleNN &b) {
                return a.fitness > b.fitness;
              });

    bestFitness_ = population_[0].fitness;
    avgFitness_ = 0;
    for (const auto &nn : population_)
      avgFitness_ += nn.fitness;
    avgFitness_ /= population_.size();

    // Create new population
    std::vector<BattleRoyaleNN> newPop;
    newPop.reserve(POPULATION_SIZE);

    // Elites
    for (int i = 0; i < ELITE_COUNT; ++i) {
      newPop.push_back(population_[i]);
    }

    // Random injection
    for (int i = 0; i < RANDOM_INJECT; ++i) {
      BattleRoyaleNN fresh;
      fresh.randomize(rng_);
      newPop.push_back(fresh);
    }

    // Crossover + mutation
    std::uniform_int_distribution<int> parentDist(0, POPULATION_SIZE / 3);
    while (newPop.size() < POPULATION_SIZE) {
      int p1 = parentDist(rng_);
      int p2 = parentDist(rng_);

      BattleRoyaleNN child =
          BattleRoyaleNN::crossover(population_[p1], population_[p2], rng_);
      child.mutate(MUTATION_RATE, MUTATION_STRENGTH, rng_);
      newPop.push_back(std::move(child));
    }

    population_ = std::move(newPop);
  }

  void saveBest() {
    auto data = population_[0].serialize();
    std::ofstream file("battle_royale_best.bin", std::ios::binary);
    file.write(reinterpret_cast<const char *>(data.data()),
               data.size() * sizeof(float));
    std::cout << "Saved best to battle_royale_best.bin" << std::endl;
  }

  void render() {
    // Clear - dark background
    SDL_SetRenderDrawColor(renderer_, 20, 20, 30, 255);
    SDL_RenderClear(renderer_);

    float scale = WINDOW_SIZE / WORLD_SIZE;
    float offset = WINDOW_SIZE / 2.0f;

    // Draw safe zone
    const auto &zone = arena_.getZone();
    drawCircle(zone.centerX * scale + offset, zone.centerY * scale + offset,
               zone.radius * scale, 50, 200, 50);

    // Draw danger zone (shrinking target)
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

      // Health affects brightness
      float hpFactor = agent.health / 100.0f;
      r = static_cast<uint8_t>(r * hpFactor);
      g = static_cast<uint8_t>(g * hpFactor);
      b = static_cast<uint8_t>(b * hpFactor);

      SDL_SetRenderDrawColor(renderer_, r, g, b, 255);

      // Tank body
      SDL_Rect rect = {ax - 4, ay - 4, 8, 8};
      SDL_RenderFillRect(renderer_, &rect);

      // Direction indicator
      int fx = ax + static_cast<int>(std::cos(agent.angle) * 8);
      int fy = ay + static_cast<int>(std::sin(agent.angle) * 8);
      SDL_RenderDrawLine(renderer_, ax, ay, fx, fy);
    }

    // Draw HUD
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
    // Simple text overlay - generation and alive count
    // (SDL2 text requires SDL_ttf, so we just use the window title)
    char title[128];
    snprintf(title, sizeof(title),
             "Gen %d | Alive: %d | Best: %.0f | Time: %.1fs | [SPACE]=Pause "
             "[+/-]=Speed [S]=Save",
             generation_, arena_.getAliveCount(), bestFitness_,
             arena_.getElapsedTime());
    SDL_SetWindowTitle(window_, title);
  }
};

int main() {
  std::cout << "=== Battle Royale AI Training ===" << std::endl;
  std::cout << "Controls:" << std::endl;
  std::cout << "  SPACE: Pause/Resume" << std::endl;
  std::cout << "  +/-: Speed up/slow down" << std::endl;
  std::cout << "  S: Save best network" << std::endl;
  std::cout << "  ESC: Exit" << std::endl;
  std::cout << std::endl;

  BattleRoyaleTrainer trainer;
  trainer.run();

  return 0;
}
