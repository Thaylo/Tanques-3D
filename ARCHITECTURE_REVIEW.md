# Tanques-3D Architecture Review

## Executive Summary

This is a 3D tank combat game built with C++ and OpenGL/GLUT. The codebase consists of 37 source files (18 headers + 19 implementation files) implementing a complete game engine with physics, rendering, AI, and input handling. The current architecture follows an object-oriented design with inheritance hierarchies but lacks modern project structure organization.

## Current Architecture Overview

### Component Analysis

The codebase can be categorized into 5 distinct architectural layers:

#### 1. Core Engine Layer (Rendering & Window Management)
**Purpose:** Low-level OpenGL interaction and window management

**Files:**
- `/Users/thaylofreitas/Projects/Tanques-3D/GLDraw.h` / `GLDraw.cpp` - OpenGL drawing primitives and texture loading
- `/Users/thaylofreitas/Projects/Tanques-3D/Window.h` / `Window.cpp` - GLUT window management and callback handling
- `/Users/thaylofreitas/Projects/Tanques-3D/oDrawable.h` / `oDrawable.cpp` - Abstract drawable interface

**Responsibilities:**
- OpenGL initialization and state management
- Texture loading from BMP files
- Drawing primitives (lines, rectangles, triangles, text)
- Window creation and event callbacks
- Platform abstraction (macOS/Linux differences)

**Dependencies:** GLUT, OpenGL, platform-specific headers

---

#### 2. Math & Utilities Layer
**Purpose:** Mathematical operations and timing utilities

**Files:**
- `/Users/thaylofreitas/Projects/Tanques-3D/Vector.h` / `Vector.cpp` - 3D vector mathematics
- `/Users/thaylofreitas/Projects/Tanques-3D/Timer.h` / `Timer.cpp` - Time measurement utilities
- `/Users/thaylofreitas/Projects/Tanques-3D/Constants.h` - Game constants and configuration

**Responsibilities:**
- 3D vector operations (add, subtract, multiply, divide)
- Vector utilities (dot product, cross product, normalization, rotation)
- Distance and angle calculations
- Timing functions for game loop
- Game configuration constants (physics, keys, timing)

**Dependencies:** Standard math libraries (cmath)

---

#### 3. Physics & Movement Layer
**Purpose:** Physical properties and movement mechanics

**Files:**
- `/Users/thaylofreitas/Projects/Tanques-3D/Movable.h` / `Movable.cpp` - Base movement class
- `/Users/thaylofreitas/Projects/Tanques-3D/Matter.h` / `Matter.cpp` - Physical properties (mass, charge)

**Responsibilities:**
- Position, velocity, acceleration management
- 3D orientation (roll, pitch, yaw)
- Direction vectors (up, dir, side)
- Physics iteration/update
- Mass and charge properties

**Dependencies:** Vector class

---

#### 4. Input & Control Layer
**Purpose:** User input handling and control abstraction

**Files:**
- `/Users/thaylofreitas/Projects/Tanques-3D/Control.h` / `Control.cpp` - Input state structure
- `/Users/thaylofreitas/Projects/Tanques-3D/Controlable.h` / `Controlable.cpp` - Controllable entity interface
- `/Users/thaylofreitas/Projects/Tanques-3D/joystick.h` / `joystick.cpp` - Linux joystick support

**Responsibilities:**
- Keyboard state management (arrow keys, space, ESC)
- Mouse input state
- Control binding to entities
- Abstract control action interface
- Joystick input handling (Linux only)

**Dependencies:** Movable class

---

#### 5. Game Logic Layer
**Purpose:** Game entities, AI, and gameplay mechanics

**Files:**
- `/Users/thaylofreitas/Projects/Tanques-3D/Agent.h` / `Agent.cpp` - Tank entity (player/enemy base)
- `/Users/thaylofreitas/Projects/Tanques-3D/Enemy.h` / `Enemy.cpp` - AI-controlled enemy tank
- `/Users/thaylofreitas/Projects/Tanques-3D/Projectile.h` / `Projectile.cpp` - Bullet/projectile entity
- `/Users/thaylofreitas/Projects/Tanques-3D/GameData.h` / `GameData.cpp` - Game state manager
- `/Users/thaylofreitas/Projects/Tanques-3D/Camera.h` / `Camera.cpp` - 3rd person camera
- `/Users/thaylofreitas/Projects/Tanques-3D/Ground.h` / `Ground.cpp` - Ground/terrain rendering
- `/Users/thaylofreitas/Projects/Tanques-3D/Terrain.h` / `Terrain.cpp` - Terrain (stub implementation)
- `/Users/thaylofreitas/Projects/Tanques-3D/Main.cpp` - Application entry point

