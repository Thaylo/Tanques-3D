/**
 * Agent.cpp - Tank agent entity implementation
 *
 * Created: 15/10/2012
 * Author: thaylo
 *
 * Uses modern C++ features: range-based for, auto, algorithms
 */

#include "entities/Agent.h"
#include "entities/Projectile.h"
#include "game/GameData.h"

#include <algorithm>
#include <limits>

extern GameData *gameData;

void Agent::glVectorT(const Vector &v) {
  glVertex3f(v.getX(), v.getY(), v.getZ());
}

void Agent::drawRadar() {
  constexpr double size = 1.0 / 16;
  if (nearestEnemy == nullptr)
    return;

  Vector dirToNearest = nearestEnemy->getPosition() - position;
  double sine = dir.crossProduct(dirToNearest).getLengthVector();
  double cosine = dir.dotProduct(dirToNearest);

  double phi = (cosine == 0) ? M_PI / 2 : atan2(sine, cosine);

  glTranslated(position.getX(), position.getY(), position.getZ() + 0.2);

  Vector localDir = dir.rotateVector(up, phi).setVectorLength(1.0);
  Vector localSide = up.crossProduct(dir);
  Vector localUp = up;

  // Draw radar indicator triangles
  glBegin(GL_TRIANGLES);
  glVectorT(localDir * 2 * size);
  glVectorT(localSide * (-size / 2) - localUp * (size / 5));
  glVectorT(localSide * (size / 2) - localUp * (size / 5));
  glEnd();

  glBegin(GL_TRIANGLES);
  glVectorT(localDir * 2 * size);
  glVectorT(localSide * (-size / 2) + localUp * (size / 5));
  glVectorT(localSide * (size / 2) + localUp * (size / 5));
  glEnd();

  glBegin(GL_QUADS);
  glVectorT(localSide * (-size / 2) + localUp * (size / 5));
  glVectorT(localSide * (size / 2) + localUp * (size / 5));
  glVectorT(localSide * (size / 2) - localUp * (size / 5));
  glVectorT(localSide * (-size / 2) - localUp * (size / 5));
  glEnd();

  glTranslated(-position.getX(), -position.getY(), -position.getZ() - 0.2);
}

Agent::Agent()
    : Controlable(), nearestEnemy(nullptr), id(0), markedForDestruction(false),
      reloadTimer(0), isFiring(false) {
  position = Vector(0, 0, 0);
  // Prevents camera initialization issue (workaround)
  velocity = Vector(-2, 0, 0);
  dir = Vector(1, 0, 0);
  side = Vector(0, 1, 0);
  up = Vector(0, 0, 1);
}

Agent::Agent(Vector pos)
    : Controlable(), nearestEnemy(nullptr), id(0), markedForDestruction(false),
      reloadTimer(0), isFiring(false) {
  position = pos;
  // Prevents camera initialization issue (workaround)
  velocity = Vector(-2, 0, 0);
  dir = Vector(1, 0, 0);
  side = Vector(0, 1, 0);
  up = Vector(0, 0, 1);
}

void Agent::setId(int idx) { id = idx; }
int Agent::getId() { return id; }

void glNormalT(const Vector &v) { glNormal3f(v.getX(), v.getY(), v.getZ()); }

