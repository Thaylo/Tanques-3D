# Testing Quick Reference

This is a quick reference guide for the Tanques-3D test infrastructure.

## Quick Start

### Build and Run All Tests
```bash
./run_tests.sh
```

### Build Tests Only (No Run)
```bash
./build_tests.sh
```

### Clean Build and Run
```bash
./run_tests.sh clean
```

## Common Operations

### List All Tests
```bash
./run_tests.sh --list
```

### Run Specific Test Suite
```bash
./run_tests.sh --filter=VectorTest*
./run_tests.sh --filter=MatrixTest*
./run_tests.sh --filter=QuaternionTest*
```

### Run Specific Test Case
```bash
./run_tests.sh --filter=VectorTest.Addition
./run_tests.sh --filter=MatrixTest.Transpose
```

### Run Multiple Test Suites
```bash
./run_tests.sh --filter=VectorTest*:MatrixTest*
```

### Verbose Output
```bash
./run_tests.sh --verbose
```

## Test Helper Macros

### Vector Assertions
```cpp
EXPECT_VECTOR_EQ(v1, v2);              // Default epsilon (1e-6)
EXPECT_VECTOR_NEAR(v1, v2, 1e-5);     // Custom epsilon
```

### Matrix Assertions
```cpp
EXPECT_MATRIX_EQ(m1, m2);              // Default epsilon (1e-6)
EXPECT_MATRIX_NEAR(m1, m2, 1e-5);     // Custom epsilon
```

### Quaternion Assertions
```cpp
EXPECT_QUATERNION_EQ(q1, q2);          // Default epsilon (1e-6)
EXPECT_QUATERNION_NEAR(q1, q2, 1e-5); // Custom epsilon
```

### Standard Assertions
```cpp
EXPECT_DOUBLE_EQ(a, b);                // Floating-point equality
EXPECT_NEAR(a, b, epsilon);            // Custom tolerance
EXPECT_TRUE(condition);                // Boolean true
EXPECT_FALSE(condition);               // Boolean false
```

## Writing New Tests

### Basic Test
```cpp
#include <gtest/gtest.h>
#include "test_helpers.h"
#include "YourClass.h"

TEST(TestSuiteName, TestCaseName) {
    // Arrange
    YourClass obj;

    // Act
    auto result = obj.doSomething();

    // Assert
    EXPECT_EQ(result, expected);
}
```

### Test Fixture
```cpp
class MyClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code
    }

    void TearDown() override {
        // Cleanup code
    }

    // Shared data
    MyClass obj;
};

TEST_F(MyClassTest, SomeTest) {
    // Use obj from fixture
    EXPECT_TRUE(obj.isValid());
}
```

## File Locations

- **Test Files**: `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_*.cpp`
- **Test Helpers**: `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_helpers.h`
- **Test Main**: `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_main.cpp`
- **CMake Config**: `/Users/thaylofreitas/Projects/Tanques-3D/tests/CMakeLists.txt`
- **Test Executable**: `/Users/thaylofreitas/Projects/Tanques-3D/build/bin/tests/physics_tests`

## Documentation

For detailed documentation, see:
- **Full Guide**: `/Users/thaylofreitas/Projects/Tanques-3D/tests/README.md`
- **Google Test Docs**: https://google.github.io/googletest/

## Troubleshooting

### Tests Won't Build
```bash
# Clean rebuild
./build_tests.sh clean
```

### Can't Find Test Executable
```bash
# Check if it exists
ls -la build/bin/tests/physics_tests
```

### CMake Configuration Issues
```bash
# Reconfigure from scratch
rm -rf build
./build_tests.sh
```

## CMake Options

### Enable Testing (default: ON)
```bash
cmake -DBUILD_TESTING=ON ..
```

### Disable Testing
```bash
cmake -DBUILD_TESTING=OFF ..
```

## Using CTest

Alternative to run_tests.sh:

```bash
cd build
ctest --output-on-failure
ctest -V                    # Verbose
ctest -R VectorTest         # Run specific tests
```

## Exit Codes

- `0`: All tests passed
- Non-zero: Some tests failed

Use in scripts:
```bash
if ./run_tests.sh; then
    echo "Tests passed!"
else
    echo "Tests failed!"
    exit 1
fi
```
