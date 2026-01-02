/**
 * GLDraw.h - OpenGL drawing utilities
 */

#ifndef GLDRAW_H
#define GLDRAW_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "core/Constants.h"
#include "core/Vector.h"

// BMP Image structure for texture loading
struct Image {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
};

// Ground textures
extern GLuint groundTextures[2];
// Tank textures
extern GLuint tankTextures[5];

// OpenGL material/lighting
extern GLfloat mat_specular[];
extern GLfloat mat_shininess[];
extern GLfloat light_position[];

/**
 * Initialize OpenGL settings and load textures.
 */
void initGl();

/**
 * Load a BMP image from file.
 */
int ImageLoad(const char *filename, Image *image);

/**
 * Load a BMP image as an OpenGL texture.
 */
GLuint *LoadGLTextures(const char *source, GLuint *texture, int pos);

// Drawing primitives
void setColor(float red, float green, float blue);
void drawLine(float x1, float y1, float x2, float y2);
void drawRectangle(float x1, float y1, float x2, float y2, float z);
void drawPlane(float x1, float y1, float x2, float y2, float z);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
                  float z);
void drawText(float x, float y, const char *msg, ...);

// Helper to convert Vector to glVertex3f
inline void glVectorT(const Vector &v) {
  glVertex3f(v.getX(), v.getY(), v.getZ());
}

#endif // GLDRAW_H
