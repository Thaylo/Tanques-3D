cmake_minimum_required(VERSION 3.14)

# Enable testing
enable_testing()

# Include FetchContent module for downloading dependencies
include(FetchContent)

# Download and configure Google Test
message(STATUS "Fetching Google Test...")
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0  # Use a specific stable version
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Make Google Test available
FetchContent_MakeAvailable(googletest)

# Find required graphics libraries (same as main project)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Include directories for tests
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/physics
    ${CMAKE_SOURCE_DIR}/include/graphics
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/game
    ${CMAKE_SOURCE_DIR}/include/input
    ${CMAKE_SOURCE_DIR}/include/world
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

# Define source files needed for testing (exclude Main.cpp to avoid multiple main() definitions)
set(TEST_CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/Vector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Timer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Matrix3x3.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Quaternion.cpp
)

set(TEST_PHYSICS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/physics/RigidBody.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/BoxShape.cpp
)

set(TEST_ENTITIES_SOURCES
    ${CMAKE_SOURCE_DIR}/src/entities/Matter.cpp
    ${CMAKE_SOURCE_DIR}/src/entities/Movable.cpp
)

# Combine sources needed for testing
set(TEST_LIB_SOURCES
    ${TEST_CORE_SOURCES}
    ${TEST_PHYSICS_SOURCES}
    ${TEST_ENTITIES_SOURCES}
)

# Collect all test files
file(GLOB TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp
)

# Remove test_main.cpp from the glob and add it separately
list(REMOVE_ITEM TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp)

# Create test executable
add_executable(physics_tests
    test_main.cpp
    ${TEST_SOURCES}
    ${TEST_LIB_SOURCES}
)

# Link against Google Test and graphics libraries
target_link_libraries(physics_tests
    GTest::gtest
    GTest::gtest_main
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

# Set C++ standard for tests (matching main project)
target_compile_features(physics_tests PRIVATE cxx_std_11)

# Compiler options for tests
target_compile_options(physics_tests PRIVATE
    -Wall
    -Wextra
    $<$<PLATFORM_ID:Darwin>:-Wno-deprecated>
)

# Set output directory for test executable
set_target_properties(physics_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
)

# Register tests with CTest for integration
include(GoogleTest)
gtest_discover_tests(physics_tests)

# Add custom target to run tests with verbose output
add_custom_target(run_tests
    COMMAND ${CMAKE_BINARY_DIR}/bin/tests/physics_tests --gtest_color=yes
    DEPENDS physics_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running physics tests..."
)

# Print configuration
message(STATUS "")
message(STATUS "=== Test Configuration ===")
message(STATUS "Google Test version: 1.14.0")
message(STATUS "Test executable: ${CMAKE_BINARY_DIR}/bin/tests/physics_tests")
message(STATUS "Test sources: ${TEST_SOURCES}")
message(STATUS "=========================")
message(STATUS "")
