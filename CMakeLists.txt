cmake_minimum_required(VERSION 3.10)

# Project definition
project(Tanques3D
    VERSION 1.0.0
    DESCRIPTION "War tanks in 3D made with OpenGL and C++"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add option for build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Print build configuration
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Platform detection and OpenGL/GLUT setup
if(APPLE)
    # macOS-specific configuration
    message(STATUS "Configuring for macOS")

    # Find OpenGL framework (built-in on macOS)
    find_package(OpenGL REQUIRED)

    # GLUT framework is also built-in on macOS
    find_package(GLUT REQUIRED)

    # macOS compiler flags
    set(PLATFORM_COMPILE_OPTIONS -Wno-deprecated)

    # Frameworks to link on macOS
    set(PLATFORM_LIBRARIES
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
    )

    # Define __APPLE__ for conditional compilation
    add_compile_definitions(__APPLE__)

elseif(UNIX AND NOT APPLE)
    # Linux-specific configuration
    message(STATUS "Configuring for Linux")

    # Find required packages on Linux
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(X11 REQUIRED)

    # Linux libraries
    set(PLATFORM_LIBRARIES
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        ${X11_LIBRARIES}
        m  # Math library
    )

    set(PLATFORM_COMPILE_OPTIONS "")

else()
    message(FATAL_ERROR "Unsupported platform. Only macOS and Linux are supported.")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
    ${CMAKE_CURRENT_SOURCE_DIR}/include/graphics
    ${CMAKE_CURRENT_SOURCE_DIR}/include/entities
    ${CMAKE_CURRENT_SOURCE_DIR}/include/game
    ${CMAKE_CURRENT_SOURCE_DIR}/include/input
    ${CMAKE_CURRENT_SOURCE_DIR}/include/world
    ${CMAKE_CURRENT_SOURCE_DIR}/include/physics
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

if(UNIX AND NOT APPLE)
    include_directories(${X11_INCLUDE_DIRS})
endif()

# Source files - organized by layer and directory structure
set(CORE_SOURCES
    src/core/Vector.cpp
    src/core/Timer.cpp
    src/core/Matrix3x3.cpp
    src/core/Quaternion.cpp
)

set(GRAPHICS_SOURCES
    src/graphics/GLDraw.cpp
    src/graphics/Window.cpp
    src/graphics/oDrawable.cpp
)

set(ENTITIES_SOURCES
    src/entities/Matter.cpp
    src/entities/Movable.cpp
    src/entities/Controlable.cpp
    src/entities/Agent.cpp
    src/entities/Enemy.cpp
    src/entities/Projectile.cpp
)

set(GAME_SOURCES
    src/game/Main.cpp
    src/game/GameData.cpp
    src/game/Camera.cpp
    src/game/Ground.cpp
    src/game/Control.cpp
)

set(INPUT_SOURCES
    src/input/joystick.cpp
)

set(WORLD_SOURCES
    src/world/Terrain.cpp
)

set(PHYSICS_SOURCES
    src/physics/RigidBody.cpp
    src/physics/BoxShape.cpp
    src/physics/SphereShape.cpp
    src/physics/CylinderShape.cpp
    src/physics/PyramidShape.cpp
    src/physics/Octree.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${ENTITIES_SOURCES}
    ${GAME_SOURCES}
    ${INPUT_SOURCES}
    ${WORLD_SOURCES}
    ${PHYSICS_SOURCES}
)

# Header files (for IDEs and documentation)
set(HEADERS
    include/core/Vector.h
    include/core/Timer.h
    include/core/Constants.h
    include/core/Matrix3x3.h
    include/core/Quaternion.h
    include/graphics/GLDraw.h
    include/graphics/Window.h
    include/graphics/oDrawable.h
    include/entities/Matter.h
    include/entities/Movable.h
    include/entities/Controlable.h
    include/entities/Agent.h
    include/entities/Enemy.h
    include/entities/Projectile.h
    include/game/GameData.h
    include/game/Camera.h
    include/game/Ground.h
    include/game/Control.h
    include/input/joystick.h
    include/world/Terrain.h
    include/physics/RigidBody.h
    include/physics/Shape.h
    include/physics/BoxShape.h
    include/physics/SphereShape.h
    include/physics/CylinderShape.h
    include/physics/PyramidShape.h
    include/physics/Octree.h
)

# Create executable target
add_executable(jogoThaylo ${ALL_SOURCES} ${HEADERS})

# Set target properties
set_target_properties(jogoThaylo PROPERTIES
    OUTPUT_NAME "jogoThaylo"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Compiler options
target_compile_options(jogoThaylo PRIVATE
    ${PLATFORM_COMPILE_OPTIONS}
    # Additional warnings for better code quality
    $<$<CONFIG:Debug>:-Wall -Wextra -g>
    $<$<CONFIG:Release>:-O3>
)

# Link libraries
target_link_libraries(jogoThaylo PRIVATE
    ${PLATFORM_LIBRARIES}
)

# Asset files (textures)
set(ASSET_FILES
    assets/textures/frente.bmp
    assets/textures/lateralDir.bmp
    assets/textures/lateralEsq.bmp
    assets/textures/sky.bmp
    assets/textures/texture.bmp
    assets/textures/topo.bmp
    assets/textures/verso.bmp
)

# Copy asset files to build directory
# This ensures textures are available when running the executable
foreach(ASSET ${ASSET_FILES})
    get_filename_component(ASSET_NAME ${ASSET} NAME)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}
        ${CMAKE_BINARY_DIR}/bin/${ASSET_NAME}
        COPYONLY
    )
endforeach()

# Installation rules
install(TARGETS jogoThaylo
    RUNTIME DESTINATION bin
)

# Install textures (extract just filenames for installation)
foreach(ASSET ${ASSET_FILES})
    install(FILES ${ASSET} DESTINATION bin)
endforeach()

# Print configuration summary
message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "OpenGL include: ${OPENGL_INCLUDE_DIR}")
message(STATUS "GLUT include: ${GLUT_INCLUDE_DIR}")
if(UNIX AND NOT APPLE)
    message(STATUS "X11 include: ${X11_INCLUDE_DIR}")
endif()
message(STATUS "=============================")
message(STATUS "")

# Add custom target for cleaning build artifacts
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/Makefile
    COMMENT "Removing all build artifacts"
)

# Print helpful messages
add_custom_command(TARGET jogoThaylo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Build complete! Run the game with:"
    COMMAND ${CMAKE_COMMAND} -E echo "  ${CMAKE_BINARY_DIR}/bin/jogoThaylo [num_enemies]"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Example: ${CMAKE_BINARY_DIR}/bin/jogoThaylo 15"
    COMMAND ${CMAKE_COMMAND} -E echo ""
)

# Enable testing support (optional, controlled by BUILD_TESTING option)
option(BUILD_TESTING "Build the testing tree" ON)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
    message(STATUS "Testing enabled - tests will be built")
endif()
