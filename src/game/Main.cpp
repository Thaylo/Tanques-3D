/**
 * Main.cpp - Tanques3D game entry point
 *
 * Vulkan/SDL2 renderer with Metal backend via MoltenVK on macOS.
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "core/Constants.h"
#include "core/Timer.h"
#include "core/Vector.h"
#include "game/Control.h"
#include "game/GameData.h"
#include "rendering/VulkanRenderer.h"
#include "rendering/VulkanWindow.h"

static std::unique_ptr<GameData> gameDataPtr;
GameData *gameData = nullptr;
static std::unique_ptr<VulkanRenderer> renderer;
static Control control;
static long lastIterationTime = 0;

// Draw a tank as arrow-shaped triangles
void drawTank(VulkanRenderer &r, const Vector &pos, const Vector &dir,
              const Vector &side, float bodyR, float bodyG, float bodyB) {
  constexpr float length = 0.08f;
  constexpr float width = 0.04f;

  // Convert world coordinates to screen (orthographic, world -20 to 20 ->
  // screen -1 to 1)
  // Vulkan Y-axis is inverted (down is positive), so negate Y
  float scale = 0.05f;
  float x = static_cast<float>(pos.getX()) * scale;
  float y = -static_cast<float>(pos.getY()) * scale; // Negate Y for Vulkan

  float dx = static_cast<float>(dir.getX());
  float dy = -static_cast<float>(dir.getY()); // Negate Y for Vulkan
  float sx = static_cast<float>(side.getX());
  float sy = -static_cast<float>(side.getY()); // Negate Y for Vulkan

  // Arrow shape vertices
  Vector front(x + dx * length, y + dy * length, 0);
  Vector backLeft(x - dx * length * 0.5 - sx * width,
                  y - dy * length * 0.5 - sy * width, 0);
  Vector backRight(x - dx * length * 0.5 + sx * width,
                   y - dy * length * 0.5 + sy * width, 0);

  r.drawTriangle(front, backLeft, backRight, bodyR, bodyG, bodyB);
}

// Render all game entities
void drawGame(VulkanRenderer &r, GameData &game) {
  const auto &agents = game.getAgentsVector();
  for (const auto &agent : agents) {
    if (!agent)
      continue;

    Vector pos = agent->getPosition();
    Vector dir = agent->getDir();
    Vector side = agent->getSide();

    // Player = green, enemies = red
    if (agent.get() == game.getPlayer()) {
      drawTank(r, pos, dir, side, 0.2f, 0.8f, 0.2f);
    } else {
      drawTank(r, pos, dir, side, 0.8f, 0.2f, 0.2f);
    }
  }
}

int main(int argc, char *argv[]) {
  int numEnemies = 3;
  if (argc > 1) {
    numEnemies = std::atoi(argv[1]);
  }

  std::cout << "Tanques3D v2.0 (Vulkan/Metal renderer)" << std::endl;
  std::cout << "Starting with " << numEnemies << " enemies" << std::endl;

  renderer = std::make_unique<VulkanRenderer>();
  if (!renderer->initialize("Tanques3D", 800, 600)) {
    std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
    return 1;
  }

  gameDataPtr = std::make_unique<GameData>();
  gameData = gameDataPtr.get();
  gameData->initializeGame(numEnemies);
  control = initializeControl();

  lastIterationTime = getCurrentTime();

  std::cout << "Starting game loop..." << std::endl;

  while (renderer->isRunning()) {
    bool running = true;
    VulkanWindow::processEvents(control, running);
    if (!running || control.keyEsc) {
      renderer->setRunning(false);
      break;
    }

    long currentTime = getCurrentTime();
    if (currentTime - lastIterationTime >= TIME_STEP) {
      gameData->setControl(control);
      gameData->iterateGameData();
      lastIterationTime = currentTime;
      control.newLeftPressed = false;
      control.newRightPressed = false;
    }

    renderer->beginFrame();
    drawGame(*renderer, *gameData);
    renderer->endFrame();
  }

  std::cout << "Game ended" << std::endl;
  return 0;
}
