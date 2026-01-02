/**
 * Window.h - GLUT window management
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "rendering/GLDraw.h"

typedef void (*MouseCallback)(int type, int button, int x, int y);
typedef void (*KeyCallback)(int code);

/**
 * GLUT window wrapper for creating and managing the game window.
 */
class window {
private:
  int width;
  int height;
  char title[100];

public:
  window();
  window(int w, int h);
  ~window();

  void showWindow();
  bool processWindow(MouseCallback mouseFunc, KeyCallback keyPress,
                     KeyCallback keyRelease);
};

#endif // WINDOW_H
