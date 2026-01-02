/**
 * Constants.h - Game constants and key mappings
 *
 * PHYSICS USE SI UNITS:
 * - Distance: meters (1 unit = 1 meter)
 * - Velocity: meters/second
 * - Acceleration: meters/second²
 * - Time: seconds (TIME_STEP in milliseconds for integer math)
 * - Angles: radians
 *
 * Reference: M1 Abrams-like main battle tank
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Time step in milliseconds (20ms = 50 physics updates/second)
#define TIME_STEP 20

#define TRUE 1

// World scaling (legacy, unused with SI)
#define SCALE 1.0
#define GRID_SIZE 1.0

// Key mappings for turbine control (legacy)
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

// =============================================================
// PHYSICS CONSTANTS (SI UNITS)
// =============================================================

// Tank movement - tuned for FAST, ARCADE gameplay
// Test showed friction=0.3 made tanks slow. Reduced significantly.
// At equilibrium: max_vel = accel / friction = 50 / 0.1 = 500 (capped at 25)
#define MOVABLE_MAX_ACCELERATION 50.0 // m/s² (very snappy)
#define MOVABLE_MAX_VELOCITY 25.0     // m/s (90 km/h, fast arcade)
#define MOVABLE_LINEAR_FRICTION 0.1   // Very low drag
#define DEFAULT_GRAVITY 9.81          // m/s² (standard gravity)

// Tank rotation (very responsive)
#define TURN_RATE 2.0 // rad/s (about 115°/s)

// Projectile physics (scaled for gameplay, real is 1000+ m/s)
#define PROJECTILE_SPEED 200.0        // m/s (~720 km/h, visible but fast)
#define PROJECTILE_MAX_DISTANCE 300.0 // m (effective range)

// =============================================================
// GAMEPLAY CONSTANTS
// =============================================================

#define RELOAD_ROUNDS 50          // Reload time in physics ticks (1 second)
#define RELOAD_HANDICAP_FOR_AI 25 // Extra reload delay for AI (0.5s)
#define PLAYER_ID 642             // Unique player identifier

// Arena dimensions (for reference)
#define ARENA_SIZE 200.0  // meters (200x200m battlefield)
#define SPAWN_RADIUS 80.0 // meters from center for enemy spawns

#endif // CONSTANTS_H
