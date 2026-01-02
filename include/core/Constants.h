/**
 * Constants.h - Game constants and key mappings
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Time step in milliseconds
#define TIME_STEP 20

#define TRUE 1

// World scaling
#define SCALE 5.0
#define GRID_SIZE 1.0

// Key mappings for turbine control
#define TURBINE_INC 21
#define TURBINE_DEC 20

// GLUT key codes for macOS compatibility
#define ARROW_UP 101    // GLUT_KEY_UP
#define ARROW_DOWN 103  // GLUT_KEY_DOWN
#define ARROW_LEFT 100  // GLUT_KEY_LEFT
#define ARROW_RIGHT 102 // GLUT_KEY_RIGHT
#define KEY_ESC 27      // ASCII ESC
#define KEY_SPACE 32    // ASCII Space

// Zoom controls
#define ZOOM_IN 86
#define ZOOM_OUT 82

// Physics constants
#define MOVABLE_MAX_ACCELERATION 10.0
#define MOVABLE_MAX_VELOCITY 15.0
#define MOVABLE_LINEAR_FRICTION 1.2
#define DEFAULT_GRAVITY 0.005

// Gameplay constants
#define RELOAD_ROUNDS 5          // Reload time in game rounds
#define RELOAD_HANDICAP_FOR_AI 5 // Extra reload delay for AI
#define PLAYER_ID 642            // Unique player identifier

#endif // CONSTANTS_H