**Responsibilities:**
- Tank rendering with textured models
- AI behavior (enemy targeting and shooting)
- Projectile physics and lifecycle
- Game state management (agent array, collision detection)
- Camera tracking and positioning
- Ground plane rendering
- Main game loop and initialization

**Dependencies:** All other layers

---

## Class Hierarchy Analysis

### Inheritance Structure

```
oDrawable (abstract)
├── Agent
│   ├── Enemy
│   └── Projectile
├── Camera
└── Ground

Movable
├── Controlable (abstract)
│   └── Agent
├── Camera
└── Ground

Matter
└── Agent
```

### Key Design Patterns

1. **Multiple Inheritance:** `Agent` inherits from `Controlable`, `oDrawable`, and `Matter`
2. **Abstract Interfaces:** `oDrawable::draw()` and `Controlable::controlAction()` are pure virtual
3. **Composition:** `GameData` owns all agents, camera, ground, and control state
4. **Polymorphism:** Agents array contains different types (Agent, Enemy, Projectile)

---

## Dependency Graph

```
Main.cpp
  ├─> Window (GLUT callbacks)
  ├─> GLDraw (OpenGL init)
  ├─> Timer (game loop timing)
  ├─> GameData
  │     ├─> Control
  │     ├─> Agent (player)
  │     ├─> Enemy (multiple instances)
  │     ├─> Projectile (spawned dynamically)
  │     ├─> Camera
  │     └─> Ground
  │
  └─> Camera

Agent
  ├─> Movable
  ├─> oDrawable
  ├─> Matter
  ├─> Controlable
  │     └─> Control
  ├─> Vector
  └─> GameData (extern reference - CIRCULAR DEPENDENCY WARNING)

Enemy : Agent
  └─> Agent (target reference)

Projectile : Agent
  └─> Agent (shooter reference)

Camera : Movable, oDrawable
  └─> Movable (tracked entity)

Ground : Movable, oDrawable
  └─> Agent (tracked entity)
```

### Critical Issues Identified

1. **Circular Dependency:** `Agent.cpp` includes `GameData.h` and uses `extern GameData* gameData`
2. **Tight Coupling:** Agents directly access global GameData for collision detection
3. **No Separation:** Headers and implementation files mixed in root directory
4. **Build Artifacts:** Object files (.o) and executable in source directory
5. **Asset Files:** Texture files (.bmp) in root alongside code

---

## Current File Organization

All 73 files are in the root directory:
- 18 header files (.h)
- 19 implementation files (.cpp)
- 20 object files (.o)
- 7 texture files (.bmp)
- 1 screenshot (.png)
- 1 executable (jogoThaylo)
- 7 configuration/documentation files

This flat structure makes navigation difficult and violates separation of concerns.

---

## Proposed Modern C++ Project Structure

### Directory Layout

