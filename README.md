# Tanques 3D

A 3D tank battle game built with **Vulkan** and **SDL2**, using Metal via MoltenVK on macOS.

## Features

- Player-controlled tank with arena movement
- AI-controlled enemy tanks that pursue and attack
- Projectile combat system with collision detection
- Modern C++23 codebase
- Cross-platform: macOS (Metal), Linux (Vulkan), Windows (Vulkan)

## Building

### Prerequisites

- CMake 3.14+
- C++23 compatible compiler (Clang 16+, GCC 13+)
- Vulkan SDK
- SDL2

**macOS (Homebrew):**
```bash
brew install sdl2 vulkan-headers molten-vk glm
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install cmake build-essential libsdl2-dev libvulkan-dev glslang-tools
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

73 unit and integration tests covering core game logic.

## Project Structure

```
Tanques-3D/
├── CMakeLists.txt
├── assets/              # Textures and shaders
├── include/
│   ├── core/            # Vector, Timer, Constants
│   ├── entities/        # Agent, Projectile, Enemy, Movable
│   ├── game/            # GameData, Control
│   └── rendering/       # VulkanRenderer, VulkanPipeline, VulkanWindow
├── src/
└── tests/
    ├── unit/
    └── integration/
```

## Tech Stack

| Component | Technology |
|-----------|------------|
| Graphics API | Vulkan 1.3+ |
| Windowing | SDL2 |
| macOS Backend | MoltenVK → Metal |
| Language | C++23 |
| Build | CMake |
| Tests | GoogleTest |

## License

Original code by Thaylo Freitas (2012-2013). Modernized to Vulkan/C++23 (2026).
