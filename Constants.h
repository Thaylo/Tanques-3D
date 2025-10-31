#ifndef CONSTANTS_H
#define CONSTANTS_H

// Milliseconds

//#define TIME_STEP 15
#define TIME_STEP 20 // para desenhar porcarias rápido

#define TRUE 1

#define SCALE 5.0
#define GRID_SIZE 1.0

#define TURBINE_INC 21
#define TURBINE_DEC 20

// GLUT key codes for macOS compatibility
#define ARROW_UP 101      // GLUT_KEY_UP
#define ARROW_DOWN 103    // GLUT_KEY_DOWN
#define ARROW_LEFT 100    // GLUT_KEY_LEFT
#define ARROW_RIGHT 102   // GLUT_KEY_RIGHT
#define KEY_ESC 27        // ASCII ESC
#define KEY_SPACE 32      // ASCII Space

#define ZOOM_IN 86
#define ZOOM_OUT 82


#define MOVABLE_MAX_ACCELERATION 10.0
//#define MOVABLE_MAX_VELOCITY 1.0*(TIME_STEP/2000.0)*0.3 // 1.0 because of the grid and 0.3 for colision test
#define MOVABLE_MAX_VELOCITY 15.0 // 1.0 because of the grid and 0.3 for colision test
#define MOVABLE_LINEAR_FRICTION 1.2

#define DEFAULT_GRAVITY 0.005

#define ROUNDS_RECARGA 5
#define ROUNDS_RECARGA_HANDICAP_FOR_IA 5

#define PLAYER_ID 642

#endif
