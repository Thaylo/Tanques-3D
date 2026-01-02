/**
 * GLDraw.cpp - OpenGL drawing utilities implementation
 */

#include "rendering/GLDraw.h"

#define Width 800
#define Height 800

GLuint groundTextures[2];
GLuint tankTextures[5];

GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light_position[4] = {0.0, 0.0, -10000, 1.0};

unsigned int getint(FILE *fp) {
  int c = getc(fp);
  int c1 = getc(fp);
  int c2 = getc(fp);
  int c3 = getc(fp);

  return ((unsigned int)c) + (((unsigned int)c1) << 8) +
         (((unsigned int)c2) << 16) + (((unsigned int)c3) << 24);
}

unsigned int getshort(FILE *fp) {
  int c = getc(fp);
  int c1 = getc(fp);
  return ((unsigned int)c) + (((unsigned int)c1) << 8);
}

int ImageLoad(const char *filename, Image *image) {
  FILE *file;
  unsigned long size;
  unsigned long i;
  unsigned short int planes;
  unsigned short int bpp;
  char temp;

  if ((file = fopen(filename, "rb")) == nullptr) {
    printf("File not found: %s\n", filename);
    return 0;
  }

  // Skip to width/height in BMP header
  fseek(file, 18, SEEK_CUR);

  // Read dimensions
  image->sizeX = getint(file);
  printf("Width of %s: %lu\n", filename, image->sizeX);
  image->sizeY = getint(file);
  printf("Height of %s: %lu\n", filename, image->sizeY);

  // Calculate size (assuming 24 bits / 3 bytes per pixel)
  size = image->sizeX * image->sizeY * 3;

  // Read number of planes
  planes = getshort(file);
  if (planes != 1) {
    printf("Planes from %s is not 1: %u\n", filename, planes);
    fclose(file);
    return 0;
  }

  // Read bits per pixel
  bpp = getshort(file);
  if (bpp != 24) {
    printf("Bpp of %s is not 24: %u\n", filename, bpp);
    fclose(file);
    return 0;
  }

  // Skip rest of header
  fseek(file, 24, SEEK_CUR);

  // Read pixel data
  image->data = (char *)malloc(size);
  if (image->data == nullptr) {
    printf("Error allocating memory for color data\n");
    fclose(file);
    return 0;
  }

  if ((i = fread(image->data, size, 1, file)) != 1) {
    printf("Error reading color data from %s.\n", filename);
    fclose(file);
    return 0;
  }

  fclose(file);

  // Convert BGR to RGB
  for (i = 0; i < size; i += 3) {
    temp = image->data[i];
    image->data[i] = image->data[i + 2];
    image->data[i + 2] = temp;
  }

  return 1;
}

GLuint *LoadGLTextures(const char *source, GLuint *texture, int pos) {
  Image *image1 = (Image *)malloc(sizeof(Image));

  if (image1 == nullptr) {
    printf("Error allocating memory for image\n");
    exit(0);
  }

  if (!ImageLoad(source, image1)) {
    exit(1);
  }

  // Create texture
  glGenTextures(1, &texture[pos]);
  glBindTexture(GL_TEXTURE_2D, texture[pos]);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image1->data);

  free(image1->data);
  free(image1);

  return texture;
}

void initGl() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  glScalef(1.0 / SCALE, 1.0 / SCALE, 1.0 / SCALE);
  gluPerspective(30.0f, 800.0 / 800.0, 0.1f, 10000.0f);

  // Generate texture names
  glGenTextures(2, groundTextures);

  // Try multiple asset paths for compatibility
  // Path 1: assets/ (run from project root or build/bin)
  // Path 2: bin/assets/ (run from build/)
  const char *assetPrefixes[] = {"assets/", "bin/assets/", "../assets/", ""};
  const char *prefix = assetPrefixes[0];

  // Find which prefix works
  for (int i = 0; i < 4; i++) {
    char testPath[256];
    snprintf(testPath, sizeof(testPath), "%stexture.bmp", assetPrefixes[i]);
    FILE *test = fopen(testPath, "rb");
    if (test) {
      fclose(test);
      prefix = assetPrefixes[i];
      break;
    }
  }

  char path[256];

  // Load ground textures
  snprintf(path, sizeof(path), "%stexture.bmp", prefix);
  LoadGLTextures(path, groundTextures, 0);
  snprintf(path, sizeof(path), "%ssky.bmp", prefix);
  LoadGLTextures(path, groundTextures, 1);

  // Load tank textures
  snprintf(path, sizeof(path), "%sfrente.bmp", prefix);
  LoadGLTextures(path, tankTextures, 0);
  snprintf(path, sizeof(path), "%sverso.bmp", prefix);
  LoadGLTextures(path, tankTextures, 1);
  snprintf(path, sizeof(path), "%slateralDir.bmp", prefix);
  LoadGLTextures(path, tankTextures, 2);
  snprintf(path, sizeof(path), "%slateralEsq.bmp", prefix);
  LoadGLTextures(path, tankTextures, 3);
  snprintf(path, sizeof(path), "%stopo.bmp", prefix);
  LoadGLTextures(path, tankTextures, 4);

  glEnable(GL_TEXTURE_2D);
}

void setColor(float red, float green, float blue) {
  glColor3f(red, green, blue);
}

void drawLine(float x1, float y1, float x2, float y2) {
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

void drawRectangle(float x1, float y1, float x2, float y2, float z) {
  glBegin(GL_QUADS);
  glVertex3f(x1, y1, z);
  glVertex3f(x2, y1, z);
  glVertex3f(x2, y2, z);
  glVertex3f(x1, y2, z);
  glEnd();
}

void drawPlane(float x1, float y1, float x2, float y2, float z) {
  glBegin(GL_QUADS);
  glVertex3f(x1, y1, 0);
  glVertex3f(x2, y1, -1);
  glVertex3f(x2, y2, 1);
  glVertex3f(x1, y2, 0);
  glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3,
                  float z) {
  glBegin(GL_TRIANGLES);
  glVertex3f(x1, y1, z);
  glVertex3f(x2, y2, z);
  glVertex3f(x3, y3, z);
  glEnd();
}

void drawText(float x, float y, const char *msg, ...) {
  char buf[1024];
  va_list args;
  va_start(args, msg);
  vsprintf(buf, msg, args);
  va_end(args);

  int length = strlen(buf);
  glRasterPos2f(x, y);
  for (int i = 0; i < length; i++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, buf[i]);
  }
}