```
Tanques-3D/
├── .git/                          # Git repository data
├── .gitignore                     # Git ignore rules
├── README.md                      # Project overview
├── BUILD_MACOS.md                 # Build instructions
├── Makefile                       # Build system
│
├── src/                           # Source files (.cpp)
│   ├── main/
│   │   └── Main.cpp              # Application entry point
│   │
│   ├── core/                      # Core engine
│   │   ├── GLDraw.cpp
│   │   ├── Window.cpp
│   │   └── oDrawable.cpp
│   │
│   ├── math/                      # Math utilities
│   │   ├── Vector.cpp
│   │   └── Timer.cpp
│   │
│   ├── physics/                   # Physics engine
│   │   ├── Movable.cpp
│   │   └── Matter.cpp
│   │
│   ├── input/                     # Input handling
│   │   ├── Control.cpp
│   │   ├── Controlable.cpp
│   │   └── joystick.cpp
│   │
│   ├── game/                      # Game logic
│   │   ├── entities/
│   │   │   ├── Agent.cpp
│   │   │   ├── Enemy.cpp
│   │   │   └── Projectile.cpp
│   │   │
│   │   ├── world/
│   │   │   ├── Camera.cpp
│   │   │   ├── Ground.cpp
│   │   │   └── Terrain.cpp
│   │   │
│   │   └── GameData.cpp          # Game state manager
│   │
│   └── CMakeLists.txt             # Optional: per-directory build config
│
├── include/                       # Header files (.h)
│   ├── core/
│   │   ├── GLDraw.h
│   │   ├── Window.h
│   │   └── oDrawable.h
│   │
│   ├── math/
│   │   ├── Vector.h
│   │   ├── Timer.h
│   │   └── Constants.h           # Global constants
│   │
│   ├── physics/
│   │   ├── Movable.h
│   │   └── Matter.h
│   │
│   ├── input/
│   │   ├── Control.h
│   │   ├── Controlable.h
│   │   └── joystick.h
│   │
│   └── game/
│       ├── entities/
│       │   ├── Agent.h
│       │   ├── Enemy.h
│       │   └── Projectile.h
│       │
│       ├── world/
│       │   ├── Camera.h
│       │   ├── Ground.h
│       │   └── Terrain.h
│       │
│       └── GameData.h
│
├── assets/                        # Game assets
│   ├── textures/
│   │   ├── tank/
│   │   │   ├── frente.bmp
│   │   │   ├── verso.bmp
│   │   │   ├── lateralDir.bmp
│   │   │   ├── lateralEsq.bmp
│   │   │   └── topo.bmp
│   │   │
│   │   ├── environment/
│   │   │   ├── texture.bmp      # Ground texture
│   │   │   └── sky.bmp
│   │   │
│   │   └── README.md             # Asset credits/licenses
│   │
│   └── screenshots/
│       └── gameplay_screenshot.png
│
├── build/                         # Build output (gitignored)
│   ├── bin/                       # Executables
│   │   └── jogoThaylo
│   │
│   ├── obj/                       # Object files
│   │   └── *.o
│   │
│   └── lib/                       # Static/shared libraries (future)
│
├── docs/                          # Documentation
│   ├── ARCHITECTURE_REVIEW.md    # This document
│   ├── API.md                    # API documentation
│   ├── CONTRIBUTING.md           # Contribution guidelines
│   └── DESIGN_DECISIONS.md       # Design rationale
│
├── tests/                         # Unit tests (future)
│   ├── math/
│   │   └── VectorTest.cpp
│   │
│   └── physics/
│       └── MovableTest.cpp
│
└── tools/                         # Build/development tools
    ├── scripts/
    │   ├── build.sh              # Build script
    │   └── clean.sh              # Clean script
    │
    └── cmake/                     # CMake modules (if using CMake)
        └── FindGLUT.cmake
```

---

## Detailed File Placement

### Layer 1: Core Engine (src/core/, include/core/)
| Current File | New Location |
|--------------|--------------|
| GLDraw.h | include/core/GLDraw.h |
| GLDraw.cpp | src/core/GLDraw.cpp |
| Window.h | include/core/Window.h |
| Window.cpp | src/core/Window.cpp |
| oDrawable.h | include/core/oDrawable.h |
| oDrawable.cpp | src/core/oDrawable.cpp |

### Layer 2: Math & Utilities (src/math/, include/math/)
| Current File | New Location |
|--------------|--------------|
| Vector.h | include/math/Vector.h |
| Vector.cpp | src/math/Vector.cpp |
| Timer.h | include/math/Timer.h |
| Timer.cpp | src/math/Timer.cpp |
| Constants.h | include/math/Constants.h |

### Layer 3: Physics (src/physics/, include/physics/)
| Current File | New Location |
|--------------|--------------|
| Movable.h | include/physics/Movable.h |
| Movable.cpp | src/physics/Movable.cpp |
| Matter.h | include/physics/Matter.h |
| Matter.cpp | src/physics/Matter.cpp |

### Layer 4: Input (src/input/, include/input/)
| Current File | New Location |
|--------------|--------------|
| Control.h | include/input/Control.h |
| Control.cpp | src/input/Control.cpp |
| Controlable.h | include/input/Controlable.h |
| Controlable.cpp | src/input/Controlable.cpp |
| joystick.h | include/input/joystick.h |
| joystick.cpp | src/input/joystick.cpp |

