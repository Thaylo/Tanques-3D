# Tanques-3D Physics Test Suite

This directory contains the Google Test-based test infrastructure for the Tanques-3D physics system.

## Overview

The test suite uses **Google Test (gtest)** framework to validate the physics engine components including:
- Vector mathematics
- Matrix operations (3x3 matrices)
- Quaternion rotations
- Rigid body physics
- Collision shapes

## Quick Start

### Building Tests

From the project root directory:

```bash
./build_tests.sh
```

To perform a clean build:

```bash
./build_tests.sh clean
```

### Running Tests

Run all tests:

```bash
./run_tests.sh
```

Run with clean build:

```bash
./run_tests.sh clean
```

### Common Test Commands

List all available tests:
```bash
./run_tests.sh --list
```

Run specific test suite:
```bash
./run_tests.sh --filter=VectorTest*
```

Run specific test case:
```bash
./run_tests.sh --filter=VectorTest.Addition
```

Run with verbose output:
```bash
./run_tests.sh --verbose
```

Get help:
```bash
./run_tests.sh --help
```

## Project Structure

```
tests/
├── CMakeLists.txt          # CMake configuration for test suite
├── README.md               # This file
├── test_helpers.h          # Test utility macros and helper functions
├── test_main.cpp           # Main entry point for Google Test
├── test_vector.cpp         # Vector class tests (example)
├── test_matrix.cpp         # Matrix3x3 class tests (example)
└── test_quaternion.cpp     # Quaternion class tests (example)
```

## Writing Tests

### Basic Test Structure

All test files should follow this pattern:

```cpp
#include <gtest/gtest.h>
#include "test_helpers.h"
#include "Vector.h"  // Or other class being tested

// Test fixture (optional, for shared setup/teardown)
class VectorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Common setup code
    }

    void TearDown() override {
        // Common cleanup code
    }

    // Shared test data
    Vector v1;
    Vector v2;
};

// Test case using fixture
TEST_F(VectorTest, Addition) {
    Vector v1(1.0, 2.0, 3.0);
    Vector v2(4.0, 5.0, 6.0);
    Vector result = v1 + v2;

    EXPECT_VECTOR_EQ(result, Vector(5.0, 7.0, 9.0));
}

// Simple test without fixture
TEST(VectorBasicTest, Construction) {
    Vector v(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v.getX(), 1.0);
    EXPECT_DOUBLE_EQ(v.getY(), 2.0);
    EXPECT_DOUBLE_EQ(v.getZ(), 3.0);
}
```

### Test Helper Macros

The `test_helpers.h` header provides custom assertion macros for physics types:

#### Vector Comparison
```cpp
EXPECT_VECTOR_NEAR(v1, v2, epsilon);  // Compare with custom epsilon
EXPECT_VECTOR_EQ(v1, v2);             // Compare with default epsilon (1e-6)
```

#### Matrix Comparison
```cpp
EXPECT_MATRIX_NEAR(m1, m2, epsilon);  // Compare with custom epsilon
EXPECT_MATRIX_EQ(m1, m2);             // Compare with default epsilon (1e-6)
```

#### Quaternion Comparison
```cpp
EXPECT_QUATERNION_NEAR(q1, q2, epsilon);  // Compare with custom epsilon
EXPECT_QUATERNION_EQ(q1, q2);             // Compare with default epsilon (1e-6)
```

#### Standard Google Test Assertions

For scalar values:
```cpp
EXPECT_DOUBLE_EQ(actual, expected);       // Exact equality (with floating-point tolerance)
EXPECT_NEAR(actual, expected, epsilon);   // Near equality with custom epsilon
EXPECT_TRUE(condition);                   // Boolean assertion
EXPECT_FALSE(condition);                  // Boolean assertion
```

### Helper Functions

The test helpers provide utility functions:

```cpp
// Check approximate equality
bool approximatelyEqual(double a, double b, double epsilon = TEST_EPSILON);

// Check if value is approximately zero
bool approximatelyZero(double value, double epsilon = TEST_EPSILON);

// Create normalized vector
Vector makeNormalizedVector(double x, double y, double z);

// Check if vectors are parallel
bool areVectorsParallel(const Vector& v1, const Vector& v2, double epsilon = TEST_EPSILON);

// Check if vectors are perpendicular
bool areVectorsPerpendicular(const Vector& v1, const Vector& v2, double epsilon = TEST_EPSILON);

// Check if matrix is identity
bool isIdentityMatrix(const std::Matrix3x3& m, double epsilon = TEST_EPSILON);

// Check if quaternion is normalized
bool isQuaternionNormalized(const std::Quaternion& q, double epsilon = TEST_EPSILON);

// Debug string conversion
std::string vectorToString(const Vector& v);
std::string quaternionToString(const std::Quaternion& q);
```

## Adding New Test Files

1. Create a new test file in the `tests/` directory following the naming convention `test_<component>.cpp`:

