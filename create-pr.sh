#!/bin/bash

# Script to create a Pull Request for macOS compatibility changes
# This script opens your web browser to create the PR on GitHub

PR_URL="https://github.com/Thaylo/Tanques-3D/pull/new/claude/study-codebase-011CUeYWyvZAExW1MvTFWCWT"

echo "Opening GitHub to create Pull Request..."
echo "URL: $PR_URL"
echo ""
echo "If the browser doesn't open automatically, copy and paste this URL:"
echo "$PR_URL"
echo ""

# Try to open the URL in the default browser
if command -v open &> /dev/null; then
    # macOS
    open "$PR_URL"
elif command -v xdg-open &> /dev/null; then
    # Linux
    xdg-open "$PR_URL"
elif command -v start &> /dev/null; then
    # Windows
    start "$PR_URL"
else
    echo "Could not detect browser. Please open the URL above manually."
fi

echo ""
echo "Suggested PR Title:"
echo "Add macOS compatibility (M1/M2/M3 Apple Silicon support)"
echo ""
echo "Suggested PR Description:"
cat << 'EOF'
## Summary
This PR refactors the Tanques-3D codebase to be fully compatible with macOS, including Apple Silicon (M1/M2/M3) Macs, while maintaining backward compatibility with Linux.

## Changes Made

### Window System Migration
- **Replaced X11/GLX with GLUT** for cross-platform compatibility
- Rewrote `Window.h` and `Window.cpp` to use GLUT callbacks instead of raw X11 event handling
- Implemented GLUT keyboard and mouse callbacks with proper event translation

### Platform-Specific Code
- Added conditional compilation directives (`#ifdef __APPLE__`) for macOS vs Linux
- Updated header includes to use `GLUT/glut.h` on macOS and `GL/glut.h` on Linux
- Configured OpenGL framework paths for macOS

### Input Handling
- Updated key codes in `Constants.h` from X11-specific values to GLUT standard codes:
  - Arrow keys: GLUT_KEY_UP (101), GLUT_KEY_DOWN (103), etc.
  - Special keys: ESC (27), Space (32)

### Build System
- Enhanced `Makefile` with automatic platform detection
- macOS: Uses `-framework OpenGL -framework GLUT`
- Linux: Uses `-lGL -lGLU -lglut -lX11`

### Documentation
- Created `BUILD_MACOS.md` with comprehensive build instructions for macOS
- Updated `README.md` to highlight cross-platform support

## Files Modified
- `Constants.h` - Updated key codes
- `GLDraw.cpp` - Removed duplicate GLUT initialization
- `GLDraw.h` - Added platform-specific includes
- `Main.cpp` - Added GLUT initialization before window creation
- `Makefile` - Platform detection and framework configuration
- `README.md` - Updated platform support documentation
- `Window.cpp` - Complete rewrite using GLUT
- `Window.h` - GLUT-based interface
- `BUILD_MACOS.md` - New file with macOS instructions

## Compatibility
✅ macOS (Intel)
✅ macOS (Apple Silicon M1/M2/M3)
✅ Linux (with freeglut3-dev)

## Building on macOS
```bash
make clean
make
./jogoThaylo 15
```

No dependencies need to be installed on macOS - OpenGL and GLUT frameworks are included with the OS.
EOF
