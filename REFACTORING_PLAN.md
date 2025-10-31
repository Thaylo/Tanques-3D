# File Reorganization Plan

## Directory Structure
```
src/
  core/       - Vector.cpp, Timer.cpp
  graphics/   - GLDraw.cpp, Window.cpp, oDrawable.cpp
  entities/   - Matter.cpp, Movable.cpp, Controlable.cpp, Agent.cpp, Enemy.cpp, Projectile.cpp
  game/       - GameData.cpp, Camera.cpp, Ground.cpp, Control.cpp, Main.cpp
  input/      - joystick.cpp
  world/      - Terrain.cpp

include/
  core/       - Vector.h, Timer.h, Constants.h
  graphics/   - GLDraw.h, Window.h, oDrawable.h
  entities/   - Matter.h, Movable.h, Controlable.h, Agent.h, Enemy.h, Projectile.h
  game/       - GameData.h, Camera.h, Ground.h, Control.h
  input/      - joystick.h
  world/      - Terrain.h

assets/textures/ - All .bmp files
docs/            - All .md documentation files
```

## Files to Move

### Core Layer (3 files)
- src/core: Vector.cpp, Timer.cpp
- include/core: Vector.h, Timer.h, Constants.h

### Graphics Layer (3 files)
- src/graphics: GLDraw.cpp, Window.cpp, oDrawable.cpp  
- include/graphics: GLDraw.h, Window.h, oDrawable.h

### Entities Layer (6 files)
- src/entities: Matter.cpp, Movable.cpp, Controlable.cpp, Agent.cpp, Enemy.cpp, Projectile.cpp
- include/entities: Matter.h, Movable.h, Controlable.h, Agent.h, Enemy.h, Projectile.h

### Game Layer (5 files)
- src/game: GameData.cpp, Camera.cpp, Ground.cpp, Control.cpp, Main.cpp
- include/game: GameData.h, Camera.h, Ground.h, Control.h

### Input Layer (1 file)
- src/input: joystick.cpp
- include/input: joystick.h

### World Layer (1 file)
- src/world: Terrain.cpp
- include/world: Terrain.h

### Assets (7 texture files)
- assets/textures: frente.bmp, verso.bmp, lateralDir.bmp, lateralEsq.bmp, topo.bmp, texture.bmp, sky.bmp

### Documentation (7 files to docs/)
- BUILD_MACOS.md → docs/BUILD_MACOS.md
- BUILD_CMAKE.md → docs/BUILD_CMAKE.md
- CMAKE_*.md (5 files) → docs/
- ARCHITECTURE_REVIEW.md → docs/
- Screenshot from 2023-01-28 22-27-33.png → docs/

## Keep in Root
- CMakeLists.txt (will be updated for new structure)
- Makefile (legacy support)
- build.sh
- .gitignore
- README.md
- LEIAME
- CMakePresets.json
