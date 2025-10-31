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
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

if(UNIX AND NOT APPLE)
    include_directories(${X11_INCLUDE_DIRS})
endif()

# Source files - organized by category
set(GAME_SOURCES
    Main.cpp
    GameData.cpp
    Window.cpp
)

set(GAME_LOGIC_SOURCES
    Agent.cpp
    Enemy.cpp
    Projectile.cpp
    Control.cpp
    Controlable.cpp
)

set(RENDERING_SOURCES
    GLDraw.cpp
    Camera.cpp
    oDrawable.cpp
)

set(PHYSICS_SOURCES
    Matter.cpp
    Movable.cpp
    Vector.cpp
)

set(WORLD_SOURCES
    Terrain.cpp
    Ground.cpp
)

set(UTILITY_SOURCES
    joystick.cpp
    Timer.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${GAME_SOURCES}
    ${GAME_LOGIC_SOURCES}
    ${RENDERING_SOURCES}
    ${PHYSICS_SOURCES}
    ${WORLD_SOURCES}
    ${UTILITY_SOURCES}
)

# Header files (for IDEs and documentation)
set(HEADERS
    Agent.h
    Camera.h
    Constants.h
    Control.h
    Controlable.h
    Enemy.h
    GameData.h
    GLDraw.h
    Ground.h
    joystick.h
    Matter.h
    Movable.h
    oDrawable.h
    Projectile.h
    Terrain.h
    Timer.h
    Vector.h
    Window.h
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
    frente.bmp
    lateralDir.bmp
    lateralEsq.bmp
    sky.bmp
    texture.bmp
    topo.bmp
    verso.bmp
)

# Copy asset files to build directory
# This ensures textures are available when running the executable
foreach(ASSET ${ASSET_FILES})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/${ASSET}
        ${CMAKE_BINARY_DIR}/bin/${ASSET}
        COPYONLY
    )
endforeach()

# Installation rules
install(TARGETS jogoThaylo
    RUNTIME DESTINATION bin
)

install(FILES ${ASSET_FILES}
    DESTINATION bin
)

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
