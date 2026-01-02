cmake_minimum_required(VERSION 3.14)
project(Tanques3D VERSION 2.0.0 LANGUAGES CXX)

# Use C++23 standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Building Tanques3D with Vulkan/SDL2 renderer (Metal via MoltenVK on macOS)")

# Find Vulkan and SDL2
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)

if(APPLE)
    # MoltenVK provides Vulkan on macOS
    set(PLATFORM_LIBS 
        Vulkan::Vulkan 
        SDL2::SDL2
        "-framework Metal"
        "-framework QuartzCore"
        "-framework IOSurface"
    )
    add_compile_definitions(VK_USE_PLATFORM_METAL_EXT)
else()
    set(PLATFORM_LIBS 
        Vulkan::Vulkan 
        SDL2::SDL2
    )
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/game
    ${CMAKE_SOURCE_DIR}/include/rendering
    ${Vulkan_INCLUDE_DIRS}
)

if(APPLE)
    include_directories(/opt/homebrew/include)  # For GLM
endif()

# Core library sources (testable, renderer-independent)
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

# Vulkan rendering sources
set(RENDERING_SOURCES
    src/rendering/VulkanRenderer.cpp
    src/rendering/VulkanPipeline.cpp
    src/rendering/VulkanWindow.cpp
    src/rendering/Drawable.cpp
)

# Library for testing (excludes main and OpenGL)
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
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Unit tests (renderer-independent core logic)
add_executable(Tanques3DTests
    tests/unit/test_vector.cpp
    tests/unit/test_movable.cpp
    tests/unit/test_control.cpp
    tests/unit/test_timer.cpp
    tests/integration/test_gamedata.cpp
)
target_link_libraries(Tanques3DTests PRIVATE
    Tanques3DLib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(Tanques3DTests)
