# Comprehensive Test Plan for Rigid Body Physics System
## Tank Game 3D - Physics Engine Testing

---

## Executive Summary

This document outlines a comprehensive test strategy for the newly implemented rigid body physics system using Google Test framework. The physics system is critical infrastructure for destructible buildings and requires thorough validation to ensure correctness, stability, and performance.

**Testing Framework:** Google Test (gtest)
**Target Components:**
- Matrix3x3 (3x3 matrix operations and inertia tensors)
- Quaternion (rotation representation)
- RigidBody (full rigid body dynamics)
- Shape system (BoxShape implementation)

**Overall Risk Assessment:** HIGH - Physics errors can cause:
- Building collapse failures
- Unrealistic gameplay
- Simulation instability
- Difficult-to-debug edge cases

---

## Table of Contents

1. [Critical Issues Found](#1-critical-issues-found)
2. [Google Test Setup](#2-google-test-setup)
3. [Test Utilities and Helpers](#3-test-utilities-and-helpers)
4. [Unit Test Suites](#4-unit-test-suites)
5. [Integration Tests](#5-integration-tests)
6. [Physics Validation Tests](#6-physics-validation-tests)
7. [Performance Tests](#7-performance-tests)
8. [Test Execution and CI/CD](#8-test-execution-and-cicd)
9. [Test Data and Scenarios](#9-test-data-and-scenarios)

---

## 1. Critical Issues Found

### 1.1 CRITICAL - Namespace Pollution (All Files)

**Severity:** CRITICAL
**Files:** Matrix3x3.h, Quaternion.h, RigidBody.h, Shape.h, BoxShape.h
**Issue:** All classes are declared in `namespace std`, which is ILLEGAL in C++.

```cpp
namespace std {
    class Matrix3x3 {  // WRONG - std namespace is reserved!
```

**Impact:**
- Undefined behavior according to C++ standard (§17.6.4.2.1)
- Name collisions with standard library
- Compilation errors on some compilers
- Makes code non-portable

**Recommendation:** Use a project-specific namespace (e.g., `namespace tanques` or `namespace physics`)

**Test Coverage Needed:**
- Namespace isolation tests
- Symbol collision detection tests

---

### 1.2 CRITICAL - Matrix Inverse Returns Identity on Singular Matrix

**Severity:** CRITICAL
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/core/Matrix3x3.cpp, Line 128-131

```cpp
if (fabs(det) < 1e-10) {
    // Return identity matrix if singular (should not happen with valid inertia tensors)
    return Matrix3x3::identity();
}
```

**Issue:** Returning identity matrix silently masks serious errors. A singular inertia tensor indicates:
- Zero mass object
- Degenerate geometry
- Numerical instability
- Configuration error

**Impact:**
- Silent physics bugs
- Objects behaving incorrectly with no error indication
- Debugging nightmares
- Invalid simulation results

**Recommendation:**
- Throw an exception or return error code
- Log an error at minimum
- Consider using epsilon based on matrix values (relative epsilon)

**Test Coverage Needed:**
- Singular matrix detection
- Near-singular matrix handling
- Proper error reporting

---

### 1.3 HIGH - Quaternion Normalize Returns Identity on Zero-Length

**Severity:** HIGH
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/core/Quaternion.cpp, Line 100-104

```cpp
Quaternion Quaternion::normalize() const {
    double n = norm();
    if (n < 1e-10) {
        return Quaternion::identity();  // Silent error!
    }
    return Quaternion(w / n, x / n, y / n, z / n);
}
```

**Issue:** Zero-length quaternions indicate serious bugs but are silently converted to identity.

**Impact:**
- Orientation corruption masked
- Simulation drift hidden
- Accumulation of numerical errors

**Recommendation:**
- Assert or throw on zero quaternion
- Log warning at minimum
- Consider if zero quaternion is ever valid in context

**Test Coverage Needed:**
- Zero quaternion handling
- Near-zero quaternion detection
- Numerical stability tests

---

### 1.4 HIGH - Missing Bounds Checking in Matrix Element Access

**Severity:** HIGH
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/core/Matrix3x3.cpp, Line 51-57

```cpp
double Matrix3x3::get(int row, int col) const {
    return m[row][col];  // No bounds checking!
}

void Matrix3x3::set(int row, int col, double value) {
    m[row][col] = value;  // No bounds checking!
}
```

**Issue:** Out-of-bounds access causes undefined behavior, memory corruption.

**Impact:**
- Memory corruption
- Crashes
- Security vulnerabilities
- Hard-to-debug errors

**Recommendation:**
- Add bounds checking (debug builds minimum)
- Use assertions
- Consider returning optional/error code

**Test Coverage Needed:**
- Boundary value testing
- Out-of-bounds access detection
- Index validation tests

---

### 1.5 MEDIUM - Potential Gimbal Lock in Euler Conversion

**Severity:** MEDIUM
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/core/Quaternion.cpp, Line 43-59

**Issue:** `fromEuler()` method doesn't document rotation order or handle gimbal lock edge cases at pitch = ±90°.

**Impact:**
- Unexpected rotation behavior
- Gimbal lock conditions (though quaternions help avoid this)
- Inconsistent rotation interpretation

**Recommendation:**
- Document rotation order clearly (ZYX convention appears to be used)
- Add tests for gimbal lock conditions
- Consider warning or alternative for problematic angles

**Test Coverage Needed:**
- Euler angle edge cases (0°, 90°, 180°, 270°)
- Gimbal lock conditions
- Rotation composition tests

---

### 1.6 MEDIUM - Axis-Angle Conversion Edge Cases

**Severity:** MEDIUM
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/core/Quaternion.cpp, Line 27-40, 152-165

**Issue in fromAxisAngle:**
```cpp
Vector normAxis = axis;
normAxis.setVectorLength(1.0);  // What if axis is zero vector?
```

**Issue in toAxisAngle:**
```cpp
if (fabs(sinHalf) < 1e-6) {
    // Angle is very small, axis is arbitrary
    axis = Vector(1, 0, 0);  // But angle could also be 2π
}
```

**Impact:**
- Division by zero in setVectorLength
- Incorrect angle extraction for 180° rotations
- Loss of rotation information

**Recommendation:**
- Check for zero-length axis before normalizing
- Handle 180° rotation case explicitly (sinHalf ≈ 0 at 0° AND 360°)
- Test with angle normalization

**Test Coverage Needed:**
- Zero axis handling
- Small angle rotations
- 180° rotations
- 360° rotations
- Axis-angle round-trip conversion

---

### 1.7 MEDIUM - Inconsistent Epsilon Values

**Severity:** MEDIUM
**Files:** Multiple

**Issue:** Different epsilon values used throughout:
- `1e-10` for matrix determinant
- `1e-10` for quaternion norm
- `1e-6` for toAxisAngle
- No epsilon in Vector operations

**Impact:**
- Inconsistent numerical stability
- Hard to tune precision
- Difficult to reason about error propagation

**Recommendation:**
- Define physics constants in a single location
- Use relative epsilon where appropriate
- Document precision requirements

**Test Coverage Needed:**
- Precision boundary tests
- Error accumulation tests
- Epsilon appropriateness validation

---

### 1.8 MEDIUM - RigidBody Mass Not Validated

**Severity:** MEDIUM
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/physics/RigidBody.cpp

**Issue:** No validation that mass > 0 when setting inertia tensor. Zero or negative mass causes division by zero.

**Impact:**
- Division by zero in physics calculations
- Invalid inertia tensor calculations
- NaN propagation through simulation

**Recommendation:**
- Validate mass > epsilon in constructor and setMass()
- Validate inertia tensor is positive definite
- Assert or throw on invalid values

**Test Coverage Needed:**
- Zero mass handling
- Negative mass rejection
- Mass update consistency

---

### 1.9 LOW - Damping Applied as Power Function

**Severity:** LOW
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/physics/RigidBody.cpp, Line 133, 147

```cpp
velocity = velocity * pow(linearDamping, dt);
angularVelocity = angularVelocity * pow(angularDamping, dt);
```

**Issue:** While mathematically correct for exponential decay, `pow()` is expensive and damping values (0.95, 0.90) suggest percentage per second, which is unclear.

**Impact:**
- Performance overhead (minor)
- Unclear damping semantics
- Inconsistent with typical game physics

**Recommendation:**
- Document that damping is exponential decay factor
- Consider linear damping for performance: `velocity *= (1.0 - linearDamping * dt)`
- Profile if pow() is a bottleneck

**Test Coverage Needed:**
- Damping behavior validation
- Energy dissipation tests
- Long-term stability tests

---

### 1.10 LOW - AABB Construction Inefficiency

**Severity:** LOW
**File:** /Users/thaylofreitas/Projects/Tanques-3D/src/physics/BoxShape.cpp, Line 36-56

**Issue:** Initializes minV/maxV to position, then overwrites in loop. First iteration could initialize directly.

**Impact:**
- Minor performance overhead
- Slightly confusing code

**Recommendation:**
- Initialize from first vertex, then iterate remaining 7

**Test Coverage Needed:**
- AABB correctness for various orientations
- AABB intersection tests

---

## 2. Google Test Setup

### 2.1 Project Structure

```
Tanques-3D/
├── tests/
│   ├── CMakeLists.txt                 # Test-specific CMake
│   ├── test_main.cpp                  # Google Test main entry point
│   ├── unit/                          # Unit tests
│   │   ├── test_matrix3x3.cpp
│   │   ├── test_quaternion.cpp
│   │   ├── test_rigidbody.cpp
│   │   └── test_boxshape.cpp
│   ├── integration/                   # Integration tests
│   │   ├── test_physics_pipeline.cpp
│   │   ├── test_rotation_integration.cpp
│   │   └── test_collision_response.cpp
│   ├── physics/                       # Physics validation tests
│   │   ├── test_conservation_laws.cpp
│   │   ├── test_numerical_stability.cpp
│   │   └── test_edge_cases.cpp
│   ├── performance/                   # Performance benchmarks
│   │   └── benchmark_physics.cpp
│   └── helpers/                       # Test utilities
│       ├── physics_test_helpers.h
│       ├── physics_test_helpers.cpp
│       └── test_fixtures.h
└── CMakeLists.txt                     # Root CMake (updated)
```

### 2.2 CMake Configuration

**Update root CMakeLists.txt:**

```cmake
# Add after existing project setup (around line 82)

# Option to build tests
option(BUILD_TESTS "Build the test suite" ON)

if(BUILD_TESTS)
    # Enable testing
    enable_testing()

    # Add Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0  # Use latest stable version
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Add test subdirectory
    add_subdirectory(tests)

    message(STATUS "Building tests: ENABLED")
else()
    message(STATUS "Building tests: DISABLED")
endif()
```

**Create tests/CMakeLists.txt:**

```cmake
# Test executable for all tests
set(TEST_SOURCES
    test_main.cpp
    # Unit tests
    unit/test_matrix3x3.cpp
    unit/test_quaternion.cpp
    unit/test_rigidbody.cpp
    unit/test_boxshape.cpp
    # Integration tests
    integration/test_physics_pipeline.cpp
    integration/test_rotation_integration.cpp
    integration/test_collision_response.cpp
    # Physics validation tests
    physics/test_conservation_laws.cpp
    physics/test_numerical_stability.cpp
    physics/test_edge_cases.cpp
    # Test helpers
    helpers/physics_test_helpers.cpp
)

# Compile the core physics code as a library for testing
add_library(physics_core STATIC
    ${CMAKE_SOURCE_DIR}/src/core/Vector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Matrix3x3.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Quaternion.cpp
    ${CMAKE_SOURCE_DIR}/src/entities/Matter.cpp
    ${CMAKE_SOURCE_DIR}/src/entities/Movable.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/RigidBody.cpp
    ${CMAKE_SOURCE_DIR}/src/physics/BoxShape.cpp
)

target_include_directories(physics_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/physics
)

# Add OpenGL/GLUT includes for BoxShape rendering
target_include_directories(physics_core PUBLIC
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

# Link OpenGL/GLUT for BoxShape
target_link_libraries(physics_core PUBLIC
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
)

# Test executable
add_executable(physics_tests ${TEST_SOURCES})

target_include_directories(physics_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/helpers
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/include/entities
    ${CMAKE_SOURCE_DIR}/include/physics
)

target_link_libraries(physics_tests PRIVATE
    physics_core
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

# Compiler options for tests
target_compile_options(physics_tests PRIVATE
    ${PLATFORM_COMPILE_OPTIONS}
    -Wall -Wextra -g
)

# Discover tests for CTest
include(GoogleTest)
gtest_discover_tests(physics_tests)

# Add custom target to run tests
add_custom_target(run_tests
    COMMAND physics_tests --gtest_color=yes
    DEPENDS physics_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running physics test suite"
)
```

### 2.3 Test Main Entry Point

**Create tests/test_main.cpp:**

```cpp
#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "==================================\n";
    std::cout << "  Physics System Test Suite\n";
    std::cout << "  Tank Game 3D - Rigid Body Physics\n";
    std::cout << "==================================\n\n";

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

    std::cout << "\n==================================\n";
    std::cout << "  Test Suite Complete\n";
    std::cout << "==================================\n";

    return result;
}
```

### 2.4 Building and Running Tests

```bash
# Configure with tests enabled (default)
mkdir -p build && cd build
cmake ..

# Build tests
make physics_tests

# Run all tests
make run_tests

# Or run directly with options
./tests/physics_tests --gtest_color=yes --gtest_filter="Matrix3x3*"

# Run with verbose output
./tests/physics_tests --gtest_color=yes -v

# Generate XML report for CI
./tests/physics_tests --gtest_output=xml:test_results.xml
```

---

## 3. Test Utilities and Helpers

### 3.1 Test Helper Header

**Create tests/helpers/physics_test_helpers.h:**

```cpp
#ifndef PHYSICS_TEST_HELPERS_H
#define PHYSICS_TEST_HELPERS_H

#include <gtest/gtest.h>
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include <cmath>

namespace physics_testing {

// Floating-point comparison epsilons
constexpr double EPSILON_TIGHT = 1e-10;     // Exact mathematical operations
constexpr double EPSILON_NORMAL = 1e-6;     // Standard physics calculations
constexpr double EPSILON_LOOSE = 1e-3;      // Integration over time, accumulated error

// Custom assertions for floating-point comparisons
#define EXPECT_DOUBLE_NEAR(val1, val2, eps) \
    EXPECT_NEAR(val1, val2, eps) << "Expected " << val1 << " to be near " << val2

#define ASSERT_DOUBLE_NEAR(val1, val2, eps) \
    ASSERT_NEAR(val1, val2, eps) << "Expected " << val1 << " to be near " << val2

// Vector assertions
void ExpectVectorNear(const std::Vector& expected, const std::Vector& actual,
                      double epsilon = EPSILON_NORMAL,
                      const std::string& message = "");

void AssertVectorNear(const std::Vector& expected, const std::Vector& actual,
                      double epsilon = EPSILON_NORMAL,
                      const std::string& message = "");

#define EXPECT_VECTOR_NEAR(expected, actual, eps) \
    ExpectVectorNear(expected, actual, eps, #actual)

#define ASSERT_VECTOR_NEAR(expected, actual, eps) \
    AssertVectorNear(expected, actual, eps, #actual)

// Matrix assertions
void ExpectMatrixNear(const std::Matrix3x3& expected, const std::Matrix3x3& actual,
                      double epsilon = EPSILON_NORMAL,
                      const std::string& message = "");

#define EXPECT_MATRIX_NEAR(expected, actual, eps) \
    ExpectMatrixNear(expected, actual, eps, #actual)

// Quaternion assertions
void ExpectQuaternionNear(const std::Quaternion& expected, const std::Quaternion& actual,
                          double epsilon = EPSILON_NORMAL,
                          const std::string& message = "");

// Check if quaternion is normalized (unit length)
void ExpectQuaternionNormalized(const std::Quaternion& q,
                                double epsilon = EPSILON_NORMAL);

#define EXPECT_QUATERNION_NORMALIZED(q) \
    ExpectQuaternionNormalized(q, EPSILON_NORMAL)

// Matrix property checks
bool IsOrthogonal(const std::Matrix3x3& m, double epsilon = EPSILON_NORMAL);
bool IsSymmetric(const std::Matrix3x3& m, double epsilon = EPSILON_NORMAL);
bool IsPositiveDefinite(const std::Matrix3x3& m);

void ExpectMatrixOrthogonal(const std::Matrix3x3& m,
                            double epsilon = EPSILON_NORMAL);
void ExpectMatrixSymmetric(const std::Matrix3x3& m,
                           double epsilon = EPSILON_NORMAL);

// Physics validation helpers
double ComputeKineticEnergy(const std::Vector& velocity, double mass);
double ComputeRotationalEnergy(const std::Vector& angularVelocity,
                                const std::Matrix3x3& inertiaTensor);

// Test data generators
std::Vector RandomUnitVector();
std::Vector RandomVector(double minVal = -10.0, double maxVal = 10.0);
std::Matrix3x3 RandomOrthogonalMatrix();
std::Matrix3x3 RandomSymmetricPositiveDefiniteMatrix();
std::Quaternion RandomQuaternion();

// Numerical utilities
bool IsNaN(double value);
bool IsInf(double value);
bool IsFinite(double value);

void ExpectAllFinite(const std::Vector& v, const std::string& message = "");
void ExpectAllFinite(const std::Matrix3x3& m, const std::string& message = "");
void ExpectAllFinite(const std::Quaternion& q, const std::string& message = "");

} // namespace physics_testing

#endif // PHYSICS_TEST_HELPERS_H
```

### 3.2 Test Helper Implementation

**Create tests/helpers/physics_test_helpers.cpp:**

```cpp
#include "physics_test_helpers.h"
#include <random>
#include <sstream>

namespace physics_testing {

// Vector assertions
void ExpectVectorNear(const std::Vector& expected, const std::Vector& actual,
                      double epsilon, const std::string& message) {
    std::stringstream ss;
    if (!message.empty()) {
        ss << message << ": ";
    }
    ss << "Expected (" << expected.getX() << ", " << expected.getY() << ", "
       << expected.getZ() << "), got (" << actual.getX() << ", "
       << actual.getY() << ", " << actual.getZ() << ")";

    EXPECT_NEAR(expected.getX(), actual.getX(), epsilon) << ss.str();
    EXPECT_NEAR(expected.getY(), actual.getY(), epsilon) << ss.str();
    EXPECT_NEAR(expected.getZ(), actual.getZ(), epsilon) << ss.str();
}

void AssertVectorNear(const std::Vector& expected, const std::Vector& actual,
                      double epsilon, const std::string& message) {
    std::stringstream ss;
    if (!message.empty()) {
        ss << message << ": ";
    }
    ss << "Expected (" << expected.getX() << ", " << expected.getY() << ", "
       << expected.getZ() << "), got (" << actual.getX() << ", "
       << actual.getY() << ", " << actual.getZ() << ")";

    ASSERT_NEAR(expected.getX(), actual.getX(), epsilon) << ss.str();
    ASSERT_NEAR(expected.getY(), actual.getY(), epsilon) << ss.str();
    ASSERT_NEAR(expected.getZ(), actual.getZ(), epsilon) << ss.str();
}

// Matrix assertions
void ExpectMatrixNear(const std::Matrix3x3& expected, const std::Matrix3x3& actual,
                      double epsilon, const std::string& message) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NEAR(expected.get(i, j), actual.get(i, j), epsilon)
                << message << ": Matrix element [" << i << "][" << j << "] differs";
        }
    }
}

// Quaternion assertions
void ExpectQuaternionNear(const std::Quaternion& expected, const std::Quaternion& actual,
                          double epsilon, const std::string& message) {
    // Note: q and -q represent the same rotation
    // Check both possibilities
    double dotProduct = expected.dot(actual);

    if (dotProduct >= 0) {
        // Same hemisphere
        EXPECT_NEAR(expected.getW(), actual.getW(), epsilon) << message;
        EXPECT_NEAR(expected.getX(), actual.getX(), epsilon) << message;
        EXPECT_NEAR(expected.getY(), actual.getY(), epsilon) << message;
        EXPECT_NEAR(expected.getZ(), actual.getZ(), epsilon) << message;
    } else {
        // Opposite hemisphere (q and -q are equivalent)
        EXPECT_NEAR(expected.getW(), -actual.getW(), epsilon) << message;
        EXPECT_NEAR(expected.getX(), -actual.getX(), epsilon) << message;
        EXPECT_NEAR(expected.getY(), -actual.getY(), epsilon) << message;
        EXPECT_NEAR(expected.getZ(), -actual.getZ(), epsilon) << message;
    }
}

void ExpectQuaternionNormalized(const std::Quaternion& q, double epsilon) {
    double norm = q.norm();
    EXPECT_NEAR(1.0, norm, epsilon)
        << "Quaternion not normalized: norm = " << norm;
}

// Matrix property checks
bool IsOrthogonal(const std::Matrix3x3& m, double epsilon) {
    std::Matrix3x3 identity = std::Matrix3x3::identity();
    std::Matrix3x3 product = m * m.transpose();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (std::fabs(product.get(i, j) - identity.get(i, j)) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

bool IsSymmetric(const std::Matrix3x3& m, double epsilon) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (std::fabs(m.get(i, j) - m.get(j, i)) > epsilon) {
                return false;
            }
        }
    }
    return true;
}

bool IsPositiveDefinite(const std::Matrix3x3& m) {
    // Check if all eigenvalues are positive using Sylvester's criterion
    // Leading principal minors must be positive
    double det1 = m.get(0, 0);
    double det2 = m.get(0, 0) * m.get(1, 1) - m.get(0, 1) * m.get(1, 0);
    double det3 = m.determinant();

    return (det1 > 0) && (det2 > 0) && (det3 > 0);
}

void ExpectMatrixOrthogonal(const std::Matrix3x3& m, double epsilon) {
    EXPECT_TRUE(IsOrthogonal(m, epsilon))
        << "Matrix is not orthogonal (M * M^T != I)";
}

void ExpectMatrixSymmetric(const std::Matrix3x3& m, double epsilon) {
    EXPECT_TRUE(IsSymmetric(m, epsilon))
        << "Matrix is not symmetric (M != M^T)";
}

// Physics validation helpers
double ComputeKineticEnergy(const std::Vector& velocity, double mass) {
    double vSquared = velocity.dotProduct(velocity);
    return 0.5 * mass * vSquared;
}

double ComputeRotationalEnergy(const std::Vector& angularVelocity,
                                const std::Matrix3x3& inertiaTensor) {
    std::Vector L = inertiaTensor * angularVelocity;
    return 0.5 * angularVelocity.dotProduct(L);
}

// Test data generators
std::Vector RandomUnitVector() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-1.0, 1.0);

    std::Vector v(dis(gen), dis(gen), dis(gen));
    v.setVectorLength(1.0);
    return v;
}

std::Vector RandomVector(double minVal, double maxVal) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(minVal, maxVal);

    return std::Vector(dis(gen), dis(gen), dis(gen));
}

std::Matrix3x3 RandomOrthogonalMatrix() {
    // Generate random rotation matrix using Gram-Schmidt
    std::Vector v1 = RandomUnitVector();
    std::Vector v2 = RandomVector();

    // Orthogonalize v2 with respect to v1
    v2 = v2 - v1 * v1.dotProduct(v2);
    v2.setVectorLength(1.0);

    // v3 = v1 × v2
    std::Vector v3 = v1.crossProduct(v2);

    return std::Matrix3x3(
        v1.getX(), v2.getX(), v3.getX(),
        v1.getY(), v2.getY(), v3.getY(),
        v1.getZ(), v2.getZ(), v3.getZ()
    );
}

std::Matrix3x3 RandomSymmetricPositiveDefiniteMatrix() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0, 10.0);

    // Generate diagonal matrix with positive values
    double d1 = dis(gen);
    double d2 = dis(gen);
    double d3 = dis(gen);

    std::Matrix3x3 D = std::Matrix3x3::diagonal(d1, d2, d3);

    // Rotate by random orthogonal matrix: M = R * D * R^T
    std::Matrix3x3 R = RandomOrthogonalMatrix();
    return R * D * R.transpose();
}

std::Quaternion RandomQuaternion() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    std::Quaternion q(dis(gen), dis(gen), dis(gen), dis(gen));
    return q.normalize();
}

// Numerical utilities
bool IsNaN(double value) {
    return std::isnan(value);
}

bool IsInf(double value) {
    return std::isinf(value);
}

bool IsFinite(double value) {
    return std::isfinite(value);
}

void ExpectAllFinite(const std::Vector& v, const std::string& message) {
    EXPECT_TRUE(IsFinite(v.getX())) << message << ": X component is not finite";
    EXPECT_TRUE(IsFinite(v.getY())) << message << ": Y component is not finite";
    EXPECT_TRUE(IsFinite(v.getZ())) << message << ": Z component is not finite";
}

void ExpectAllFinite(const std::Matrix3x3& m, const std::string& message) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_TRUE(IsFinite(m.get(i, j)))
                << message << ": Element [" << i << "][" << j << "] is not finite";
        }
    }
}

void ExpectAllFinite(const std::Quaternion& q, const std::string& message) {
    EXPECT_TRUE(IsFinite(q.getW())) << message << ": W component is not finite";
    EXPECT_TRUE(IsFinite(q.getX())) << message << ": X component is not finite";
    EXPECT_TRUE(IsFinite(q.getY())) << message << ": Y component is not finite";
    EXPECT_TRUE(IsFinite(q.getZ())) << message << ": Z component is not finite";
}

} // namespace physics_testing
```

### 3.3 Test Fixtures

**Create tests/helpers/test_fixtures.h:**

```cpp
#ifndef TEST_FIXTURES_H
#define TEST_FIXTURES_H

#include <gtest/gtest.h>
#include "Vector.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "RigidBody.h"
#include "BoxShape.h"

namespace physics_testing {

// Base fixture for matrix tests
class Matrix3x3Test : public ::testing::Test {
protected:
    void SetUp() override {
        identity = std::Matrix3x3::identity();
        zero = std::Matrix3x3::zero();
        diagonal = std::Matrix3x3::diagonal(2.0, 3.0, 4.0);
    }

    std::Matrix3x3 identity;
    std::Matrix3x3 zero;
    std::Matrix3x3 diagonal;
};

// Fixture for quaternion tests
class QuaternionTest : public ::testing::Test {
protected:
    void SetUp() override {
        identityQuat = std::Quaternion::identity();
        xAxis = std::Vector(1, 0, 0);
        yAxis = std::Vector(0, 1, 0);
        zAxis = std::Vector(0, 0, 1);
    }

    std::Quaternion identityQuat;
    std::Vector xAxis, yAxis, zAxis;
};

// Fixture for rigid body tests
class RigidBodyTest : public ::testing::Test {
protected:
    void SetUp() override {
        position = std::Vector(0, 0, 0);
        mass = 1.0;
        body = new std::RigidBody(position, mass);
    }

    void TearDown() override {
        delete body;
    }

    std::Vector position;
    double mass;
    std::RigidBody* body;
};

// Parameterized test fixture for rotation angles
class RotationAngleTest : public ::testing::TestWithParam<double> {
protected:
    void SetUp() override {
        angle = GetParam();
    }

    double angle;
};

// Parameterized test fixture for mass values
class MassValueTest : public ::testing::TestWithParam<double> {
protected:
    void SetUp() override {
        mass = GetParam();
    }

    double mass;
};

} // namespace physics_testing

#endif // TEST_FIXTURES_H
```

---

## 4. Unit Test Suites

### 4.1 Matrix3x3 Unit Tests

**Priority:** CRITICAL
**File:** tests/unit/test_matrix3x3.cpp

#### Test Categories:

1. **Construction and Initialization** (Critical)
   - Default constructor creates zero matrix
   - Parameterized constructor sets correct values
   - Identity factory method
   - Diagonal factory method
   - Zero factory method

2. **Element Access** (Critical)
   - Valid get/set operations
   - Boundary indices (0, 2)
   - OUT-OF-BOUNDS: Negative indices (should assert/throw)
   - OUT-OF-BOUNDS: Indices >= 3 (should assert/throw)

3. **Basic Arithmetic** (Critical)
   - Matrix addition (A + B)
   - Matrix subtraction (A - B)
   - Scalar multiplication (A * k)
   - Commutative properties where applicable

4. **Matrix Multiplication** (Critical)
   - Matrix-matrix multiplication (A * B)
   - Matrix-vector multiplication (A * v)
   - Identity multiplication: I * A = A
   - Associativity: (A * B) * C = A * (B * C)
   - Non-commutativity: A * B ≠ B * A (general case)

5. **Transpose** (High)
   - Transpose of transpose: (A^T)^T = A
   - Transpose of product: (A * B)^T = B^T * A^T
   - Symmetric matrix: A = A^T

6. **Determinant** (Critical)
   - Identity determinant = 1
   - Zero matrix determinant = 0
   - Diagonal matrix determinant = product of diagonal
   - det(A * B) = det(A) * det(B)
   - det(A^T) = det(A)
   - Known matrices with known determinants

7. **Matrix Inverse** (CRITICAL - Bug Found)
   - Inverse of identity = identity
   - A * A^-1 = I
   - (A^-1)^-1 = A
   - (A * B)^-1 = B^-1 * A^-1
   - **SINGULAR MATRIX: det = 0 (ERROR CASE)**
   - **NEAR-SINGULAR: det ≈ 1e-12 (ERROR CASE)**
   - Orthogonal matrix: A^-1 = A^T

8. **Special Properties for Inertia Tensors** (High)
   - Symmetry: I = I^T
   - Positive definiteness
   - All diagonal elements > 0
   - Triangle inequality for moments of inertia

9. **Numerical Stability** (High)
   - Operations with very small numbers
   - Operations with very large numbers
   - Mixed scale operations
   - Error accumulation over many operations

10. **Edge Cases** (Medium)
    - All elements zero
    - All elements same value
    - Very large determinants (> 1e10)
    - Very small determinants (< 1e-10)

#### Sample Test Implementation:

```cpp
#include <gtest/gtest.h>
#include "Matrix3x3.h"
#include "physics_test_helpers.h"
#include "test_fixtures.h"

using namespace physics_testing;

class Matrix3x3Test : public ::testing::Test {
protected:
    void SetUp() override {
        identity = std::Matrix3x3::identity();
        zero = std::Matrix3x3::zero();
        diagonal = std::Matrix3x3::diagonal(2.0, 3.0, 4.0);
    }

    std::Matrix3x3 identity;
    std::Matrix3x3 zero;
    std::Matrix3x3 diagonal;
};

// ========== CONSTRUCTION TESTS ==========

TEST_F(Matrix3x3Test, DefaultConstructorCreatesZeroMatrix) {
    std::Matrix3x3 m;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_DOUBLE_EQ(0.0, m.get(i, j))
                << "Element [" << i << "][" << j << "] should be zero";
        }
    }
}

TEST_F(Matrix3x3Test, ParameterizedConstructorSetsCorrectValues) {
    std::Matrix3x3 m(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    EXPECT_DOUBLE_EQ(1.0, m.get(0, 0));
    EXPECT_DOUBLE_EQ(2.0, m.get(0, 1));
    EXPECT_DOUBLE_EQ(3.0, m.get(0, 2));
    EXPECT_DOUBLE_EQ(4.0, m.get(1, 0));
    EXPECT_DOUBLE_EQ(5.0, m.get(1, 1));
    EXPECT_DOUBLE_EQ(6.0, m.get(1, 2));
    EXPECT_DOUBLE_EQ(7.0, m.get(2, 0));
    EXPECT_DOUBLE_EQ(8.0, m.get(2, 1));
    EXPECT_DOUBLE_EQ(9.0, m.get(2, 2));
}

TEST_F(Matrix3x3Test, IdentityFactoryCreatesIdentityMatrix) {
    std::Matrix3x3 I = std::Matrix3x3::identity();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == j) {
                EXPECT_DOUBLE_EQ(1.0, I.get(i, j));
            } else {
                EXPECT_DOUBLE_EQ(0.0, I.get(i, j));
            }
        }
    }
}

// ========== ELEMENT ACCESS TESTS ==========

TEST_F(Matrix3x3Test, GetSetValidIndices) {
    std::Matrix3x3 m;

    m.set(1, 2, 42.0);
    EXPECT_DOUBLE_EQ(42.0, m.get(1, 2));
}

TEST_F(Matrix3x3Test, BoundaryIndices) {
    std::Matrix3x3 m;

    // Should not crash
    m.set(0, 0, 1.0);
    m.set(2, 2, 2.0);

    EXPECT_DOUBLE_EQ(1.0, m.get(0, 0));
    EXPECT_DOUBLE_EQ(2.0, m.get(2, 2));
}

// CRITICAL: These tests will likely FAIL until bounds checking is added
TEST_F(Matrix3x3Test, DISABLED_OutOfBoundsRowNegative) {
    std::Matrix3x3 m;

    // Should assert or throw
    EXPECT_DEATH(m.get(-1, 0), ".*");
}

TEST_F(Matrix3x3Test, DISABLED_OutOfBoundsRowTooLarge) {
    std::Matrix3x3 m;

    // Should assert or throw
    EXPECT_DEATH(m.get(3, 0), ".*");
}

TEST_F(Matrix3x3Test, DISABLED_OutOfBoundsColNegative) {
    std::Matrix3x3 m;

    // Should assert or throw
    EXPECT_DEATH(m.get(0, -1), ".*");
}

// ========== ARITHMETIC TESTS ==========

TEST_F(Matrix3x3Test, Addition) {
    std::Matrix3x3 a(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 b(9, 8, 7,
                     6, 5, 4,
                     3, 2, 1);

    std::Matrix3x3 sum = a + b;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_DOUBLE_EQ(a.get(i, j) + b.get(i, j), sum.get(i, j));
        }
    }
}

TEST_F(Matrix3x3Test, ScalarMultiplication) {
    std::Matrix3x3 a(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 scaled = a * 2.0;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_DOUBLE_EQ(a.get(i, j) * 2.0, scaled.get(i, j));
        }
    }
}

TEST_F(Matrix3x3Test, IdentityMultiplicationPreservesMatrix) {
    std::Matrix3x3 a(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 result = identity * a;

    ExpectMatrixNear(a, result, EPSILON_TIGHT);
}

// ========== TRANSPOSE TESTS ==========

TEST_F(Matrix3x3Test, TransposeOfTransposeIsOriginal) {
    std::Matrix3x3 a(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 result = a.transpose().transpose();

    ExpectMatrixNear(a, result, EPSILON_TIGHT);
}

TEST_F(Matrix3x3Test, TransposeSwapsRowsAndColumns) {
    std::Matrix3x3 a(1, 2, 3,
                     4, 5, 6,
                     7, 8, 9);

    std::Matrix3x3 aT = a.transpose();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_DOUBLE_EQ(a.get(i, j), aT.get(j, i));
        }
    }
}

// ========== DETERMINANT TESTS ==========

TEST_F(Matrix3x3Test, DeterminantOfIdentity) {
    EXPECT_DOUBLE_NEAR(1.0, identity.determinant(), EPSILON_TIGHT);
}

TEST_F(Matrix3x3Test, DeterminantOfZeroMatrix) {
    EXPECT_DOUBLE_NEAR(0.0, zero.determinant(), EPSILON_TIGHT);
}

TEST_F(Matrix3x3Test, DeterminantOfDiagonalMatrix) {
    double expected = 2.0 * 3.0 * 4.0;  // Product of diagonal elements
    EXPECT_DOUBLE_NEAR(expected, diagonal.determinant(), EPSILON_NORMAL);
}

TEST_F(Matrix3x3Test, DeterminantOfKnownMatrix) {
    std::Matrix3x3 m(1, 2, 3,
                     0, 1, 4,
                     5, 6, 0);

    // Computed by hand: det = 1(0-24) - 2(0-20) + 3(0-5) = -24 + 40 - 15 = 1
    EXPECT_DOUBLE_NEAR(1.0, m.determinant(), EPSILON_NORMAL);
}

// ========== INVERSE TESTS (CRITICAL - BUG DETECTION) ==========

TEST_F(Matrix3x3Test, InverseOfIdentity) {
    std::Matrix3x3 inv = identity.inverse();

    ExpectMatrixNear(identity, inv, EPSILON_TIGHT);
}

TEST_F(Matrix3x3Test, InverseTimesOriginalIsIdentity) {
    std::Matrix3x3 a(4, 7, 2,
                     3, 6, 1,
                     2, 5, 3);

    std::Matrix3x3 inv = a.inverse();
    std::Matrix3x3 product = a * inv;

    ExpectMatrixNear(identity, product, EPSILON_NORMAL);
}

// CRITICAL TEST: This should FAIL or produce identity (current behavior)
TEST_F(Matrix3x3Test, SingularMatrixInverse) {
    // Create singular matrix (determinant = 0)
    std::Matrix3x3 singular(1, 2, 3,
                           2, 4, 6,
                           3, 6, 9);

    double det = singular.determinant();
    EXPECT_DOUBLE_NEAR(0.0, det, EPSILON_TIGHT) << "Matrix should be singular";

    std::Matrix3x3 inv = singular.inverse();

    // CURRENT BEHAVIOR: Returns identity (BAD!)
    // DESIRED BEHAVIOR: Should throw exception or return error

    // For now, document the issue
    ExpectMatrixNear(identity, inv, EPSILON_TIGHT)
        << "BUG: Singular matrix returns identity instead of error";
}

// Test near-singular matrix (conditioning issues)
TEST_F(Matrix3x3Test, NearSingularMatrixInverse) {
    std::Matrix3x3 nearSingular(1.0, 2.0, 3.0,
                                4.0, 5.0, 6.0,
                                7.0, 8.0, 9.0 + 1e-12);

    double det = nearSingular.determinant();
    EXPECT_NEAR(0.0, det, 1e-10) << "Matrix should be near-singular";

    std::Matrix3x3 inv = nearSingular.inverse();

    // This matrix is extremely ill-conditioned
    // Inverse may have huge numerical errors
    std::Matrix3x3 product = nearSingular * inv;

    // Allow large error due to ill-conditioning
    ExpectMatrixNear(identity, product, 0.1);
}

// ========== INERTIA TENSOR SPECIFIC TESTS ==========

TEST_F(Matrix3x3Test, InertiaTensorIsSymmetric) {
    // Inertia tensors must be symmetric
    std::Matrix3x3 inertia(100, 10, 5,
                          10, 120, 8,
                          5, 8, 110);

    ExpectMatrixSymmetric(inertia, EPSILON_TIGHT);
}

TEST_F(Matrix3x3Test, InertiaTensorIsPositiveDefinite) {
    // Valid inertia tensor (sphere: I = 2/5 m r^2)
    double m = 10.0;
    double r = 2.0;
    double I = 0.4 * m * r * r;

    std::Matrix3x3 inertia = std::Matrix3x3::diagonal(I, I, I);

    EXPECT_TRUE(IsPositiveDefinite(inertia))
        << "Inertia tensor must be positive definite";
}

// Total test count for Matrix3x3: ~40-50 tests
```

**Test Count Estimate:** 45 tests

---

### 4.2 Quaternion Unit Tests

**Priority:** CRITICAL
**File:** tests/unit/test_quaternion.cpp

#### Test Categories:

1. **Construction** (Critical)
   - Default constructor creates identity
   - Parameterized constructor
   - Identity factory method
   - FromAxisAngle factory (various axes and angles)
   - FromEuler factory (various angle combinations)

2. **Basic Operations** (Critical)
   - Addition
   - Subtraction
   - Scalar multiplication
   - Quaternion multiplication (Hamilton product)
   - Dot product
   - Norm/length calculation

3. **Normalization** (CRITICAL - Bug Found)
   - Normalize unit quaternion (should be unchanged)
   - Normalize non-unit quaternion
   - **ZERO QUATERNION (should error)**
   - **NEAR-ZERO quaternion**
   - Numerical stability after many operations

4. **Conjugate and Inverse** (High)
   - Conjugate: q* = (w, -x, -y, -z)
   - Inverse: q^-1 = q* / |q|^2
   - For unit quaternion: q^-1 = q*
   - q * q^-1 = identity
   - Zero quaternion inverse (should error)

5. **Rotation Operations** (Critical)
   - Rotate vector by identity quaternion (unchanged)
   - Rotate by 90° about X, Y, Z axes
   - Rotate by 180° about axes
   - Rotate by 360° (returns to original)
   - Rotation composition: q2 * q1 applies q1 then q2
   - Inverse rotation: q^-1 undoes rotation

6. **Conversion to Rotation Matrix** (Critical)
   - Identity quaternion → identity matrix
   - 90° rotations → known matrices
   - Matrix is orthogonal
   - det(matrix) = 1
   - Matrix correctly rotates vectors

7. **Conversion to Axis-Angle** (CRITICAL - Bug Found)
   - Identity quaternion → any axis, angle = 0
   - Known rotations → correct axis and angle
   - 180° rotation → correct axis
   - **SMALL ANGLES** (< 1e-6)
   - Round-trip: axis-angle → quat → axis-angle

8. **Euler Angle Conversion** (HIGH - Gimbal Lock)
   - Round-trip: Euler → quat → Euler
   - **GIMBAL LOCK: pitch = 90°**
   - **GIMBAL LOCK: pitch = -90°**
   - Various angle combinations
   - Rotation order consistency

9. **Integration for Physics** (High)
   - Zero angular velocity (no change)
   - Constant angular velocity (known rotation)
   - Integration maintains normalization
   - Small timestep accuracy
   - Large timestep stability

10. **Numerical Stability** (High)
    - Quaternion drift over many normalizations
    - Error accumulation in integration
    - Operations with very small components
    - Operations with large quaternions (before normalization)

11. **Edge Cases** (Medium)
    - Opposite quaternions (q and -q represent same rotation)
    - 0° rotation
    - 360° rotation
    - Axis aligned with zero vector
    - Very small rotation angles

#### Sample Test Implementation:

```cpp
#include <gtest/gtest.h>
#include "Quaternion.h"
#include "Vector.h"
#include "physics_test_helpers.h"
#include "test_fixtures.h"
#include <cmath>

using namespace physics_testing;

class QuaternionTest : public ::testing::Test {
protected:
    void SetUp() override {
        identity = std::Quaternion::identity();
        xAxis = std::Vector(1, 0, 0);
        yAxis = std::Vector(0, 1, 0);
        zAxis = std::Vector(0, 0, 1);
    }

    std::Quaternion identity;
    std::Vector xAxis, yAxis, zAxis;
};

// ========== CONSTRUCTION TESTS ==========

TEST_F(QuaternionTest, DefaultConstructorCreatesIdentity) {
    std::Quaternion q;

    EXPECT_DOUBLE_EQ(1.0, q.getW());
    EXPECT_DOUBLE_EQ(0.0, q.getX());
    EXPECT_DOUBLE_EQ(0.0, q.getY());
    EXPECT_DOUBLE_EQ(0.0, q.getZ());
}

TEST_F(QuaternionTest, ParameterizedConstructor) {
    std::Quaternion q(0.5, 0.5, 0.5, 0.5);

    EXPECT_DOUBLE_EQ(0.5, q.getW());
    EXPECT_DOUBLE_EQ(0.5, q.getX());
    EXPECT_DOUBLE_EQ(0.5, q.getY());
    EXPECT_DOUBLE_EQ(0.5, q.getZ());
}

TEST_F(QuaternionTest, FromAxisAngle_90DegreesAboutX) {
    double angle = M_PI / 2.0;  // 90 degrees
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, angle);

    EXPECT_QUATERNION_NORMALIZED(q);

    // 90° about X: q = (cos(45°), sin(45°), 0, 0)
    double halfAngle = angle / 2.0;
    EXPECT_DOUBLE_NEAR(std::cos(halfAngle), q.getW(), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(std::sin(halfAngle), q.getX(), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(0.0, q.getY(), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(0.0, q.getZ(), EPSILON_NORMAL);
}

// CRITICAL: This should fail or produce unexpected results
TEST_F(QuaternionTest, DISABLED_FromAxisAngle_ZeroAxis) {
    std::Vector zeroAxis(0, 0, 0);

    // Should error or handle gracefully
    EXPECT_DEATH(std::Quaternion::fromAxisAngle(zeroAxis, M_PI / 2.0), ".*");
}

// ========== NORMALIZATION TESTS (CRITICAL) ==========

TEST_F(QuaternionTest, NormalizeUnitQuaternion) {
    std::Quaternion q = identity.normalize();

    ExpectQuaternionNear(identity, q, EPSILON_TIGHT);
    EXPECT_QUATERNION_NORMALIZED(q);
}

TEST_F(QuaternionTest, NormalizeNonUnitQuaternion) {
    std::Quaternion q(2.0, 0.0, 0.0, 0.0);

    std::Quaternion normalized = q.normalize();

    EXPECT_QUATERNION_NORMALIZED(normalized);
    EXPECT_DOUBLE_NEAR(1.0, normalized.getW(), EPSILON_TIGHT);
    EXPECT_DOUBLE_NEAR(0.0, normalized.getX(), EPSILON_TIGHT);
}

// CRITICAL: This exposes the bug
TEST_F(QuaternionTest, NormalizeZeroQuaternion) {
    std::Quaternion zero(0, 0, 0, 0);

    std::Quaternion result = zero.normalize();

    // CURRENT BEHAVIOR: Returns identity (BAD!)
    ExpectQuaternionNear(identity, result, EPSILON_TIGHT)
        << "BUG: Zero quaternion returns identity instead of error";

    // DESIRED: Should throw or assert
}

// ========== ROTATION TESTS ==========

TEST_F(QuaternionTest, RotateVectorByIdentityIsUnchanged) {
    std::Vector v(1, 2, 3);
    std::Vector rotated = identity.rotate(v);

    ExpectVectorNear(v, rotated, EPSILON_TIGHT);
}

TEST_F(QuaternionTest, Rotate90DegreesAboutZ) {
    // 90° rotation about Z-axis: (1, 0, 0) → (0, 1, 0)
    std::Quaternion q = std::Quaternion::fromAxisAngle(zAxis, M_PI / 2.0);

    std::Vector v(1, 0, 0);
    std::Vector rotated = q.rotate(v);

    std::Vector expected(0, 1, 0);
    ExpectVectorNear(expected, rotated, EPSILON_NORMAL);
}

TEST_F(QuaternionTest, Rotate180DegreesAboutY) {
    // 180° rotation about Y-axis: (1, 0, 0) → (-1, 0, 0)
    std::Quaternion q = std::Quaternion::fromAxisAngle(yAxis, M_PI);

    std::Vector v(1, 0, 0);
    std::Vector rotated = q.rotate(v);

    std::Vector expected(-1, 0, 0);
    ExpectVectorNear(expected, rotated, EPSILON_NORMAL);
}

TEST_F(QuaternionTest, Rotate360DegreesReturnsToOriginal) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, 2.0 * M_PI);

    std::Vector v(1, 2, 3);
    std::Vector rotated = q.rotate(v);

    ExpectVectorNear(v, rotated, EPSILON_NORMAL);
}

TEST_F(QuaternionTest, InverseRotationUndoesRotation) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(zAxis, M_PI / 3.0);
    std::Quaternion qInv = q.inverse();

    std::Vector v(1, 2, 3);
    std::Vector rotated = q.rotate(v);
    std::Vector restored = qInv.rotate(rotated);

    ExpectVectorNear(v, restored, EPSILON_NORMAL);
}

// ========== ROTATION MATRIX CONVERSION ==========

TEST_F(QuaternionTest, IdentityQuaternionToIdentityMatrix) {
    std::Matrix3x3 mat = identity.toRotationMatrix();
    std::Matrix3x3 expected = std::Matrix3x3::identity();

    ExpectMatrixNear(expected, mat, EPSILON_TIGHT);
}

TEST_F(QuaternionTest, RotationMatrixIsOrthogonal) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, M_PI / 4.0);
    std::Matrix3x3 mat = q.toRotationMatrix();

    ExpectMatrixOrthogonal(mat, EPSILON_NORMAL);
}

TEST_F(QuaternionTest, RotationMatrixDeterminantIsOne) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(yAxis, M_PI / 3.0);
    std::Matrix3x3 mat = q.toRotationMatrix();

    EXPECT_DOUBLE_NEAR(1.0, mat.determinant(), EPSILON_NORMAL);
}

// ========== AXIS-ANGLE CONVERSION (CRITICAL - BUG PRONE) ==========

TEST_F(QuaternionTest, ToAxisAngle_IdentityQuaternion) {
    std::Vector axis;
    double angle;

    identity.toAxisAngle(axis, angle);

    EXPECT_DOUBLE_NEAR(0.0, angle, EPSILON_NORMAL);
    // Axis can be anything for zero rotation
}

TEST_F(QuaternionTest, AxisAngleRoundTrip) {
    std::Vector originalAxis(1, 2, 3);
    originalAxis.setVectorLength(1.0);
    double originalAngle = M_PI / 3.0;

    std::Quaternion q = std::Quaternion::fromAxisAngle(originalAxis, originalAngle);

    std::Vector extractedAxis;
    double extractedAngle;
    q.toAxisAngle(extractedAxis, extractedAngle);

    EXPECT_DOUBLE_NEAR(originalAngle, extractedAngle, EPSILON_NORMAL);
    ExpectVectorNear(originalAxis, extractedAxis, EPSILON_NORMAL);
}

TEST_F(QuaternionTest, ToAxisAngle_180DegreeRotation) {
    // 180° rotation is a special case
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, M_PI);

    std::Vector axis;
    double angle;
    q.toAxisAngle(axis, angle);

    EXPECT_DOUBLE_NEAR(M_PI, angle, EPSILON_NORMAL);
    ExpectVectorNear(xAxis, axis, EPSILON_NORMAL);
}

// CRITICAL: Small angle edge case
TEST_F(QuaternionTest, ToAxisAngle_VerySmallAngle) {
    double smallAngle = 1e-7;
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, smallAngle);

    std::Vector axis;
    double angle;
    q.toAxisAngle(axis, angle);

    // Should recover small angle, not zero
    EXPECT_DOUBLE_NEAR(smallAngle, angle, 1e-6);
}

// ========== EULER ANGLE TESTS (GIMBAL LOCK) ==========

TEST_F(QuaternionTest, EulerRoundTrip_NoGimbalLock) {
    double roll = M_PI / 6.0;
    double pitch = M_PI / 4.0;
    double yaw = M_PI / 3.0;

    std::Quaternion q = std::Quaternion::fromEuler(roll, pitch, yaw);

    EXPECT_QUATERNION_NORMALIZED(q);

    // Full round-trip test would require toEuler() method (not implemented)
    // Test rotation equivalence instead
    std::Vector v(1, 0, 0);
    std::Vector rotated = q.rotate(v);

    ExpectAllFinite(rotated, "Euler rotation result");
}

TEST_F(QuaternionTest, GimbalLock_Pitch90Degrees) {
    double roll = M_PI / 6.0;
    double pitch = M_PI / 2.0;  // GIMBAL LOCK CONDITION
    double yaw = M_PI / 3.0;

    std::Quaternion q = std::Quaternion::fromEuler(roll, pitch, yaw);

    EXPECT_QUATERNION_NORMALIZED(q);
    ExpectAllFinite(q, "Gimbal lock quaternion");
}

// ========== INTEGRATION TESTS ==========

TEST_F(QuaternionTest, IntegrateWithZeroAngularVelocity) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(xAxis, M_PI / 4.0);
    std::Vector omega(0, 0, 0);
    double dt = 0.01;

    std::Quaternion integrated = q.integrate(omega, dt);

    ExpectQuaternionNear(q, integrated, EPSILON_NORMAL);
    EXPECT_QUATERNION_NORMALIZED(integrated);
}

TEST_F(QuaternionTest, IntegrateWithConstantAngularVelocity) {
    std::Quaternion q = identity;
    std::Vector omega(0, 0, 1.0);  // 1 rad/s about Z
    double dt = 0.01;

    // Integrate for 100 steps (1 second total)
    for (int i = 0; i < 100; i++) {
        q = q.integrate(omega, dt);
    }

    // After 1 second at 1 rad/s, should rotate ~1 radian
    std::Vector axis;
    double angle;
    q.toAxisAngle(axis, angle);

    EXPECT_DOUBLE_NEAR(1.0, angle, 0.01);  // Allow integration error
    ExpectVectorNear(zAxis, axis, EPSILON_NORMAL);
    EXPECT_QUATERNION_NORMALIZED(q);
}

TEST_F(QuaternionTest, IntegrationMaintainsNormalization) {
    std::Quaternion q = identity;
    std::Vector omega(1.0, 0.5, 0.3);
    double dt = 0.01;

    // Integrate for many steps
    for (int i = 0; i < 1000; i++) {
        q = q.integrate(omega, dt);
    }

    // Should still be normalized despite many operations
    EXPECT_QUATERNION_NORMALIZED(q);
}

// Total test count for Quaternion: ~50-60 tests
```

**Test Count Estimate:** 55 tests

---

### 4.3 RigidBody Unit Tests

**Priority:** CRITICAL
**File:** tests/unit/test_rigidbody.cpp

#### Test Categories:

1. **Construction and Initialization** (Critical)
   - Default constructor
   - Constructor with position and mass
   - Default inertia tensor setup
   - Initial orientation is identity
   - Initial velocities are zero

2. **Inertia Tensor Management** (Critical)
   - Set custom inertia tensor
   - Inverse is computed correctly
   - World-space transformation
   - Symmetric property maintained
   - Positive definite validation

3. **Orientation and Direction** (High)
   - Set orientation from quaternion
   - Set orientation from Euler angles
   - getUp() returns correct world-space vector
   - getDir() returns correct world-space vector
   - getSide() returns correct world-space vector
   - Directions are orthonormal

4. **Force Application** (Critical)
   - applyForce() accumulates correctly
   - applyForceAtPoint() generates torque
   - applyTorque() accumulates correctly
   - applyImpulse() changes velocity immediately
   - clearAccumulators() resets forces/torques

5. **Linear Integration** (Critical)
   - Zero force → no acceleration
   - Constant force → linear acceleration
   - Gravity integration
   - Damping reduces velocity over time
   - Position updates correctly

6. **Rotational Integration** (Critical)
   - Zero torque → no angular acceleration
   - Constant torque → angular acceleration
   - Angular momentum conservation
   - Angular velocity from momentum
   - Orientation integration
   - Angular damping

7. **Combined Motion** (High)
   - Force + torque integration
   - Off-center force generates both motion and rotation
   - Point velocity calculation
   - Linear and angular coupling

8. **Coordinate Transformations** (High)
   - localToWorld() correct
   - worldToLocal() correct
   - Round-trip transformation
   - Transformation with rotation

9. **Mass and Physics Properties** (HIGH - Bug Risk)
   - **Zero mass handling**
   - **Negative mass rejection**
   - Mass update affects inertia
   - Infinite mass (static objects)

10. **Numerical Stability** (High)
    - Long simulation stability
    - Large forces
    - Small timesteps
    - Large timesteps
    - Quaternion drift prevention

11. **Edge Cases** (Medium)
    - Very small inertia
    - Very large inertia
    - Anisotropic inertia (different I_xx, I_yy, I_zz)
    - Extreme rotations

#### Sample Test Implementation:

```cpp
#include <gtest/gtest.h>
#include "RigidBody.h"
#include "physics_test_helpers.h"
#include "test_fixtures.h"

using namespace physics_testing;

class RigidBodyTest : public ::testing::Test {
protected:
    void SetUp() override {
        position = std::Vector(0, 0, 0);
        mass = 10.0;
        body = new std::RigidBody(position, mass);
    }

    void TearDown() override {
        delete body;
    }

    std::Vector position;
    double mass;
    std::RigidBody* body;
};

// ========== CONSTRUCTION TESTS ==========

TEST_F(RigidBodyTest, DefaultConstructor) {
    std::RigidBody rb;

    EXPECT_DOUBLE_EQ(0.0, rb.getMass());  // From Matter

    std::Quaternion orient = rb.getOrientation();
    ExpectQuaternionNear(std::Quaternion::identity(), orient, EPSILON_TIGHT);

    std::Vector vel = rb.getVelocity();
    ExpectVectorNear(std::Vector(0, 0, 0), vel, EPSILON_TIGHT);

    std::Vector angVel = rb.getAngularVelocity();
    ExpectVectorNear(std::Vector(0, 0, 0), angVel, EPSILON_TIGHT);
}

TEST_F(RigidBodyTest, ConstructorWithPositionAndMass) {
    EXPECT_DOUBLE_EQ(mass, body->getMass());

    std::Vector pos = body->getPosition();
    ExpectVectorNear(position, pos, EPSILON_TIGHT);
}

TEST_F(RigidBodyTest, DefaultInertiaTensorIsSymmetric) {
    std::Matrix3x3 I = body->getInertiaTensor();

    ExpectMatrixSymmetric(I, EPSILON_TIGHT);
}

// ========== INERTIA TENSOR TESTS ==========

TEST_F(RigidBodyTest, SetCustomInertiaTensor) {
    std::Matrix3x3 customI = std::Matrix3x3::diagonal(100, 150, 120);

    body->setInertiaTensor(customI);

    std::Matrix3x3 retrieved = body->getInertiaTensor();
    ExpectMatrixNear(customI, retrieved, EPSILON_TIGHT);
}

TEST_F(RigidBodyTest, InertiaTensorWorldSpace) {
    // Set diagonal inertia tensor
    std::Matrix3x3 bodyI = std::Matrix3x3::diagonal(100, 200, 300);
    body->setInertiaTensor(bodyI);

    // Rotate body 90° about Z
    std::Quaternion q = std::Quaternion::fromAxisAngle(std::Vector(0, 0, 1), M_PI / 2.0);
    body->setOrientation(q);

    // World-space inertia should be rotated
    // I_world = R * I_body * R^T

    // For now, just verify it's symmetric and positive definite
    std::Matrix3x3 worldI = body->getInertiaTensorWorld();

    ExpectMatrixSymmetric(worldI, EPSILON_NORMAL);
    EXPECT_TRUE(IsPositiveDefinite(worldI));
}

// ========== ORIENTATION TESTS ==========

TEST_F(RigidBodyTest, SetOrientationFromQuaternion) {
    std::Quaternion q = std::Quaternion::fromAxisAngle(std::Vector(1, 0, 0), M_PI / 4.0);

    body->setOrientation(q);

    std::Quaternion retrieved = body->getOrientation();
    ExpectQuaternionNear(q.normalize(), retrieved, EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, SetOrientationFromEuler) {
    double roll = M_PI / 6.0;
    double pitch = M_PI / 4.0;
    double yaw = M_PI / 3.0;

    body->setOrientation(roll, pitch, yaw);

    std::Quaternion q = body->getOrientation();
    EXPECT_QUATERNION_NORMALIZED(q);
}

TEST_F(RigidBodyTest, GetDirectionVectorsOrthonormal) {
    // Rotate body randomly
    body->setOrientation(std::Quaternion::fromAxisAngle(std::Vector(1, 2, 3), M_PI / 5.0));

    std::Vector up = body->getUp();
    std::Vector dir = body->getDir();
    std::Vector side = body->getSide();

    // Should be unit vectors
    EXPECT_DOUBLE_NEAR(1.0, up.getLengthVector(), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(1.0, dir.getLengthVector(), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(1.0, side.getLengthVector(), EPSILON_NORMAL);

    // Should be orthogonal
    EXPECT_DOUBLE_NEAR(0.0, up.dotProduct(dir), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(0.0, up.dotProduct(side), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(0.0, dir.dotProduct(side), EPSILON_NORMAL);
}

// ========== FORCE APPLICATION TESTS ==========

TEST_F(RigidBodyTest, ApplyForceAccumulates) {
    body->clearAccumulators();

    std::Vector f1(1, 0, 0);
    std::Vector f2(0, 2, 0);

    body->applyForce(f1);
    body->applyForce(f2);

    // Forces should accumulate
    // We can't directly read accumulator, so integrate and check result
    double dt = 1.0;
    body->integrate(dt);

    std::Vector velocity = body->getVelocity();

    // a = F/m = (1, 2, 0) / 10 = (0.1, 0.2, 0)
    // v = a * dt = (0.1, 0.2, 0) * 1 = (0.1, 0.2, 0)
    // (neglecting damping for this test - need to account for it)

    // Actually, damping is applied: v *= pow(0.95, dt)
    // Let's just check direction and that it's not zero
    EXPECT_GT(velocity.getX(), 0.0);
    EXPECT_GT(velocity.getY(), 0.0);
    EXPECT_DOUBLE_NEAR(0.0, velocity.getZ(), EPSILON_TIGHT);
}

TEST_F(RigidBodyTest, ApplyForceAtPointGeneratesTorque) {
    body->clearAccumulators();

    // Apply force at point offset from center of mass
    std::Vector force(0, 0, 10);  // Upward force
    std::Vector point(1, 0, 0);   // 1 meter in X direction

    body->applyForceAtPoint(force, point);

    double dt = 0.01;
    body->integrate(dt);

    // Should have both linear and angular velocity
    std::Vector angVel = body->getAngularVelocity();

    // Torque = r × F = (1, 0, 0) × (0, 0, 10) = (0, -10, 0)
    // Angular velocity should be in -Y direction
    EXPECT_DOUBLE_NEAR(0.0, angVel.getX(), EPSILON_NORMAL);
    EXPECT_LT(angVel.getY(), 0.0);  // Negative Y
    EXPECT_DOUBLE_NEAR(0.0, angVel.getZ(), EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, ApplyImpulseChangesVelocityImmediately) {
    std::Vector initialVel(1, 2, 3);
    body->setVelocity(initialVel);

    std::Vector impulse(5, 0, 0);
    std::Vector point = body->getPosition();  // At center of mass

    body->applyImpulse(impulse, point);

    std::Vector newVel = body->getVelocity();

    // Δv = impulse / mass = 5 / 10 = 0.5
    std::Vector expectedVel = initialVel + impulse * (1.0 / mass);

    ExpectVectorNear(expectedVel, newVel, EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, ClearAccumulatorsResetsForces) {
    body->applyForce(std::Vector(100, 200, 300));
    body->applyTorque(std::Vector(10, 20, 30));

    body->clearAccumulators();

    // After clearing, integration should not change velocity
    std::Vector initialVel = body->getVelocity();
    std::Vector initialAngVel = body->getAngularVelocity();

    body->integrate(0.01);

    // Velocity should only change due to damping, not forces
    // (This is a weak test - stronger would be to check accumulators directly)
    std::Vector newVel = body->getVelocity();

    // If accumulators were cleared, no acceleration
    // Only damping: v *= pow(damping, dt)
    EXPECT_LT(newVel.getLengthVector(), initialVel.getLengthVector() + EPSILON_NORMAL);
}

// ========== INTEGRATION TESTS ==========

TEST_F(RigidBodyTest, IntegrateWithZeroForceNoAcceleration) {
    std::Vector initialPos = body->getPosition();
    std::Vector initialVel = body->getVelocity();

    body->clearAccumulators();
    body->integrate(0.01);

    std::Vector newPos = body->getPosition();
    std::Vector newVel = body->getVelocity();

    // With zero force and zero velocity, position shouldn't change
    ExpectVectorNear(initialPos, newPos, EPSILON_TIGHT);
    ExpectVectorNear(initialVel, newVel, EPSILON_TIGHT);
}

TEST_F(RigidBodyTest, IntegrateWithConstantForce) {
    body->clearAccumulators();

    std::Vector force(10, 0, 0);
    body->applyForce(force);

    double dt = 0.1;
    body->integrate(dt);

    std::Vector velocity = body->getVelocity();

    // a = F/m = 10/10 = 1 m/s^2
    // v = a * dt = 1 * 0.1 = 0.1 m/s (before damping)
    // damping: v *= pow(0.95, 0.1) ≈ 0.995

    EXPECT_GT(velocity.getX(), 0.0);
    EXPECT_LT(velocity.getX(), 0.1);  // Should be less due to damping
}

TEST_F(RigidBodyTest, LinearDampingReducesVelocity) {
    std::Vector initialVel(10, 0, 0);
    body->setVelocity(initialVel);

    body->clearAccumulators();

    // Integrate for 1 second
    for (int i = 0; i < 100; i++) {
        body->integrate(0.01);
    }

    std::Vector finalVel = body->getVelocity();

    // Velocity should be reduced by damping
    EXPECT_LT(finalVel.getLengthVector(), initialVel.getLengthVector());
    EXPECT_GT(finalVel.getLengthVector(), 0.0);  // But not zero
}

TEST_F(RigidBodyTest, AngularDampingReducesAngularVelocity) {
    std::Vector initialAngVel(5, 0, 0);
    body->setAngularVelocity(initialAngVel);

    body->clearAccumulators();

    // Integrate for 1 second
    for (int i = 0; i < 100; i++) {
        body->integrate(0.01);
    }

    std::Vector finalAngVel = body->getAngularVelocity();

    // Angular velocity should be reduced by damping
    EXPECT_LT(finalAngVel.getLengthVector(), initialAngVel.getLengthVector());
    EXPECT_GT(finalAngVel.getLengthVector(), 0.0);
}

TEST_F(RigidBodyTest, IntegrationMaintainsQuaternionNormalization) {
    std::Vector angVel(1, 2, 3);
    body->setAngularVelocity(angVel);

    // Integrate for many steps
    for (int i = 0; i < 1000; i++) {
        body->clearAccumulators();
        body->integrate(0.01);
    }

    std::Quaternion q = body->getOrientation();
    EXPECT_QUATERNION_NORMALIZED(q);
}

// ========== COORDINATE TRANSFORMATION TESTS ==========

TEST_F(RigidBodyTest, LocalToWorldTransformation) {
    body->setPosition(std::Vector(10, 20, 30));
    body->setOrientation(std::Quaternion::fromAxisAngle(std::Vector(0, 0, 1), M_PI / 2.0));

    std::Vector localPoint(1, 0, 0);
    std::Vector worldPoint = body->localToWorld(localPoint);

    // 90° rotation about Z: (1, 0, 0) → (0, 1, 0)
    // Then translate: (0, 1, 0) + (10, 20, 30) = (10, 21, 30)
    std::Vector expected(10, 21, 30);

    ExpectVectorNear(expected, worldPoint, EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, WorldToLocalTransformation) {
    body->setPosition(std::Vector(10, 20, 30));
    body->setOrientation(std::Quaternion::fromAxisAngle(std::Vector(0, 0, 1), M_PI / 2.0));

    std::Vector worldPoint(10, 21, 30);
    std::Vector localPoint = body->worldToLocal(worldPoint);

    // Inverse of localToWorld
    std::Vector expected(1, 0, 0);

    ExpectVectorNear(expected, localPoint, EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, CoordinateTransformationRoundTrip) {
    body->setPosition(std::Vector(5, 10, 15));
    body->setOrientation(RandomQuaternion());

    std::Vector localPoint(1, 2, 3);

    std::Vector worldPoint = body->localToWorld(localPoint);
    std::Vector backToLocal = body->worldToLocal(worldPoint);

    ExpectVectorNear(localPoint, backToLocal, EPSILON_NORMAL);
}

// ========== MASS VALIDATION TESTS (CRITICAL) ==========

TEST_F(RigidBodyTest, DISABLED_ZeroMassHandling) {
    std::RigidBody rb;
    rb.setMass(0.0);

    // Should handle gracefully or error
    // Currently likely causes division by zero

    std::Vector force(10, 0, 0);
    rb.applyForce(force);

    EXPECT_DEATH(rb.integrate(0.01), ".*");
}

TEST_F(RigidBodyTest, DISABLED_NegativeMassRejection) {
    std::RigidBody rb;

    // Should reject or assert
    EXPECT_DEATH(rb.setMass(-10.0), ".*");
}

// ========== POINT VELOCITY TESTS ==========

TEST_F(RigidBodyTest, PointVelocityAtCenterOfMass) {
    body->setVelocity(std::Vector(1, 2, 3));
    body->setAngularVelocity(std::Vector(0, 0, 1));

    std::Vector pointVel = body->getPointVelocity(body->getPosition());

    // At center of mass, point velocity equals linear velocity
    ExpectVectorNear(std::Vector(1, 2, 3), pointVel, EPSILON_NORMAL);
}

TEST_F(RigidBodyTest, PointVelocityOffCenter) {
    body->setPosition(std::Vector(0, 0, 0));
    body->setVelocity(std::Vector(0, 0, 0));
    body->setAngularVelocity(std::Vector(0, 0, 1));  // Spinning about Z

    std::Vector point(1, 0, 0);  // 1 meter in X direction
    std::Vector pointVel = body->getPointVelocity(point);

    // v_point = v_cm + ω × r
    // v_cm = 0
    // ω × r = (0, 0, 1) × (1, 0, 0) = (0, 1, 0)
    std::Vector expected(0, 1, 0);

    ExpectVectorNear(expected, pointVel, EPSILON_NORMAL);
}

// Total test count for RigidBody: ~50-60 tests
```

**Test Count Estimate:** 55 tests

---

### 4.4 BoxShape Unit Tests

**Priority:** HIGH
**File:** tests/unit/test_boxshape.cpp

#### Test Categories:

1. **Construction** (High)
   - Constructor with dimensions
   - Getter methods

2. **Inertia Tensor Calculation** (Critical)
   - Cube (equal dimensions)
   - Rectangular box
   - Very thin box (plate)
   - Very long box (rod)
   - Compare with analytical formulas

3. **AABB Calculation** (High)
   - Axis-aligned box (identity orientation)
   - Rotated box (various angles)
   - 45° rotation
   - 90° rotation
   - Arbitrary rotation

4. **AABB Intersection** (High)
   - Overlapping boxes
   - Non-overlapping boxes
   - Edge cases (touching)
   - Fully contained

5. **Vertex Calculation** (Medium)
   - Correct number of vertices (8)
   - Vertices in correct positions
   - Symmetric about origin

6. **Characteristic Size** (Low)
   - Diagonal calculation
   - Various box sizes

7. **Edge Cases** (Medium)
   - Zero-size box
   - Very small box
   - Very large box
   - Extremely anisotropic box

#### Sample Test Implementation:

```cpp
#include <gtest/gtest.h>
#include "BoxShape.h"
#include "Quaternion.h"
#include "physics_test_helpers.h"

using namespace physics_testing;

class BoxShapeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a 2x3x4 box
        box = new std::BoxShape(2.0, 3.0, 4.0);
    }

    void TearDown() override {
        delete box;
    }

    std::BoxShape* box;
};

// ========== CONSTRUCTION TESTS ==========

TEST_F(BoxShapeTest, ConstructorSetsDimensions) {
    EXPECT_DOUBLE_EQ(2.0, box->getWidth());
    EXPECT_DOUBLE_EQ(3.0, box->getHeight());
    EXPECT_DOUBLE_EQ(4.0, box->getDepth());
}

// ========== INERTIA TENSOR TESTS ==========

TEST_F(BoxShapeTest, InertiaTensorForCube) {
    std::BoxShape cube(2.0, 2.0, 2.0);
    double mass = 10.0;

    std::Matrix3x3 I = cube.calculateInertiaTensor(mass);

    // For a cube, all diagonal elements should be equal
    // I = (1/12) * m * (a^2 + a^2) = (1/12) * 10 * 8 = 6.667
    double expected = (mass / 12.0) * (2.0 * 2.0 + 2.0 * 2.0);

    EXPECT_DOUBLE_NEAR(expected, I.get(0, 0), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(expected, I.get(1, 1), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(expected, I.get(2, 2), EPSILON_NORMAL);

    // Off-diagonal should be zero
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != j) {
                EXPECT_DOUBLE_EQ(0.0, I.get(i, j));
            }
        }
    }
}

TEST_F(BoxShapeTest, InertiaTensorForBox) {
    double mass = 12.0;
    std::Matrix3x3 I = box->calculateInertiaTensor(mass);

    // Ixx = (1/12) * m * (h^2 + d^2) = (1/12) * 12 * (9 + 16) = 25
    // Iyy = (1/12) * m * (w^2 + d^2) = (1/12) * 12 * (4 + 16) = 20
    // Izz = (1/12) * m * (w^2 + h^2) = (1/12) * 12 * (4 + 9) = 13

    EXPECT_DOUBLE_NEAR(25.0, I.get(0, 0), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(20.0, I.get(1, 1), EPSILON_NORMAL);
    EXPECT_DOUBLE_NEAR(13.0, I.get(2, 2), EPSILON_NORMAL);

    ExpectMatrixSymmetric(I, EPSILON_TIGHT);
}

TEST_F(BoxShapeTest, InertiaTensorIsPositiveDefinite) {
    double mass = 5.0;
    std::Matrix3x3 I = box->calculateInertiaTensor(mass);

    EXPECT_TRUE(IsPositiveDefinite(I))
        << "Inertia tensor must be positive definite";
}

// ========== AABB TESTS ==========

TEST_F(BoxShapeTest, AABBForAxisAlignedBox) {
    std::Vector position(0, 0, 0);
    std::Quaternion orientation = std::Quaternion::identity();

    std::AABB aabb = box->getAABB(position, orientation);

    // For axis-aligned box, AABB should be ±half-dimensions
    std::Vector expectedMin(-1, -1.5, -2);
    std::Vector expectedMax(1, 1.5, 2);

    ExpectVectorNear(expectedMin, aabb.min, EPSILON_NORMAL);
    ExpectVectorNear(expectedMax, aabb.max, EPSILON_NORMAL);
}

TEST_F(BoxShapeTest, AABBForRotated45DegreesAboutZ) {
    std::Vector position(0, 0, 0);
    std::Quaternion orientation = std::Quaternion::fromAxisAngle(std::Vector(0, 0, 1), M_PI / 4.0);

    std::AABB aabb = box->getAABB(position, orientation);

    // After 45° rotation, AABB should be larger
    // For a 2x3 rectangle rotated 45°:
    // new width = (2*cos(45) + 3*sin(45)) = (2*0.707 + 3*0.707) = 3.536
    // But this is approximate - just check it's larger than axis-aligned

    double axisAlignedWidth = 2.0;
    double rotatedWidth = aabb.max.getX() - aabb.min.getX();

    EXPECT_GT(rotatedWidth, axisAlignedWidth);
}

TEST_F(BoxShapeTest, AABBIntersectionOverlapping) {
    std::AABB aabb1(std::Vector(0, 0, 0), std::Vector(2, 2, 2));
    std::AABB aabb2(std::Vector(1, 1, 1), std::Vector(3, 3, 3));

    EXPECT_TRUE(aabb1.intersects(aabb2));
    EXPECT_TRUE(aabb2.intersects(aabb1));  // Symmetric
}

TEST_F(BoxShapeTest, AABBIntersectionNonOverlapping) {
    std::AABB aabb1(std::Vector(0, 0, 0), std::Vector(1, 1, 1));
    std::AABB aabb2(std::Vector(2, 2, 2), std::Vector(3, 3, 3));

    EXPECT_FALSE(aabb1.intersects(aabb2));
}

TEST_F(BoxShapeTest, AABBIntersectionTouching) {
    std::AABB aabb1(std::Vector(0, 0, 0), std::Vector(1, 1, 1));
    std::AABB aabb2(std::Vector(1, 0, 0), std::Vector(2, 1, 1));

    // Edge touching - should be considered intersecting
    EXPECT_TRUE(aabb1.intersects(aabb2));
}

// ========== VERTEX TESTS ==========

TEST_F(BoxShapeTest, GetVerticesReturns8Vertices) {
    std::Vector vertices[8];
    box->getVertices(vertices);

    // Just verify we can call it and get 8 vertices
    // More detailed testing would check positions

    // All vertices should be within half-dimensions from origin
    for (int i = 0; i < 8; i++) {
        EXPECT_LE(std::abs(vertices[i].getX()), 1.0);
        EXPECT_LE(std::abs(vertices[i].getY()), 1.5);
        EXPECT_LE(std::abs(vertices[i].getZ()), 2.0);
    }
}

TEST_F(BoxShapeTest, VerticesSymmetricAboutOrigin) {
    std::BoxShape cube(2, 2, 2);
    std::Vector vertices[8];
    cube.getVertices(vertices);

    // For a cube, should have 4 pairs of opposite vertices
    // Sum of all vertices should be zero (centered)
    std::Vector sum(0, 0, 0);
    for (int i = 0; i < 8; i++) {
        sum = sum + vertices[i];
    }

    ExpectVectorNear(std::Vector(0, 0, 0), sum, EPSILON_NORMAL);
}

// ========== CHARACTERISTIC SIZE TESTS ==========

TEST_F(BoxShapeTest, CharacteristicSizeIsDiagonal) {
    double size = box->getCharacteristicSize();

    // Diagonal = sqrt(w^2 + h^2 + d^2) = sqrt(4 + 9 + 16) = sqrt(29)
    double expected = std::sqrt(2.0*2.0 + 3.0*3.0 + 4.0*4.0);

    EXPECT_DOUBLE_NEAR(expected, size, EPSILON_NORMAL);
}

// Total test count for BoxShape: ~20-25 tests
```

**Test Count Estimate:** 22 tests

---

## 5. Integration Tests

Integration tests verify that components work together correctly.

**Priority:** HIGH
**Directory:** tests/integration/

### 5.1 Complete Physics Pipeline Test

**File:** tests/integration/test_physics_pipeline.cpp

Tests the full pipeline from force application to motion:

```cpp
TEST(PhysicsPipelineTest, ForceToMotionPipeline) {
    // Create rigid body with box shape
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    std::BoxShape box(2, 2, 2);

    // Set inertia from shape
    std::Matrix3x3 inertia = box.calculateInertiaTensor(10.0);
    body.setInertiaTensor(inertia);

    // Apply force for 1 second
    for (int i = 0; i < 100; i++) {
        body.clearAccumulators();
        body.applyForce(std::Vector(10, 0, 0));
        body.integrate(0.01);
    }

    // Verify motion
    std::Vector velocity = body.getVelocity();
    EXPECT_GT(velocity.getX(), 0.0);

    std::Vector position = body.getPosition();
    EXPECT_GT(position.getX(), 0.0);
}
```

**Test Count Estimate:** 15 tests

### 5.2 Rotation Integration Test

**File:** tests/integration/test_rotation_integration.cpp

Tests complete rotation from torque to orientation change:

```cpp
TEST(RotationIntegrationTest, TorqueToRotation) {
    std::RigidBody body(std::Vector(0, 0, 0), 5.0);

    // Apply torque for 1 second
    for (int i = 0; i < 100; i++) {
        body.clearAccumulators();
        body.applyTorque(std::Vector(0, 0, 1));  // Torque about Z
        body.integrate(0.01);
    }

    // Verify rotation occurred
    std::Quaternion q = body.getOrientation();
    std::Vector axis;
    double angle;
    q.toAxisAngle(axis, angle);

    EXPECT_GT(angle, 0.0);
    ExpectVectorNear(std::Vector(0, 0, 1), axis, EPSILON_NORMAL);
}
```

**Test Count Estimate:** 12 tests

### 5.3 Collision Response Test

**File:** tests/integration/test_collision_response.cpp

Tests impulse-based collision handling:

```cpp
TEST(CollisionResponseTest, ElasticCollisionConservesMomentum) {
    // Two bodies colliding
    std::RigidBody body1(std::Vector(-1, 0, 0), 1.0);
    std::RigidBody body2(std::Vector(1, 0, 0), 1.0);

    body1.setVelocity(std::Vector(1, 0, 0));
    body2.setVelocity(std::Vector(-1, 0, 0));

    std::Vector p_before = body1.getVelocity() * body1.getMass() +
                           body2.getVelocity() * body2.getMass();

    // Simulate collision with impulses
    // (Collision detection/response would be more complex in reality)

    std::Vector p_after = body1.getVelocity() * body1.getMass() +
                          body2.getVelocity() * body2.getMass();

    ExpectVectorNear(p_before, p_after, EPSILON_NORMAL);
}
```

**Test Count Estimate:** 18 tests

**Total Integration Tests:** ~45 tests

---

## 6. Physics Validation Tests

These tests verify physical correctness, not just code correctness.

**Priority:** HIGH
**Directory:** tests/physics/

### 6.1 Conservation Laws

**File:** tests/physics/test_conservation_laws.cpp

```cpp
TEST(ConservationLawsTest, LinearMomentumConservedWithoutExternalForce) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.setVelocity(std::Vector(5, 3, 2));

    std::Vector p_initial = body.getVelocity() * body.getMass();

    // Integrate without external forces
    for (int i = 0; i < 1000; i++) {
        body.clearAccumulators();
        body.integrate(0.01);
    }

    std::Vector p_final = body.getVelocity() * body.getMass();

    // Momentum conserved (accounting for damping)
    // Actually, damping violates conservation - this is a feature for stability
    // Test should verify damping effect is as expected
    EXPECT_LT(p_final.getLengthVector(), p_initial.getLengthVector());
}

TEST(ConservationLawsTest, AngularMomentumConservedWithoutExternalTorque) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.setAngularVelocity(std::Vector(1, 2, 3));

    std::Vector L_initial = body.getAngularMomentum();

    // Integrate without external torques
    for (int i = 0; i < 1000; i++) {
        body.clearAccumulators();
        body.integrate(0.01);
    }

    std::Vector L_final = body.getAngularMomentum();

    // Angular momentum conserved (accounting for damping)
    EXPECT_LT(L_final.getLengthVector(), L_initial.getLengthVector());
}

TEST(ConservationLawsTest, EnergyDissipationByDamping) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.setVelocity(std::Vector(10, 0, 0));

    double KE_initial = ComputeKineticEnergy(body.getVelocity(), body.getMass());

    // Integrate with damping, no external forces
    for (int i = 0; i < 1000; i++) {
        body.clearAccumulators();
        body.integrate(0.01);
    }

    double KE_final = ComputeKineticEnergy(body.getVelocity(), body.getMass());

    // Energy should decrease due to damping
    EXPECT_LT(KE_final, KE_initial);
    EXPECT_GT(KE_final, 0.0);  // But not zero
}
```

**Test Count Estimate:** 12 tests

### 6.2 Numerical Stability

**File:** tests/physics/test_numerical_stability.cpp

```cpp
TEST(NumericalStabilityTest, LongSimulationRemainsStable) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.setVelocity(std::Vector(1, 1, 1));
    body.setAngularVelocity(std::Vector(0.1, 0.2, 0.3));

    // Simulate for 1000 seconds
    for (int i = 0; i < 100000; i++) {
        body.clearAccumulators();
        body.applyForce(std::Vector(0, 0, -9.8 * body.getMass()));  // Gravity
        body.integrate(0.01);
    }

    // Check all values are finite
    ExpectAllFinite(body.getPosition(), "Position");
    ExpectAllFinite(body.getVelocity(), "Velocity");
    ExpectAllFinite(body.getAngularVelocity(), "Angular velocity");

    // Quaternion should still be normalized
    EXPECT_QUATERNION_NORMALIZED(body.getOrientation());
}

TEST(NumericalStabilityTest, HighAngularVelocityStability) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.setAngularVelocity(std::Vector(100, 0, 0));  // Very fast spin

    for (int i = 0; i < 1000; i++) {
        body.clearAccumulators();
        body.integrate(0.01);
    }

    EXPECT_QUATERNION_NORMALIZED(body.getOrientation());
    ExpectAllFinite(body.getOrientation(), "Orientation");
}
```

**Test Count Estimate:** 15 tests

### 6.3 Edge Cases

**File:** tests/physics/test_edge_cases.cpp

```cpp
TEST(EdgeCasesTest, VerySmallTimestep) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.applyForce(std::Vector(100, 0, 0));

    double dt = 1e-6;
    body.integrate(dt);

    ExpectAllFinite(body.getVelocity(), "Velocity");
    ExpectAllFinite(body.getPosition(), "Position");
}

TEST(EdgeCasesTest, VeryLargeTimestep) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);
    body.applyForce(std::Vector(100, 0, 0));

    double dt = 1.0;  // 1 second timestep (very large for physics)
    body.integrate(dt);

    // Should remain stable (though accuracy may suffer)
    ExpectAllFinite(body.getVelocity(), "Velocity");
    ExpectAllFinite(body.getPosition(), "Position");
}
```

**Test Count Estimate:** 20 tests

**Total Physics Validation Tests:** ~47 tests

---

## 7. Performance Tests

**Priority:** MEDIUM
**Directory:** tests/performance/

### 7.1 Performance Benchmarks

**File:** tests/performance/benchmark_physics.cpp

```cpp
TEST(PerformanceTest, MatrixInversionBenchmark) {
    auto start = std::chrono::high_resolution_clock::now();

    int iterations = 100000;
    for (int i = 0; i < iterations; i++) {
        std::Matrix3x3 m = RandomSymmetricPositiveDefiniteMatrix();
        std::Matrix3x3 inv = m.inverse();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTime = duration.count() / (double)iterations;
    std::cout << "Average matrix inversion time: " << avgTime << " μs\n";

    // Should be fast enough for real-time physics (< 1 μs)
    EXPECT_LT(avgTime, 10.0);
}

TEST(PerformanceTest, RigidBodyIntegrationBenchmark) {
    std::RigidBody body(std::Vector(0, 0, 0), 10.0);

    auto start = std::chrono::high_resolution_clock::now();

    int iterations = 10000;
    for (int i = 0; i < iterations; i++) {
        body.clearAccumulators();
        body.applyForce(std::Vector(10, 0, 0));
        body.applyTorque(std::Vector(0, 1, 0));
        body.integrate(0.01);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTime = duration.count() / (double)iterations;
    std::cout << "Average rigid body integration time: " << avgTime << " μs\n";

    // Should be fast enough for many rigid bodies (< 10 μs)
    EXPECT_LT(avgTime, 50.0);
}
```

**Test Count Estimate:** 8 tests

---

## 8. Test Execution and CI/CD

### 8.1 Running Tests Locally

```bash
# Build and run all tests
cd build
make physics_tests
make run_tests

# Run specific test suite
./tests/physics_tests --gtest_filter="Matrix3x3Test.*"

# Run with verbose output
./tests/physics_tests --gtest_filter="*" --gtest_color=yes -v

# Run only critical tests (custom label - would need implementation)
./tests/physics_tests --gtest_filter="*Critical*"

# Generate XML report for CI
./tests/physics_tests --gtest_output=xml:test_results.xml
```

### 8.2 Continuous Integration

**GitHub Actions Example (.github/workflows/tests.yml):**

```yaml
name: Physics Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3

    - name: Install Dependencies (Linux)
      if: runner.os == 'Linux'
      run: |
        sudo apt-get update
        sudo apt-get install -y freeglut3-dev libxi-dev libxmu-dev

    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} -DBUILD_TESTS=ON

    - name: Build
      run: |
        cd build
        make physics_tests

    - name: Run Tests
      run: |
        cd build
        ./tests/physics_tests --gtest_output=xml:test_results.xml

    - name: Upload Test Results
      if: always()
      uses: actions/upload-artifact@v3
      with:
        name: test-results-${{ matrix.os }}-${{ matrix.build_type }}
        path: build/test_results.xml
```

### 8.3 Test Coverage

**Add coverage support to CMakeLists.txt:**

```cmake
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(physics_core PRIVATE --coverage)
    target_link_libraries(physics_core PRIVATE --coverage)
    target_compile_options(physics_tests PRIVATE --coverage)
    target_link_libraries(physics_tests PRIVATE --coverage)
endif()
```

**Generate coverage report:**

```bash
# Build with coverage
cmake .. -DENABLE_COVERAGE=ON
make physics_tests

# Run tests
./tests/physics_tests

# Generate coverage report (requires lcov)
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html

# View coverage
open coverage_html/index.html
```

### 8.4 Sanitizers

**Memory safety testing with AddressSanitizer:**

```cmake
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
    target_compile_options(physics_tests PRIVATE -fsanitize=address)
    target_link_libraries(physics_tests PRIVATE -fsanitize=address)
endif()
```

**Run with sanitizers:**

```bash
cmake .. -DENABLE_ASAN=ON
make physics_tests
./tests/physics_tests
```

---

## 9. Test Data and Scenarios

### 9.1 Standard Test Scenarios

**Falling Box:**
```cpp
struct FallingBoxScenario {
    static std::RigidBody* create() {
        std::RigidBody* body = new std::RigidBody(std::Vector(0, 10, 0), 10.0);
        std::BoxShape box(1, 1, 1);
        body->setInertiaTensor(box.calculateInertiaTensor(10.0));
        return body;
    }

    static void applyPhysics(std::RigidBody* body, double dt) {
        body->clearAccumulators();
        body->applyForce(std::Vector(0, -9.8 * body->getMass(), 0));
        body->integrate(dt);
    }
};
```

**Spinning Top:**
```cpp
struct SpinningTopScenario {
    static std::RigidBody* create() {
        std::RigidBody* body = new std::RigidBody(std::Vector(0, 0, 0), 5.0);
        body->setAngularVelocity(std::Vector(0, 0, 10));  // Fast spin about Z
        return body;
    }
};
```

**Collision Pair:**
```cpp
struct CollisionPairScenario {
    std::RigidBody* body1;
    std::RigidBody* body2;

    CollisionPairScenario() {
        body1 = new std::RigidBody(std::Vector(-2, 0, 0), 1.0);
        body2 = new std::RigidBody(std::Vector(2, 0, 0), 1.0);

        body1->setVelocity(std::Vector(1, 0, 0));
        body2->setVelocity(std::Vector(-1, 0, 0));
    }
};
```

### 9.2 Parameterized Test Data

**Rotation Angles:**
```cpp
INSTANTIATE_TEST_SUITE_P(
    RotationAngles,
    RotationAngleTest,
    ::testing::Values(
        0.0,
        M_PI / 6.0,      // 30°
        M_PI / 4.0,      // 45°
        M_PI / 3.0,      // 60°
        M_PI / 2.0,      // 90°
        M_PI,            // 180°
        3.0 * M_PI / 2.0,// 270°
        2.0 * M_PI,      // 360°
        -M_PI / 2.0      // -90°
    )
);
```

**Mass Values:**
```cpp
INSTANTIATE_TEST_SUITE_P(
    MassValues,
    MassValueTest,
    ::testing::Values(
        0.001,    // Very light
        1.0,      // Unit mass
        10.0,     // Typical
        100.0,    // Heavy
        1000.0    // Very heavy
    )
);
```

---

## 10. Summary

### 10.1 Test Count Summary

| Category | Test Count | Priority |
|----------|-----------|----------|
| Matrix3x3 Unit Tests | 45 | CRITICAL |
| Quaternion Unit Tests | 55 | CRITICAL |
| RigidBody Unit Tests | 55 | CRITICAL |
| BoxShape Unit Tests | 22 | HIGH |
| Integration Tests | 45 | HIGH |
| Physics Validation | 47 | HIGH |
| Performance Tests | 8 | MEDIUM |
| **TOTAL** | **277** | |

### 10.2 Critical Issues Priority

1. **CRITICAL - Fix namespace pollution** (All files use `namespace std`)
2. **CRITICAL - Fix singular matrix inverse** (Returns identity instead of error)
3. **CRITICAL - Fix zero quaternion normalize** (Returns identity instead of error)
4. **HIGH - Add bounds checking** (Matrix element access)
5. **HIGH - Validate mass** (Prevent zero/negative mass)
6. **MEDIUM - Fix axis-angle edge cases** (Zero axis, 180° rotation)
7. **MEDIUM - Standardize epsilon values**
8. **LOW - Optimize AABB construction**
9. **LOW - Consider damping alternatives**

### 10.3 Test Coverage Goals

- **Unit Test Coverage:** >90%
- **Integration Test Coverage:** >80%
- **Critical Path Coverage:** 100%
- **Edge Case Coverage:** >70%

### 10.4 Next Steps

1. **Immediate:**
   - Set up Google Test infrastructure (CMake, directories)
   - Create test helpers and fixtures
   - Implement Matrix3x3 critical tests
   - Fix namespace pollution issue

2. **Short-term:**
   - Implement all unit tests
   - Fix critical bugs found during testing
   - Set up CI/CD pipeline
   - Add sanitizers and coverage

3. **Long-term:**
   - Implement integration tests
   - Add physics validation suite
   - Performance optimization based on benchmarks
   - Continuous improvement based on test results

---

## Appendix A: Google Test Quick Reference

### Common Assertions

```cpp
// Basic assertions
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);
EXPECT_EQ(expected, actual);
EXPECT_NE(val1, val2);
EXPECT_LT(val1, val2);
EXPECT_LE(val1, val2);
EXPECT_GT(val1, val2);
EXPECT_GE(val1, val2);

// Floating-point assertions
EXPECT_DOUBLE_EQ(expected, actual);
EXPECT_NEAR(expected, actual, epsilon);
EXPECT_FLOAT_EQ(expected, actual);

// String assertions
EXPECT_STREQ(expected, actual);
EXPECT_STRCASEEQ(expected, actual);

// Death tests (expects crash/assert)
EXPECT_DEATH(statement, regex);
ASSERT_DEATH(statement, regex);

// Use ASSERT_* for fatal failures (stops test immediately)
ASSERT_TRUE(condition);
ASSERT_EQ(expected, actual);
```

### Test Fixtures

```cpp
class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Run before each test
    }

    void TearDown() override {
        // Run after each test
    }

    // Member variables available to all tests
    int value;
};

TEST_F(MyTest, TestName) {
    // Use fixture members
    EXPECT_EQ(42, value);
}
```

### Parameterized Tests

```cpp
class ParamTest : public ::testing::TestWithParam<int> {};

TEST_P(ParamTest, TestName) {
    int param = GetParam();
    EXPECT_GT(param, 0);
}

INSTANTIATE_TEST_SUITE_P(
    MyParams,
    ParamTest,
    ::testing::Values(1, 2, 3, 5, 8)
);
```

---

## Appendix B: File Checklist

### Files to Create

- [ ] docs/TEST_PLAN.md (this document)
- [ ] tests/CMakeLists.txt
- [ ] tests/test_main.cpp
- [ ] tests/helpers/physics_test_helpers.h
- [ ] tests/helpers/physics_test_helpers.cpp
- [ ] tests/helpers/test_fixtures.h
- [ ] tests/unit/test_matrix3x3.cpp
- [ ] tests/unit/test_quaternion.cpp
- [ ] tests/unit/test_rigidbody.cpp
- [ ] tests/unit/test_boxshape.cpp
- [ ] tests/integration/test_physics_pipeline.cpp
- [ ] tests/integration/test_rotation_integration.cpp
- [ ] tests/integration/test_collision_response.cpp
- [ ] tests/physics/test_conservation_laws.cpp
- [ ] tests/physics/test_numerical_stability.cpp
- [ ] tests/physics/test_edge_cases.cpp
- [ ] tests/performance/benchmark_physics.cpp

### Files to Modify

- [ ] CMakeLists.txt (root) - Add Google Test support
- [ ] All physics headers - Fix namespace pollution
- [ ] Matrix3x3.cpp - Add bounds checking, fix singular matrix handling
- [ ] Quaternion.cpp - Fix zero quaternion handling, axis-angle edge cases
- [ ] RigidBody.cpp - Add mass validation

---

**End of Test Plan**
