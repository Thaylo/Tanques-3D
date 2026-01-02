/**
 * Ground.cpp - Ground/terrain rendering implementation
 */

#include "rendering/Ground.h"

Ground::Ground() : Movable(), tracked(nullptr) {}

Ground::Ground(Agent *a) : Movable(), tracked(a) {}

void Ground::iterate() { position = tracked->getPosition(); }

void Ground::draw() {
  double edge = 60;

  // Draw ground texture
  glBindTexture(GL_TEXTURE_2D, groundTextures[0]);
  glBegin(GL_QUADS);
  glColor4f(1, 1, 1, 0);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-edge, -edge, 0);
  glNormal3f(0, 0, 1);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(edge, -edge, 0);
  glNormal3f(0, 0, 1);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(edge, edge, 0);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-edge, edge, 0);
  glEnd();

  // Draw sky box centered on tracked position
  glTranslatef(tracked->getPosition().getX(), tracked->getPosition().getY(),
               tracked->getPosition().getZ());

  edge = 100;

  glBindTexture(GL_TEXTURE_2D, groundTextures[1]);
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-edge, -edge, 5);
  glNormal3f(0, 0, 1);
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(edge, -edge, 5);
  glNormal3f(0, 0, 1);
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(edge, edge, 5);
  glNormal3f(0, 0, 1);
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-edge, edge, 5);
  glEnd();

  glTranslatef(-tracked->getPosition().getX(), -tracked->getPosition().getY(),
               -tracked->getPosition().getZ());
}

Ground::~Ground() {}
