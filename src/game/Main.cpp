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

// Draw a 3D box (6 faces) at the given position
void drawBox(VulkanRenderer &r, float cx, float cy, float cz, float halfW,
             float halfL, float halfH, float dx, float dy, float sx, float sy,
             float colR, float colG, float colB) {
  // Box corners relative to center, rotated by direction
  // Front-back axis is direction, left-right is side

  // Calculate 8 corner positions
  float frontX = cx + dx * halfL;
  float frontY = cy + dy * halfL;
  float backX = cx - dx * halfL;
  float backY = cy - dy * halfL;

  // Top face (z + halfH)
  Vector ftl(frontX - sx * halfW, frontY - sy * halfW, cz + halfH);
  Vector ftr(frontX + sx * halfW, frontY + sy * halfW, cz + halfH);
  Vector btl(backX - sx * halfW, backY - sy * halfW, cz + halfH);
  Vector btr(backX + sx * halfW, backY + sy * halfW, cz + halfH);

  // Bottom face (z - halfH)
  Vector fbl(frontX - sx * halfW, frontY - sy * halfW, cz - halfH);
  Vector fbr(frontX + sx * halfW, frontY + sy * halfW, cz - halfH);
  Vector bbl(backX - sx * halfW, backY - sy * halfW, cz - halfH);
  Vector bbr(backX + sx * halfW, backY + sy * halfW, cz - halfH);

  // Draw 6 faces with slight color variation
  r.drawQuad(ftl, ftr, btr, btl, colR + 0.1f, colG + 0.1f, colB + 0.1f); // Top
  r.drawQuad(fbl, fbr, bbr, bbl, colR - 0.1f, colG - 0.1f,
             colB - 0.1f);                          // Bottom
  r.drawQuad(ftl, ftr, fbr, fbl, colR, colG, colB); // Front
  r.drawQuad(btl, btr, bbr, bbl, colR - 0.05f, colG - 0.05f,
             colB - 0.05f);                                // Back
  r.drawQuad(ftl, btl, bbl, fbl, colR - 0.1f, colG, colB); // Left
  r.drawQuad(ftr, btr, bbr, fbr, colR, colG - 0.1f, colB); // Right
}

// Draw a 3D tank with body, turret, and cannon
void drawTank3D(VulkanRenderer &r, const Vector &pos, const Vector &dir,
                const Vector &side, float bodyR, float bodyG, float bodyB) {
  // Scale factor: world units to Vulkan clip space
  float scale = 0.05f;

  // Tank center position (negate Y for Vulkan coord system)
  float cx = static_cast<float>(pos.getX()) * scale;
  float cy = -static_cast<float>(pos.getY()) * scale;
  float cz = 0.0f; // Ground level

  float dx = static_cast<float>(dir.getX());
  float dy = -static_cast<float>(dir.getY());
  float sx = static_cast<float>(side.getX());
  float sy = -static_cast<float>(side.getY());

  // Tank body: 0.12 x 0.06 x 0.03
  drawBox(r, cx, cy, cz + 0.015f, 0.03f, 0.04f, 0.015f, dx, dy, sx, sy, bodyR,
          bodyG, bodyB);

  // Turret: 0.05 x 0.05 x 0.02 (slightly above body)
  drawBox(r, cx, cy, cz + 0.04f, 0.02f, 0.02f, 0.01f, dx, dy, sx, sy,
          bodyR * 0.8f, bodyG * 0.8f, bodyB * 0.8f);

  // Cannon: 0.06 long, 0.01 wide, 0.01 tall (extends forward from turret)
  float cannonCx = cx + dx * 0.05f;
  float cannonCy = cy + dy * 0.05f;
  drawBox(r, cannonCx, cannonCy, cz + 0.04f, 0.005f, 0.03f, 0.005f, dx, dy, sx,
          sy, 0.3f, 0.3f, 0.3f);
}

// Draw the ground plane
void drawGround(VulkanRenderer &r) {
  // Large green plane at z = 0
  Vector p1(-1.5f, -1.5f, -0.01f);
  Vector p2(1.5f, -1.5f, -0.01f);
  Vector p3(1.5f, 1.5f, -0.01f);
  Vector p4(-1.5f, 1.5f, -0.01f);
  r.drawQuad(p1, p2, p3, p4, 0.2f, 0.35f, 0.15f);
}

// Render all game entities in 3D
void drawGame(VulkanRenderer &r, GameData &game) {
  // Draw ground first
  drawGround(r);

  // Draw all tanks
  const auto &agents = game.getAgentsVector();
  for (const auto &agent : agents) {
    if (!agent)
      continue;

    Vector pos = agent->getPosition();
    Vector dir = agent->getDir();
    Vector side = agent->getSide();

    // Player = green, enemies = red
    if (agent.get() == game.getPlayer()) {
      drawTank3D(r, pos, dir, side, 0.2f, 0.7f, 0.2f);
    } else {
      drawTank3D(r, pos, dir, side, 0.7f, 0.2f, 0.2f);
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
