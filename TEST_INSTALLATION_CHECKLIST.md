# Test Infrastructure Installation Checklist

Use this checklist to verify that the Google Test infrastructure is properly set up.

## File Verification

### Core Files
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/CMakeLists.txt` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_helpers.h` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_main.cpp` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/test_example.cpp` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/.gitignore` exists

### Build Scripts
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/build_tests.sh` exists and is executable
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/run_tests.sh` exists and is executable

### Documentation
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/tests/README.md` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/TESTING.md` exists
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/TEST_INFRASTRUCTURE_SUMMARY.md` exists

### CMake Integration
- [ ] Main `CMakeLists.txt` has `BUILD_TESTING` option
- [ ] Main `CMakeLists.txt` includes `add_subdirectory(tests)`
- [ ] Main `CMakeLists.txt` has `enable_testing()` call

## Quick Verification Commands

Run these commands from the project root:

### Check Files Exist
```bash
ls -la tests/CMakeLists.txt
ls -la tests/test_helpers.h
ls -la tests/test_main.cpp
ls -la tests/test_example.cpp
ls -la build_tests.sh
ls -la run_tests.sh
```

### Check Scripts Are Executable
```bash
test -x build_tests.sh && echo "build_tests.sh is executable" || echo "ERROR: build_tests.sh not executable"
test -x run_tests.sh && echo "run_tests.sh is executable" || echo "ERROR: run_tests.sh not executable"
```

### Check CMake Version
```bash
cmake --version
# Should be 3.14 or higher
```

## Build Verification

### Step 1: Build Tests
```bash
./build_tests.sh
```

**Expected Output:**
- CMake configuration messages
- Google Test download messages (first time only)
- Compilation messages
- Success message with test executable path

**Verify:**
- [ ] Build completes without errors
- [ ] `/Users/thaylofreitas/Projects/Tanques-3D/build/bin/tests/physics_tests` exists

### Step 2: Run Tests
```bash
./run_tests.sh
```

**Expected Output:**
- Build messages
- Test execution messages
- Test results (pass/fail)
- Summary of test results

**Verify:**
- [ ] Tests execute without crashes
- [ ] Example tests pass (from `test_example.cpp`)
- [ ] Clear pass/fail indicators shown

### Step 3: List Tests
```bash
./run_tests.sh --list
```

**Verify:**
- [ ] All test suites are listed
- [ ] Example tests are visible (VectorTest, MatrixTest, QuaternionTest)

### Step 4: Run Specific Test
```bash
./run_tests.sh --filter=VectorTest.Addition
```

**Verify:**
- [ ] Only the specified test runs
- [ ] Test result is shown

## Functional Tests

### Test 1: Custom Assertions Work
Check that `test_example.cpp` compiles and runs successfully with custom macros.

```bash
./run_tests.sh --filter=VectorTest*
```

**Verify:**
- [ ] `EXPECT_VECTOR_EQ` macro works
- [ ] `EXPECT_VECTOR_NEAR` macro works
- [ ] Tests pass or fail appropriately

### Test 2: Helper Functions Work
Check that helper functions in `test_helpers.h` are usable.

```bash
./run_tests.sh --filter=VectorTest.PerpendicularVectors
```

**Verify:**
- [ ] Helper function `areVectorsPerpendicular()` compiles
- [ ] Test executes successfully

### Test 3: Matrix Assertions Work
```bash
./run_tests.sh --filter=MatrixTest*
```

**Verify:**
- [ ] `EXPECT_MATRIX_EQ` macro works
- [ ] `EXPECT_MATRIX_NEAR` macro works

### Test 4: Quaternion Assertions Work
```bash
./run_tests.sh --filter=QuaternionTest*
```

**Verify:**
- [ ] `EXPECT_QUATERNION_EQ` macro works
- [ ] `EXPECT_QUATERNION_NEAR` macro works

## Integration Tests

### Test 5: Clean Build Works
```bash
./build_tests.sh clean
```

**Verify:**
- [ ] Build directory is cleaned
- [ ] Fresh build succeeds
- [ ] Test executable is created

### Test 6: Run with Clean
```bash
./run_tests.sh clean
```

**Verify:**
- [ ] Clean build is triggered
- [ ] Tests run after build
- [ ] All operations complete successfully

### Test 7: CTest Integration
```bash
cd build
ctest --output-on-failure
cd ..
```

**Verify:**
- [ ] CTest finds the tests
- [ ] CTest executes the tests
- [ ] Results are displayed

## Documentation Verification

### Test 8: README is Complete
```bash
cat tests/README.md
```

**Verify:**
- [ ] Quick Start section exists
- [ ] Writing Tests section exists
- [ ] Test Helper Macros section exists
- [ ] Examples are provided

### Test 9: Quick Reference is Accessible
```bash
cat TESTING.md
```

**Verify:**
- [ ] Common commands are listed
- [ ] Macro examples are provided
- [ ] File locations are documented

## Troubleshooting

### If build_tests.sh Fails

1. Check CMake version: `cmake --version` (needs 3.14+)
2. Check internet connection (Google Test needs to download)
3. Try clean build: `./build_tests.sh clean`
4. Check CMake output for specific errors

### If Tests Don't Run

1. Check test executable exists: `ls -la build/bin/tests/physics_tests`
2. Try running directly: `./build/bin/tests/physics_tests`
3. Check for compilation errors in build output

### If Custom Macros Don't Work

1. Verify `test_helpers.h` is included
2. Check that test files include Google Test: `#include <gtest/gtest.h>`
3. Verify include paths in `tests/CMakeLists.txt`

## Sign-Off

Once all items are checked, the test infrastructure is fully operational!

**Installation Completed By:** ___________________

**Date:** ___________________

**Notes:**
_______________________________________________________
_______________________________________________________
_______________________________________________________

## Next Steps After Verification

1. Review the example tests in `test_example.cpp`
2. Read the full documentation in `tests/README.md`
3. Start writing tests for your physics components
4. Run tests regularly during development
5. Consider adding test execution to your CI/CD pipeline

---

**For Support:** See `tests/README.md` and `TESTING.md`
