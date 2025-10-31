/*
 * test_main.cpp
 *
 *  Created on: 2025-01-31
 *      Author: Test Infrastructure
 *
 *  Main entry point for Google Test suite
 */

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    // Run all tests
    return RUN_ALL_TESTS();
}
