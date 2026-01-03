/**
 * Main.cpp - Tanques3D game entry point
 *
 * Vulkan/SDL2 renderer with Metal backend via MoltenVK on macOS.
 * Camera logic inspired by original OpenGL Camera.cpp
 */

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "ai/AIController.h"
#include "core/Constants.h"
#include "core/Timer.h"
#include "core/Vector.h"
#include "entities/Projectile.h"
#include "game/Control.h"
#include "game/GameData.h"
#include "physics/PhysicsWorld.h"
#include "rendering/VulkanRenderer.h"
#include "rendering/VulkanWindow.h"

static std::unique_ptr<GameData> gameDataPtr;
GameData *gameData = nullptr;
static std::unique_ptr<VulkanRenderer> renderer;
static std::unique_ptr<Physics::PhysicsWorld> physicsWorld;
static std::unique_ptr<AI::AIController> aiController;
static Control control;
static long lastIterationTime = 0;

// Camera state (follows player, positioned behind and above)
static Vector camPos(0, 0, 10);
static Vector camTarget(0, 0, 0);
static Vector camUp(0, 0, 1);

// Projection parameters (SI: meters)
constexpr float FOV = 60.0f * 3.14159f / 180.0f;
constexpr float NEAR_PLANE = 1.0f; // 1 meter near plane
constexpr float FAR_PLANE =
    500.0f; // 500 meters far plane (enemies spawn 50-150m)
constexpr float ASPECT = 800.0f / 600.0f;

// Build perspective projection matrix
void buildProjectionMatrix(float *m) {
  float tanHalfFov = std::tan(FOV / 2.0f);
  float f = 1.0f / tanHalfFov;
  float nf = 1.0f / (NEAR_PLANE - FAR_PLANE);

  m[0] = f / ASPECT;
  m[1] = 0;
  m[2] = 0;
  m[3] = 0;
  m[4] = 0;
  m[5] = -f;
  m[6] = 0;
  m[7] = 0;
  m[8] = 0;
  m[9] = 0;
  m[10] = (FAR_PLANE + NEAR_PLANE) * nf;
  m[11] = -1;
  m[12] = 0;
  m[13] = 0;
  m[14] = (2 * FAR_PLANE * NEAR_PLANE) * nf;
  m[15] = 0;
}

// Build look-at view matrix (like gluLookAt)
void buildViewMatrix(float *m, const Vector &eye, const Vector &target,
                     const Vector &up) {
  Vector f = target - eye;
  f.setVectorLength(1.0);

  Vector s = f.crossProduct(up);
  s.setVectorLength(1.0);

  Vector u = s.crossProduct(f);
  u.setVectorLength(1.0);

  float fx = static_cast<float>(f.getX());
  float fy = static_cast<float>(f.getY());
  float fz = static_cast<float>(f.getZ());
  float sx = static_cast<float>(s.getX());
  float sy = static_cast<float>(s.getY());
  float sz = static_cast<float>(s.getZ());
  float ux = static_cast<float>(u.getX());
  float uy = static_cast<float>(u.getY());
  float uz = static_cast<float>(u.getZ());
  float ex = static_cast<float>(eye.getX());
  float ey = static_cast<float>(eye.getY());
  float ez = static_cast<float>(eye.getZ());

  m[0] = sx;
  m[1] = ux;
  m[2] = -fx;
  m[3] = 0;
  m[4] = sy;
  m[5] = uy;
  m[6] = -fy;
  m[7] = 0;
  m[8] = sz;
  m[9] = uz;
  m[10] = -fz;
  m[11] = 0;
  m[12] = -(sx * ex + sy * ey + sz * ez);
  m[13] = -(ux * ex + uy * ey + uz * ez);
  m[14] = (fx * ex + fy * ey + fz * ez);
  m[15] = 1;
}

// Multiply two 4x4 matrices: result = a * b
void multiplyMatrices(float *result, const float *a, const float *b) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i * 4 + j] =
          a[0 * 4 + j] * b[i * 4 + 0] + a[1 * 4 + j] * b[i * 4 + 1] +
          a[2 * 4 + j] * b[i * 4 + 2] + a[3 * 4 + j] * b[i * 4 + 3];
    }
  }
}