void Agent::draw() {
  constexpr double size = 1 / 8.0;
  // Golden ratio for tank proportions
  const double goldenRatio = (1 + sqrt(5)) / 2.0;
  Vector dirl = dir * size * goldenRatio;
  Vector sidel = side * size;
  Vector upl = up * size;

  // Calculate tank corners
  Vector P1 = dirl - sidel;
  Vector P2 = P1 + upl;
  Vector P3 = P2 + sidel * 2;
  Vector P4 = P3 - upl;

  Vector P5 = P1 - dirl * 2;
  Vector P6 = P2 - dirl * 2;
  Vector P7 = P3 - dirl * 2;
  Vector P8 = P4 - dirl * 2;

  // Adjustments to fit texture
  P5 = P5 + dirl * 0.23;
  P8 = P8 + dirl * 0.23;

  glColor3f(0.2f, 0.7f, 0.1f);

  // Draw radar only for player
  if (id == PLAYER_ID) {
    drawRadar();
  }

  glTranslatef(position.getX(), position.getY(), position.getZ());

  // Front of tank
  glBindTexture(GL_TEXTURE_2D, tankTextures[0]);
  glBegin(GL_QUADS);
  glNormalT(dir);
  glTexCoord2f(0.0f, 0.0f);
  glVectorT(P1);
  glTexCoord2f(1.0f, 0.0f);
  glVectorT(P4);
  glTexCoord2f(1.0f, 1.0f);
  glVectorT(P3);
  glTexCoord2f(0.0f, 1.0f);
  glVectorT(P2);
  glEnd();

  // Back of tank
  glBindTexture(GL_TEXTURE_2D, tankTextures[1]);
  glBegin(GL_QUADS);
  glNormalT(dir * (-1.0));
  glTexCoord2f(0.0f, 0.0f);
  glVectorT(P5);
  glTexCoord2f(1.0f, 0.0f);
  glVectorT(P8);
  glTexCoord2f(1.0f, 1.0f);
  glVectorT(P7);
  glTexCoord2f(0.0f, 1.0f);
  glVectorT(P6);
  glEnd();

  // Right side of tank
  glBindTexture(GL_TEXTURE_2D, tankTextures[2]);
  glBegin(GL_QUADS);
  glNormalT(side * (-1.0));
  glTexCoord2f(0.15f, 0.0f);
  glVectorT(P5);
  glTexCoord2f(0.94f, 0.0f);
  glVectorT(P1);
  glTexCoord2f(1.0f, 1.0f);
  glVectorT(P2);
  glTexCoord2f(0.0f, 1.0f);
  glVectorT(P6);
  glEnd();

  // Left side of tank
  glBindTexture(GL_TEXTURE_2D, tankTextures[3]);
  glBegin(GL_QUADS);
  glNormalT(side);
  glTexCoord2f(0.06f, 0.0f);
  glVectorT(P4);
  glTexCoord2f(0.85f, 0.0f);
  glVectorT(P8);
  glTexCoord2f(1.0f, 1.0f);
  glVectorT(P7);
  glTexCoord2f(0.0f, 1.0f);
  glVectorT(P3);
  glEnd();

  // Top of tank
  glBindTexture(GL_TEXTURE_2D, tankTextures[4]);
  glBegin(GL_QUADS);
  glNormalT(up);
  glTexCoord2f(0.0f, 0.0f);
  glVectorT(P6);
  glTexCoord2f(1.0f, 0.0f);
  glVectorT(P2);
  glTexCoord2f(1.0f, 1.0f);
  glVectorT(P3);
  glTexCoord2f(0.0f, 1.0f);
  glVectorT(P7);
  glEnd();

  glTranslatef(-position.getX(), -position.getY(), -position.getZ());
}

void Agent::iterate() {
  // Find nearest enemy using STL algorithm with min_element
  const auto &agentsVec = gameData->getAgentsVector();

  double minDist = std::numeric_limits<double>::max();
  nearestEnemy = nullptr;

  // Use range-based for with structured bindings would be nice, but we need
  // distance calc
  auto nearestIt = std::min_element(
      agentsVec.begin(), agentsVec.end(), [this](const auto &a, const auto &b) {
        // Skip self, projectiles, and player when finding enemies
        auto distA = (a->getId() != PLAYER_ID &&
                      dynamic_cast<Projectile *>(a.get()) == nullptr)
                         ? (a->getPosition() - position).getLengthVector()
                         : std::numeric_limits<double>::max();
        auto distB = (b->getId() != PLAYER_ID &&
                      dynamic_cast<Projectile *>(b.get()) == nullptr)
                         ? (b->getPosition() - position).getLengthVector()
                         : std::numeric_limits<double>::max();
        return distA < distB;
      });

  if (nearestIt != agentsVec.end()) {
    Agent *candidate = nearestIt->get();
    if (candidate->getId() != PLAYER_ID &&
        dynamic_cast<Projectile *>(candidate) == nullptr) {
      nearestEnemy = candidate;
    }
  }

  reloadTimer++;
  controlAction();
  Controlable::iterate();
}

void Agent::controlAction() {
  if (!control)
    return;

  double vert = 0, hor = 0;

  if (control->arrowUp)
    vert += 1;
  if (control->arrowDown)
    vert -= 1;
  if (control->arrowRight)
    hor += 1;
  if (control->arrowLeft)
    hor -= 1;

  Vector newDir = dir;

  if (control->space)
    shoot();

  // Tank always moves in direction it's facing
  acceleration = newDir.setVectorLength(MOVABLE_MAX_ACCELERATION) * vert;
  v_yaw = -hor;
}

void Agent::shoot() {
  if (reloadTimer > RELOAD_ROUNDS) {
    reloadTimer = 0;
    isFiring = true;
  }
}

void Agent::markForDestruction() { markedForDestruction = true; }

bool Agent::isMarkedForDestruction() { return markedForDestruction; }

bool Agent::checkFired() {
  if (isFiring) {
    isFiring = false;
    return true;
  }
  return false;
}

Agent::~Agent() {}
