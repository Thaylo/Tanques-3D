cmake_minimum_required(VERSION 3.14)
project(Tanques3D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform detection and OpenGL/GLUT configuration
if(APPLE)
    find_library(OPENGL_FRAMEWORK OpenGL REQUIRED)
    find_library(GLUT_FRAMEWORK GLUT REQUIRED)
    set(PLATFORM_LIBS ${OPENGL_FRAMEWORK} ${GLUT_FRAMEWORK})
    add_compile_options(-Wno-deprecated-declarations)
else()
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    set(PLATFORM_LIBS OpenGL::GL OpenGL::GLU GLUT::GLUT m X11)
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/game
    ${CMAKE_SOURCE_DIR}/include/rendering
)

# Core library sources (testable without OpenGL)
set(CORE_SOURCES
    src/core/Vector.cpp
    src/core/Timer.cpp
)

# Entity sources
set(ENTITY_SOURCES
    src/entities/Movable.cpp
    src/entities/Matter.cpp
    src/entities/Controlable.cpp
    src/entities/Agent.cpp
    src/entities/Projectile.cpp
    src/entities/Enemy.cpp
)

# Game logic sources
set(GAME_SOURCES
    src/game/Control.cpp
    src/game/GameData.cpp
)

# Rendering sources (OpenGL dependent)
set(RENDERING_SOURCES
    src/rendering/GLDraw.cpp
    src/rendering/Camera.cpp
    src/rendering/Ground.cpp
    src/rendering/Window.cpp
    src/rendering/Drawable.cpp
    src/rendering/Terrain.cpp
    src/rendering/joystick.cpp
)

# Library for testing (excludes main)
add_library(Tanques3DLib STATIC
    ${CORE_SOURCES}
    ${ENTITY_SOURCES}
    ${GAME_SOURCES}
    ${RENDERING_SOURCES}
)
target_link_libraries(Tanques3DLib PUBLIC ${PLATFORM_LIBS})

# Main executable
add_executable(Tanques3D src/game/Main.cpp)
target_link_libraries(Tanques3D PRIVATE Tanques3DLib)

# Set working directory for assets
set_target_properties(Tanques3D PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/bin)

# Testing with GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Unit tests
add_executable(Tanques3DTests
    tests/unit/test_vector.cpp
    tests/unit/test_movable.cpp
    tests/unit/test_control.cpp
    tests/integration/test_gamedata.cpp
)
target_link_libraries(Tanques3DTests PRIVATE
    Tanques3DLib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tanques3DTests)
