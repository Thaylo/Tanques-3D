# CMake Build System Guide

This project uses CMake for cross-platform build configuration, supporting both macOS and Linux with automatic platform detection and dependency management.

## Prerequisites

### macOS
- **Xcode Command Line Tools** (provides g++/clang++)
  ```bash
  xcode-select --install
  ```
- **CMake** (version 3.10 or higher)
  ```bash
  brew install cmake
  ```
- OpenGL and GLUT frameworks (included with macOS)

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install freeglut3-dev libglu1-mesa-dev libgl1-mesa-dev libx11-dev
```

### Linux (Fedora/RHEL)
```bash
sudo dnf install cmake gcc-c++
sudo dnf install freeglut-devel mesa-libGLU-devel mesa-libGL-devel libX11-devel
```

## Building the Project

### Quick Start (Release Build)

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build .

# Run the game
./bin/jogoThaylo 15
```

### Detailed Build Options

#### 1. Release Build (Optimized for Performance)
```bash
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

**Features:**
- Full compiler optimizations (-O3)
- No debug symbols
- Fastest runtime performance
- Recommended for playing the game

#### 2. Debug Build (For Development)
```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

**Features:**
- Debug symbols included (-g)
- Additional compiler warnings (-Wall -Wextra)
- No optimizations
- Easier to debug with gdb/lldb

#### 3. Parallel Build (Faster Compilation)
```bash
# Use all CPU cores
cmake --build . --parallel

# Or specify number of jobs
cmake --build . -j 8
```

## Build Directory Structure

After building, your directory structure will look like:

```
build/
├── bin/
│   ├── jogoThaylo          # Executable
│   ├── frente.bmp          # Texture files (auto-copied)
│   ├── lateralDir.bmp
│   ├── lateralEsq.bmp
│   ├── sky.bmp
│   ├── texture.bmp
│   ├── topo.bmp
│   └── verso.bmp
├── CMakeCache.txt
├── CMakeFiles/
└── Makefile
```

## Running the Game

### Basic Usage
```bash
# From build directory
./bin/jogoThaylo

# Specify number of enemy tanks (default: 3)
./bin/jogoThaylo 15
```

### From Any Directory
```bash
# Run with absolute path
/path/to/build/bin/jogoThaylo 20
```

## Installation (Optional)

Install the game system-wide:

```bash
# From build directory
sudo cmake --install .

# This installs to /usr/local/bin by default
# Run from anywhere:
jogoThaylo 10
```

### Custom Install Location
```bash
# Configure with custom prefix
cmake -DCMAKE_INSTALL_PREFIX=/opt/tanques3d ..
cmake --build .
sudo cmake --install .

# Run from custom location
/opt/tanques3d/bin/jogoThaylo
```

## Cleaning Build Artifacts

### Clean build files (keeps configuration)
```bash
cmake --build . --target clean
```

### Complete clean (removes everything)
```bash
# From project root
rm -rf build

# Or use custom clean target
cd build
cmake --build . --target clean-all
```

## CMake Configuration Options

### Viewing Configuration
```bash
# See all CMake variables
cmake -L ..

# See advanced options
cmake -LA ..

# See help for specific variable
cmake -DCMAKE_BUILD_TYPE:STRING=Help ..
```

### Common CMake Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `CMAKE_BUILD_TYPE` | Build type (Release/Debug) | `-DCMAKE_BUILD_TYPE=Release` |
| `CMAKE_INSTALL_PREFIX` | Installation directory | `-DCMAKE_INSTALL_PREFIX=/usr/local` |
| `CMAKE_CXX_COMPILER` | C++ compiler to use | `-DCMAKE_CXX_COMPILER=g++-11` |
| `CMAKE_VERBOSE_MAKEFILE` | Verbose build output | `-DCMAKE_VERBOSE_MAKEFILE=ON` |

### Using Different Compilers

```bash
# Use specific GCC version
cmake -DCMAKE_CXX_COMPILER=g++-11 ..

# Use Clang instead of GCC
cmake -DCMAKE_CXX_COMPILER=clang++ ..

# On macOS, use Apple Clang
cmake -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
```

## Troubleshooting

### Issue: OpenGL/GLUT Not Found