// Update camera to follow player (SI units: 1 unit = 1 meter)
void updateCamera(GameData &game) {
  Agent *player = game.getPlayer();
  if (!player)
    return;

  // Camera positioned behind and above tank for good view
  constexpr double distBehind = 25.0; // 25 meters behind
  constexpr double height = 15.0;     // 15 meters high

  Vector trackedDir = player->getDir();
  trackedDir.setVectorLength(distBehind);

  camPos = player->getPosition() - trackedDir + Vector(0, 0, height);
  camTarget = player->getPosition() + Vector(0, 0, 1.0); // Look at turret level
  camUp = Vector(0, 0, 1);
}

// Draw a 3D box with direct vertex positions
void drawBox(VulkanRenderer &r, const Vector &center, float halfW, float halfL,
             float halfH, float dx, float dy, float sx, float sy, float colR,
             float colG, float colB) {
  float cx = static_cast<float>(center.getX());
  float cy = static_cast<float>(center.getY());
  float cz = static_cast<float>(center.getZ());

  float frontX = cx + dx * halfL;
  float frontY = cy + dy * halfL;
  float backX = cx - dx * halfL;
  float backY = cy - dy * halfL;

  // 8 corners in world space
  Vector ftl(frontX - sx * halfW, frontY - sy * halfW, cz + halfH);
  Vector ftr(frontX + sx * halfW, frontY + sy * halfW, cz + halfH);
  Vector btl(backX - sx * halfW, backY - sy * halfW, cz + halfH);
  Vector btr(backX + sx * halfW, backY + sy * halfW, cz + halfH);
  Vector fbl(frontX - sx * halfW, frontY - sy * halfW, cz - halfH);
  Vector fbr(frontX + sx * halfW, frontY + sy * halfW, cz - halfH);
  Vector bbl(backX - sx * halfW, backY - sy * halfW, cz - halfH);
  Vector bbr(backX + sx * halfW, backY + sy * halfW, cz - halfH);

  // Draw faces
  r.drawQuad(ftl, ftr, btr, btl, colR + 0.1f, colG + 0.1f, colB + 0.1f); // Top
  r.drawQuad(fbl, bbl, bbr, fbr, colR - 0.1f, colG - 0.1f,
             colB - 0.1f);                          // Bottom
  r.drawQuad(ftl, ftr, fbr, fbl, colR, colG, colB); // Front
  r.drawQuad(btl, btr, bbr, bbl, colR - 0.05f, colG - 0.05f,
             colB - 0.05f);                                // Back
  r.drawQuad(ftl, btl, bbl, fbl, colR - 0.1f, colG, colB); // Left
  r.drawQuad(ftr, btr, bbr, fbr, colR, colG - 0.1f, colB); // Right
}

// Draw a 3D tank (SI units: meters)
// M1 Abrams-like: ~8m long, ~4m wide, ~2.5m tall
void drawTank3D(VulkanRenderer &r, const Vector &pos, const Vector &dir,
                const Vector &side, float bodyR, float bodyG, float bodyB) {
  float dx = static_cast<float>(dir.getX());
  float dy = static_cast<float>(dir.getY());
  float sx = static_cast<float>(side.getX());
  float sy = static_cast<float>(side.getY());

  float cx = static_cast<float>(pos.getX());
  float cy = static_cast<float>(pos.getY());

  // Tank body: 8m long x 4m wide x 1.5m tall (halfL=4, halfW=2, halfH=0.75)
  Vector bodyCenter(cx, cy, 0.75f);
  drawBox(r, bodyCenter, 2.0f, 4.0f, 0.75f, dx, dy, sx, sy, bodyR, bodyG,
          bodyB);

  // Turret: 3m diameter x 1m tall, on top of body
  Vector turretCenter(cx, cy, 2.0f);
  drawBox(r, turretCenter, 1.5f, 1.5f, 0.5f, dx, dy, sx, sy, bodyR * 0.8f,
          bodyG * 0.8f, bodyB * 0.8f);

  // Cannon: 5m long x 0.3m diameter, extending forward from turret
  float cannonCx = cx + dx * 4.5f;
  float cannonCy = cy + dy * 4.5f;
  Vector cannonCenter(cannonCx, cannonCy, 2.0f);
  drawBox(r, cannonCenter, 0.15f, 2.5f, 0.15f, dx, dy, sx, sy, 0.25f, 0.25f,
          0.25f);
}

// Draw projectile - tank shell ~0.5m
void drawProjectile(VulkanRenderer &r, const Vector &pos, float colR,
                    float colG, float colB) {
  Vector center(static_cast<float>(pos.getX()), static_cast<float>(pos.getY()),
                2.0f);
  // Shell: 0.5m long projectile, bright orange
  drawBox(r, center, 0.15f, 0.3f, 0.15f, 1, 0, 0, 1, 1.0f, 0.6f, 0.1f);
}

