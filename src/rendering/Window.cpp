/**
 * Window.cpp - GLUT window management implementation
 */

#include "rendering/Window.h"
#include <cstring>

// Static callbacks for GLUT
static MouseCallback g_mouseFunc = nullptr;
static KeyCallback g_keyPress = nullptr;
static KeyCallback g_keyRelease = nullptr;
static bool g_initialized = false;

static void mouseCallback(int button, int state, int x, int y) {
  if (g_mouseFunc) {
    g_mouseFunc(state, button, x, y);
  }
}

static void keyboardCallback(unsigned char key, int x, int y) {
  if (g_keyPress) {
    g_keyPress(key);
  }
}

static void keyboardUpCallback(unsigned char key, int x, int y) {
  if (g_keyRelease) {
    g_keyRelease(key);
  }
}

static void specialCallback(int key, int x, int y) {
  if (g_keyPress) {
    g_keyPress(key);
  }
}

static void specialUpCallback(int key, int x, int y) {
  if (g_keyRelease) {
    g_keyRelease(key);
  }
}

// Empty display callback for manual rendering
static void displayCallback() {
  // Rendering is done via drawGame(), not GLUT's display callback
}

// Idle callback to process events
static void idleCallback() { glutPostRedisplay(); }

window::window() : width(800), height(800) {
  strcpy(title, "Tanques 3D");

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(title);
}

window::window(int w, int h) : width(w), height(h) {
  strcpy(title, "Tanques 3D");

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(title);
}

void window::showWindow() { glutSwapBuffers(); }

bool window::processWindow(MouseCallback mouseFunc, KeyCallback keyPress,
                           KeyCallback keyRelease) {
  // Store callbacks for GLUT
  g_mouseFunc = mouseFunc;
  g_keyPress = keyPress;
  g_keyRelease = keyRelease;

  // Register GLUT callbacks only once
  if (!g_initialized) {
    glutDisplayFunc(displayCallback);
    glutIdleFunc(idleCallback);
    glutMouseFunc(mouseCallback);
    glutKeyboardFunc(keyboardCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutSpecialFunc(specialCallback);
    glutSpecialUpFunc(specialUpCallback);
    g_initialized = true;
  }

  // Process pending events on macOS without blocking
  // On macOS, we need to use glutCheckLoop (if available) or
  // rely on glutIdleFunc for continuous updates
#ifdef __APPLE__
  // On macOS, process one iteration of events
  // The display and idle callbacks keep things moving
  glutPostRedisplay();
#else
  glutMainLoopEvent();
#endif

  return true;
}

window::~window() {}