**macOS:**
```bash
# Ensure Xcode Command Line Tools installed
xcode-select --install

# Verify frameworks exist
ls /System/Library/Frameworks/OpenGL.framework
ls /System/Library/Frameworks/GLUT.framework
```

**Linux:**
```bash
# Install missing dependencies
sudo apt-get install freeglut3-dev libglu1-mesa-dev
```

### Issue: CMake Version Too Old

```bash
# Check CMake version
cmake --version

# Update CMake (macOS)
brew upgrade cmake

# Update CMake (Linux)
# Visit https://cmake.org/download/ for latest version
```

### Issue: Missing X11 (Linux)

```bash
sudo apt-get install libx11-dev
```

### Issue: Textures Not Loading

The CMake build system automatically copies texture files to the build directory. If textures are missing:

```bash
# Verify textures were copied
ls build/bin/*.bmp

# Manually reconfigure if needed
cd build
cmake ..
```

### Verbose Build Output

```bash
# See full compiler commands
cmake --build . --verbose

# Or configure with verbose makefiles
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build .
```

## CMake Features Used

This build system leverages modern CMake practices:

### 1. **Platform Detection**
- Automatic macOS vs Linux detection
- Platform-specific compiler flags
- Framework vs library linking

### 2. **Dependency Management**
- `find_package()` for OpenGL, GLUT, X11
- Automatic include path configuration
- Library linking management

### 3. **Build Type Support**
- Debug: `-g -Wall -Wextra`
- Release: `-O3` optimization
- Generator expressions for conditional compilation

### 4. **Asset Management**
- Automatic texture file copying
- Runtime resource availability
- Installation rules for assets

### 5. **Source Organization**
- Logical grouping (Game, Physics, Rendering)
- Maintainable structure
- Easy to add new files

### 6. **Modern CMake Standards**
- Target-based approach
- Interface properties
- Clear dependencies

## IDE Integration

### Visual Studio Code
```bash
# Install CMake Tools extension
# Open project folder
# CMake will auto-configure
```

### CLion
```bash
# Open CMakeLists.txt as project
# CLion automatically configures
```

### Xcode (macOS)
```bash
# Generate Xcode project
mkdir build-xcode
cd build-xcode
cmake -G Xcode ..
open Tanques3D.xcodeproj
```

## Comparison with Makefile

| Feature | Makefile | CMake |
|---------|----------|-------|
| Platform detection | Manual shell commands | Automatic |
| Dependency management | Manual flags | `find_package()` |
| Build types | Single configuration | Debug/Release |
| IDE support | None | Full support |
| Out-of-source builds | No | Yes |
| Installation rules | Manual | Built-in |
| Cross-platform | Platform-specific | Universal |
| Asset handling | Manual | Automatic |

## Advanced Usage

### Cross-Compilation
```bash
# Example: Build for different architecture
cmake -DCMAKE_SYSTEM_PROCESSOR=arm64 ..
```

### Custom Build Directory Names
```bash
# Organize multiple configurations
mkdir build-{release,debug,profiling}

cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..

cd ../build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

### Build Benchmarking
```bash
# Time the build
time cmake --build . --parallel

# Profile compilation
cmake --build . --verbose 2>&1 | tee build.log
```

## Migration from Makefile

The old Makefile is still functional but CMake is recommended:

```bash
# Old way (still works)
make clean
make

# New way (recommended)
mkdir build && cd build
cmake ..
cmake --build .
```

## Contributing

When adding new source files:

1. Add `.cpp` files to appropriate category in `CMakeLists.txt`:
   ```cmake
   set(GAME_LOGIC_SOURCES
       Agent.cpp
       Enemy.cpp
       YourNewFile.cpp  # Add here
   )
   ```

2. Add `.h` files to `HEADERS`:
   ```cmake
   set(HEADERS
       Agent.h
       YourNewHeader.h  # Add here
   )
   ```

3. Reconfigure CMake:
   ```bash
   cd build
   cmake ..
   cmake --build .
   ```

## Support

For issues with the build system:
1. Check CMake version: `cmake --version`
2. Verify dependencies are installed
3. Try clean rebuild: `rm -rf build && mkdir build && cd build && cmake ..`
4. Check CMake output for error messages

## Resources

- [CMake Documentation](https://cmake.org/documentation/)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [Modern CMake](https://cliutils.gitlab.io/modern-cmake/)
