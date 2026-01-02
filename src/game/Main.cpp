/**
 * Main.cpp - Tanques3D game entry point
 *
 * Vulkan/SDL2 renderer with Metal backend via MoltenVK on macOS.
 */

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
GameData *gameData = nullptr; // Global raw pointer for Agent/Projectile access
static std::unique_ptr<VulkanRenderer> renderer;
static Control control;
static long lastIterationTime = 0;

int main(int argc, char *argv[]) {
  int numEnemies = 3;
  if (argc > 1) {
    numEnemies = std::atoi(argv[1]);
  }

  std::cout << "Tanques3D v2.0 (Vulkan/Metal renderer)" << std::endl;
  std::cout << "Starting with " << numEnemies << " enemies" << std::endl;

  // Initialize Vulkan renderer
  renderer = std::make_unique<VulkanRenderer>();
  if (!renderer->initialize("Tanques3D", 800, 600)) {
    std::cerr << "Failed to initialize Vulkan renderer!" << std::endl;
    return 1;
  }

  // Initialize game data
  gameDataPtr = std::make_unique<GameData>();
  gameData = gameDataPtr.get(); // Set global raw pointer
  gameData->initializeGame(numEnemies);
  control = initializeControl();

  lastIterationTime = getCurrentTime();

  std::cout << "Starting game loop..." << std::endl;

  // Main game loop
  while (renderer->isRunning()) {
    // Process input
    bool running = true;
    VulkanWindow::processEvents(control, running);
    if (!running || control.keyEsc) {
      renderer->setRunning(false);
      break;
    }

    // Update game logic at fixed timestep
    long currentTime = getCurrentTime();
    if (currentTime - lastIterationTime >= TIME_STEP) {
      gameData->setControl(control);
      gameData->iterateGameData();
      lastIterationTime = currentTime;

      // Reset one-shot inputs
      control.newLeftPressed = false;
      control.newRightPressed = false;
    }

    // Render frame
    renderer->beginFrame();
    // TODO: Draw game entities
    renderer->endFrame();
  }

  std::cout << "Game ended" << std::endl;
  return 0;
}