// Draw ground with checkerboard pattern for movement perception
void drawGround(VulkanRenderer &r) {
  constexpr float gridSize = 20.0f; // 20m grid squares
  constexpr int numTiles = 25;      // 25 tiles each direction = 500m total

  for (int i = -numTiles; i < numTiles; i++) {
    for (int j = -numTiles; j < numTiles; j++) {
      float x1 = i * gridSize;
      float y1 = j * gridSize;
      float x2 = x1 + gridSize;
      float y2 = y1 + gridSize;

      // Checkerboard pattern: alternate colors
      bool isLight = ((i + j) % 2) == 0;
      float r_col = isLight ? 0.35f : 0.25f;
      float g_col = isLight ? 0.55f : 0.45f;
      float b_col = isLight ? 0.30f : 0.20f;

      Vector p1(x1, y1, -0.1);
      Vector p2(x2, y1, -0.1);
      Vector p3(x2, y2, -0.1);
      Vector p4(x1, y2, -0.1);
      r.drawQuad(p1, p2, p3, p4, r_col, g_col, b_col);
    }
  }
}

// Draw sky with horizon gradient effect
void drawSky(VulkanRenderer &r) {
  // Sky backdrop - large quad at horizon
  // Upper sky (lighter blue)
  Vector s1(-1000, 600, 50);
  Vector s2(1000, 600, 50);
  Vector s3(1000, 600, 200);
  Vector s4(-1000, 600, 200);
  r.drawQuad(s1, s2, s3, s4, 0.5f, 0.7f, 0.95f);

  // Horizon haze (lighter, fog-like)
  Vector h1(-1000, 600, -10);
  Vector h2(1000, 600, -10);
  Vector h3(1000, 600, 50);
  Vector h4(-1000, 600, 50);
  r.drawQuad(h1, h2, h3, h4, 0.7f, 0.8f, 0.9f);
}

// Render all game entities
void drawGame(VulkanRenderer &r, GameData &game) {
  // Update camera
  updateCamera(game);

  // Build MVP matrix
  float proj[16], view[16], vp[16];
  buildProjectionMatrix(proj);
  buildViewMatrix(view, camPos, camTarget, camUp);
  multiplyMatrices(vp, proj, view);

  // Set MVP matrix in renderer
  r.setMVPMatrix(vp);

  // Draw sky first (furthest back)
  drawSky(r);

  // Draw ground
  drawGround(r);

  // Draw entities
  const auto &agents = game.getAgentsVector();
  for (const auto &agent : agents) {
    if (!agent)
      continue;

    Projectile *proj = dynamic_cast<Projectile *>(agent.get());
    if (proj) {
      drawProjectile(r, agent->getPosition(), 1.0f, 0.8f, 0.2f);
    } else {
      Vector pos = agent->getPosition();
      Vector dir = agent->getDir();
      Vector side = agent->getSide();

      if (agent.get() == game.getPlayer()) {
        drawTank3D(r, pos, dir, side, 0.2f, 0.7f, 0.2f);
      } else {
        drawTank3D(r, pos, dir, side, 0.7f, 0.2f, 0.2f);
      }
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

  // Initialize Apple Silicon Metal physics
  try {
    physicsWorld = std::make_unique<Physics::PhysicsWorld>();
    physicsWorld->initialize();
    physicsWorld->setGravity(9.81f); // Standard Earth gravity
    std::cout << "Metal physics: " << physicsWorld->getDeviceName()
              << std::endl;
  } catch (const Physics::MetalComputeError &e) {
    std::cerr << "Metal physics error: " << e.what() << std::endl;
    return 1;
  }

  // Initialize Neural Engine AI
  try {
    aiController = std::make_unique<AI::AIController>();
    aiController->initialize();
    std::cout << "AI: " << aiController->getComputeDevice() << std::endl;
  } catch (const AI::CoreMLError &e) {
    std::cerr << "AI error: " << e.what() << std::endl;
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
    long elapsedTime = currentTime - lastIterationTime;

    // Accumulator-based physics: run multiple steps if needed to catch up
    // This ensures physics ALWAYS runs at 50Hz (TIME_STEP=20ms) in real time
    while (elapsedTime >= TIME_STEP) {
      gameData->setControl(control);
      gameData->iterateGameData();
      elapsedTime -= TIME_STEP;
      lastIterationTime += TIME_STEP;
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