### Layer 5: Game Logic (src/game/, include/game/)
| Current File | New Location |
|--------------|--------------|
| Main.cpp | src/main/Main.cpp |
| GameData.h | include/game/GameData.h |
| GameData.cpp | src/game/GameData.cpp |
| Agent.h | include/game/entities/Agent.h |
| Agent.cpp | src/game/entities/Agent.cpp |
| Enemy.h | include/game/entities/Enemy.h |
| Enemy.cpp | src/game/entities/Enemy.cpp |
| Projectile.h | include/game/entities/Projectile.h |
| Projectile.cpp | src/game/entities/Projectile.cpp |
| Camera.h | include/game/world/Camera.h |
| Camera.cpp | src/game/world/Camera.cpp |
| Ground.h | include/game/world/Ground.h |
| Ground.cpp | src/game/world/Ground.cpp |
| Terrain.h | include/game/world/Terrain.h |
| Terrain.cpp | src/game/world/Terrain.cpp |

### Assets (assets/)
| Current File | New Location |
|--------------|--------------|
| frente.bmp | assets/textures/tank/frente.bmp |
| verso.bmp | assets/textures/tank/verso.bmp |
| lateralDir.bmp | assets/textures/tank/lateralDir.bmp |
| lateralEsq.bmp | assets/textures/tank/lateralEsq.bmp |
| topo.bmp | assets/textures/tank/topo.bmp |
| texture.bmp | assets/textures/environment/texture.bmp |
| sky.bmp | assets/textures/environment/sky.bmp |
| Screenshot from 2023-01-28 22-27-33.png | assets/screenshots/gameplay_screenshot.png |

---

## Rationale for Organization

### 1. Separation of Concerns
**Problem:** All files mixed in root directory
**Solution:** Separate directories by responsibility (core, math, physics, input, game)
**Benefits:**
- Easy to locate related functionality
- Clear architectural boundaries
- Prevents accidental coupling between layers

### 2. Header/Source Separation
**Problem:** Headers and implementation files mixed
**Solution:** `include/` for headers, `src/` for implementation
**Benefits:**
- Standard C++ project structure
- Clearer public API (headers)
- Easier to create libraries in future

### 3. Build Artifact Isolation
**Problem:** .o files and executable in source directory
**Solution:** `build/` directory for all build outputs
**Benefits:**
- Clean source tree
- Easy to clean build (rm -rf build/)
- gitignore pattern: `build/`

### 4. Asset Management
**Problem:** Texture files mixed with code
**Solution:** `assets/` directory with subdirectories by type
**Benefits:**
- Easy to locate and modify assets
- Asset versioning and management
- Supports future asset pipeline

### 5. Logical Grouping
**Problem:** Flat file structure
**Solution:** Hierarchical grouping (entities/, world/)
**Benefits:**
- Scales with project growth
- Related files grouped together
- Mirrors mental model of architecture

### 6. Documentation Centralization
**Problem:** Documentation files in root
**Solution:** `docs/` directory
**Benefits:**
- All documentation in one place
- Supports multiple docs without root clutter
- Professional project appearance

---

## Recommended Implementation Plan

### Phase 1: Create Directory Structure
1. Create all new directories
2. Update .gitignore to ignore `build/` directory
3. Update README.md with new structure

### Phase 2: Move Core Layer (Lowest Dependencies)
1. Move math utilities (Vector, Timer, Constants)
2. Move physics layer (Movable, Matter)
3. Update Makefile with new paths
4. Test build

### Phase 3: Move Engine Layer
1. Move rendering layer (GLDraw, Window, oDrawable)
2. Update include paths
3. Test build

### Phase 4: Move Input Layer
1. Move control system (Control, Controlable, joystick)
2. Update include paths
3. Test build

### Phase 5: Move Game Logic Layer
1. Move game entities (Agent, Enemy, Projectile)
2. Move world (Camera, Ground, Terrain)
3. Move GameData and Main
4. Update all include paths
5. Test build

### Phase 6: Move Assets
1. Create assets directory structure
2. Move texture files
3. Update texture loading paths in GLDraw.cpp
4. Test rendering

### Phase 7: Update Build System
1. Update Makefile with new directory structure
2. Add include path flags (-I include/)
3. Add object file output to build/obj/
4. Add executable output to build/bin/
5. Consider migrating to CMake for better cross-platform support

### Phase 8: Documentation
1. Move this document to docs/
2. Create docs/API.md
3. Update README.md with build instructions
4. Add docs/CONTRIBUTING.md

---

## Makefile Changes Required

### Current Makefile Issues
- No include path specification
- Object files in root directory
- Executable in root directory

### Proposed Makefile Structure

