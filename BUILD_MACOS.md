# Building Tanques-3D on macOS

This codebase has been refactored to be compatible with macOS (including Apple Silicon M1/M2/M3).

## Prerequisites

On macOS, GLUT comes pre-installed as part of the system frameworks, so you don't need to install any additional dependencies.

## Building

1. Open Terminal and navigate to the project directory:
```bash
cd /path/to/Tanques-3D
```

2. Clean any previous build artifacts:
```bash
make clean
```

3. Build the project:
```bash
make
```

The Makefile automatically detects macOS and uses the appropriate frameworks (`-framework OpenGL -framework GLUT`).

## Running

Run the game with:
```bash
./jogoThaylo
```

Or specify the number of enemies:
```bash
./jogoThaylo 15
```

## Controls

- **Arrow Keys**: Move tank (forward/backward/turn)
- **Space**: Fire weapon
- **ESC**: Exit game

## Game Objective

Destroy all enemy tanks while staying alive. Last tank standing wins!

## Key Changes for macOS Compatibility

The following files were modified to support macOS:

1. **Constants.h** - Updated key codes from X11 to GLUT values
2. **Window.h** - Replaced X11/GLX with GLUT
3. **Window.cpp** - Complete rewrite using GLUT instead of raw X11
4. **Main.cpp** - Added proper GLUT initialization
5. **GLDraw.h** - Added platform-specific header includes
6. **GLDraw.cpp** - Removed duplicate glutInit call
7. **Makefile** - Added automatic platform detection for macOS vs Linux

## Troubleshooting

If you encounter compilation errors:

- Ensure you're using a compatible C++ compiler (clang on macOS)
- Make sure Xcode Command Line Tools are installed:
  ```bash
  xcode-select --install
  ```

## Platform Support

This codebase now works on:
- **macOS** (Intel and Apple Silicon)
- **Linux** (with freeglut3-dev installed)
