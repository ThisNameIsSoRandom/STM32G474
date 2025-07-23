#include "test_runner.h"
#include <gtest/gtest.h>
#include <iostream>

extern "C" {
    int run_all_tests(void) {
        // Initialize Google Test
        int argc = 1;
        char* argv[] = {(char*)"test_runner"};
        ::testing::InitGoogleTest(&argc, argv);
        
        // Run all tests
        return RUN_ALL_TESTS();
    }
}