```makefile
# Detect operating system
UNAME_S := $(shell uname -s)

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Target executable
TARGET = $(BIN_DIR)/jogoThaylo

# Compiler
CXX = g++
CXXFLAGS = -I$(INC_DIR) -Wall -Wextra -std=c++11

# Platform-specific flags
ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -framework OpenGL -framework GLUT -Wno-deprecated
else
    LDFLAGS = -lm -lGLU -lGL -lglut -lX11
endif

# Source files (auto-discovered)
SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Create necessary directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))
$(shell mkdir -p $(OBJ_DIR)/main $(OBJ_DIR)/core $(OBJ_DIR)/math)
$(shell mkdir -p $(OBJ_DIR)/physics $(OBJ_DIR)/input $(OBJ_DIR)/game)
$(shell mkdir -p $(OBJ_DIR)/game/entities $(OBJ_DIR)/game/world)

# Build rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

.PHONY: all clean rebuild
```

---

## Additional Recommendations

### 1. Fix Circular Dependency
**Current Issue:** Agent.cpp uses `extern GameData* gameData`
**Solution:**
- Remove direct GameData access from Agent
- Pass necessary data (agent array, count) to Agent::iterate() as parameters
- Or implement proper event/messaging system

### 2. Use Smart Pointers
**Current Issue:** Raw pointers with manual memory management
**Solution:**
- Replace raw pointers with `std::unique_ptr` and `std::shared_ptr`
- Use `std::vector<std::unique_ptr<Agent>>` instead of `Agent* agents[4000]`

### 3. Namespace Organization
**Current Issue:** Inconsistent namespace usage
**Solution:**
- Create `tanques` namespace for all game code
- Nested namespaces: `tanques::core`, `tanques::math`, `tanques::game`

### 4. Configuration File
**Current Issue:** Constants hardcoded in Constants.h
**Solution:**
- External configuration file (JSON/INI)
- Runtime configuration loading
- Allows tweaking without recompilation

### 5. Asset Loading Abstraction
**Current Issue:** Hardcoded texture paths in code
**Solution:**
- Resource manager class
- Asset manifest file
- Supports different texture formats (not just BMP)

### 6. CMake Migration
**Current Issue:** Simple Makefile doesn't scale
**Solution:**
- Migrate to CMake for better cross-platform support
- Automatic dependency tracking
- Better IDE integration

### 7. Unit Testing
**Current Issue:** No tests
**Solution:**
- Add tests/ directory
- Use Google Test or Catch2
- Test Vector math, physics calculations

---

## Code Quality Observations

### Strengths
1. Clear class hierarchy with proper inheritance
2. Separation of interface (oDrawable, Controlable) from implementation
3. Vector class with comprehensive mathematical operations
4. Platform abstraction for macOS/Linux
5. Reasonable physics abstraction (Movable, Matter)

### Areas for Improvement
1. **Memory Management:** Raw pointers, manual delete, potential leaks
2. **Global Variables:** Multiple extern declarations (gameData, level, tank[], etc.)
3. **Error Handling:** No exception handling, no null checks in many places
4. **Magic Numbers:** Many hardcoded values in code (0.2, 0.23, etc.)
5. **Comments:** Minimal code documentation
6. **Naming:** Inconsistent (Agent vs oDrawable, camelCase vs snake_case)

---

## Conclusion

The Tanques-3D project demonstrates solid object-oriented design principles but suffers from organizational issues typical of early-stage projects. The proposed directory structure addresses these issues while maintaining the existing architecture.

### Key Benefits of Reorganization
1. **Maintainability:** Clear structure makes code easier to navigate
2. **Scalability:** Room to grow without clutter
3. **Professionalism:** Industry-standard project layout
4. **Collaboration:** Easier for new developers to contribute
5. **Build Management:** Cleaner separation of source and build artifacts

### Next Steps
1. Implement directory structure (Phase 1)
2. Incrementally move files by layer (Phases 2-6)
3. Update build system (Phase 7)
4. Document changes (Phase 8)
5. Consider additional improvements (smart pointers, CMake, testing)

### Estimated Effort
- Directory creation: 30 minutes
- File migration: 2-3 hours
- Build system updates: 1-2 hours
- Testing and verification: 1-2 hours
- **Total: 5-8 hours**

This restructuring effort will pay dividends in long-term maintainability and will establish a solid foundation for future enhancements to the game.
