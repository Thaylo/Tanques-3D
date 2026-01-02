# Tanques 3D

A 3D tank battle game built with OpenGL and GLUT.

## Features

- Player-controlled tank with arena movement
- AI-controlled enemy tanks that pursue and attack
- Projectile combat system
- Radar display for tracking nearest enemy
- Skybox and textured terrain

## Building

### Prerequisites

- CMake 3.14+
- C++14 compatible compiler
- OpenGL and GLUT libraries

**macOS:**
```bash
xcode-select --install  # Command line tools include OpenGL/GLUT
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install cmake build-essential libgl1-mesa-dev freeglut3-dev
```

### Build Instructions

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Running the Game

```bash
# From the build directory
./bin/Tanques3D [numberOfEnemies]

# Example with 5 enemies
./bin/Tanques3D 5
```

Default is 3 enemies if not specified.

## Controls

| Key | Action |
|-----|--------|
| ↑ / ↓ | Move forward / backward |
| ← / → | Turn left / right |
| Space | Fire projectile |
| ESC | Exit game |

## Running Tests

```bash
cd build
ctest --output-on-failure

# Or run directly
./Tanques3DTests
```

## Project Structure

```
Tanques-3D/
├── CMakeLists.txt
├── assets/           # Texture files (.bmp)
├── include/
│   ├── core/         # Vector, Timer, Constants
│   ├── entities/     # Agent, Projectile, Enemy, Movable
│   ├── game/         # GameData, Control
│   └── rendering/    # GLDraw, Camera, Ground, Window
├── src/              # Implementation files
└── tests/
    ├── unit/         # Unit tests
    └── integration/  # Integration tests
```

## License

Original code by Thaylo Freitas (2012-2013). Refactored for modern C++ standards.