```cpp
// tests/test_rigidbody.cpp
#include <gtest/gtest.h>
#include "test_helpers.h"
#include "RigidBody.h"

TEST(RigidBodyTest, Initialization) {
    // Your test code here
}
```

2. The CMake configuration automatically discovers all `test_*.cpp` files, so no configuration changes are needed.

3. Rebuild the tests:

```bash
./build_tests.sh
```

4. Run your new tests:

```bash
./run_tests.sh --filter=RigidBodyTest*
```

## Test Organization Best Practices

### Naming Conventions

- **Test Files**: `test_<component>.cpp` (e.g., `test_vector.cpp`)
- **Test Suites**: `<ComponentName>Test` (e.g., `VectorTest`, `MatrixTest`)
- **Test Cases**: Descriptive names using PascalCase (e.g., `Addition`, `CrossProduct`, `NormalizationPreservesDirection`)

### Test Structure

```cpp
TEST(TestSuiteName, TestCaseName) {
    // Arrange - Set up test data
    Vector v1(1, 0, 0);
    Vector v2(0, 1, 0);

    // Act - Perform the operation
    Vector result = v1.crossProduct(v2);

    // Assert - Verify the result
    EXPECT_VECTOR_EQ(result, Vector(0, 0, 1));
}
```

### Test Categories

Organize tests into logical groups:

1. **Construction/Initialization Tests**
   - Default constructors
   - Parameterized constructors
   - Factory methods

2. **Basic Operation Tests**
   - Arithmetic operations
   - Comparison operations
   - Assignment operations

3. **Mathematical Operation Tests**
   - Vector operations (dot, cross, normalize)
   - Matrix operations (transpose, inverse, determinant)
   - Quaternion operations (rotation, integration)

4. **Edge Case Tests**
   - Zero vectors/matrices
   - Division by zero
   - Denormalized quaternions

5. **Integration Tests**
   - Combined operations
   - Physics simulations
   - Performance tests

## CMake Configuration

### Building Tests Only

The test suite can be built independently:

```bash
cd build
cmake -DBUILD_TESTING=ON ..
cmake --build . --target physics_tests
```

### Disabling Tests

To build the project without tests:

```bash
cmake -DBUILD_TESTING=OFF ..
```

### Test Dependencies

Tests are built with:
- Google Test 1.14.0 (fetched automatically via CMake FetchContent)
- C++11 standard (matching main project)
- Debug symbols enabled for better debugging

## Running Tests with CTest

CMake's CTest integration is also available:

```bash
cd build
ctest --output-on-failure
```

Run with verbose output:
```bash
ctest -V
```

Run specific tests:
```bash
ctest -R VectorTest
```

## Continuous Integration

The test suite is designed to integrate with CI/CD pipelines:

```bash
# In your CI script
./build_tests.sh clean
./run_tests.sh
```

Exit codes:
- `0`: All tests passed
- Non-zero: Some tests failed

## Debugging Failed Tests

### Running Specific Failed Tests

```bash
./run_tests.sh --filter=FailedTestName
```

### Debugging with GDB (macOS/Linux)

```bash
cd build/bin/tests
gdb ./physics_tests
(gdb) run --gtest_filter=FailedTestName
```

### Debugging with LLDB (macOS)

```bash
cd build/bin/tests
lldb ./physics_tests
(lldb) run -- --gtest_filter=FailedTestName
```

## Test Coverage

To enable code coverage (requires gcov/lcov):

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
cmake --build .
./run_tests.sh
# Generate coverage report
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## Performance Testing

For performance-critical code, use Google Test's benchmarking features:

```cpp
#include <gtest/gtest.h>
#include <chrono>

TEST(PerformanceTest, MatrixMultiplication) {
    Matrix3x3 m1 = Matrix3x3::identity();
    Matrix3x3 m2 = Matrix3x3::identity();

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000000; ++i) {
        Matrix3x3 result = m1 * m2;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "1M matrix multiplications: " << duration.count() << " μs" << std::endl;
}
```

## Common Issues

### Issue: Tests not found
**Solution**: Make sure your test file starts with `test_` and is in the `tests/` directory.

### Issue: Linker errors with multiple definitions
**Solution**: Ensure you're not including `Main.cpp` in test builds. The `tests/CMakeLists.txt` excludes it automatically.

### Issue: Google Test download fails
**Solution**: Check your internet connection. Google Test is downloaded from GitHub during CMake configuration.

### Issue: OpenGL/GLUT dependencies in tests
**Solution**: Tests only link against core physics classes and don't require OpenGL/GLUT.

## Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [Google Test Primer](https://google.github.io/googletest/primer.html)
- [Google Test Advanced Guide](https://google.github.io/googletest/advanced.html)
- [CMake Testing Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)

## Contributing

When adding new physics features:

1. Write tests first (TDD approach)
2. Ensure all tests pass before committing
3. Aim for high test coverage of critical physics code
4. Document edge cases in test comments
5. Use descriptive test names that explain what is being tested

## License

Same as the main project.
