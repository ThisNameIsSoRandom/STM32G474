/**
 * @file gtest_minimal.cpp
 * @brief Minimal Google Test implementation for embedded systems
 */

#include "gtest_minimal.h"
#include "SEGGER_RTT.h"
#include <vector>

namespace testing {

// Static instance
UnitTest* UnitTest::instance_ = nullptr;

// Initialize Google Test
void InitGoogleTest(int* argc, char** argv) {
    (void)argc; (void)argv; // Unused parameters
    UnitTest::GetInstance(); // Initialize singleton
}

// UnitTest implementation
UnitTest::UnitTest() : total_count_(0), passed_count_(0), failed_count_(0) {
    // Initialize default printer
    listeners_.default_printer_ = nullptr;
}

UnitTest* UnitTest::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = new UnitTest();
    }
    return instance_;
}

TestInfo* UnitTest::RegisterTest(const char* suite_name, const char* test_name, Test* test) {
    TestInfo* info = new TestInfo();
    info->suite_name_ = suite_name;
    info->test_name_ = test_name;
    info->should_run_ = true;
    info->result_ = nullptr;
    
    tests_.push_back(info);
    total_count_++;
    
    // Store test instance (simplified - in real implementation this would be more complex)
    test_instances_.push_back(test);
    
    return info;
}

int UnitTest::Run() {
    // Notify listeners that program is starting
    for (auto listener : listeners_.listeners_) {
        listener->OnTestProgramStart(*this);
    }
    
    // Run each test
    for (size_t i = 0; i < tests_.size(); ++i) {
        TestInfo* test_info = tests_[i];
        Test* test_instance = test_instances_[i];
        
        // Notify listeners that test is starting
        for (auto listener : listeners_.listeners_) {
            listener->OnTestStart(*test_info);
        }
        
        // Reset failure state
        current_test_failed_ = false;
        
        // Run the test (no exception handling since exceptions are disabled)
        test_instance->SetUp();
        test_instance->TestBody();
        test_instance->TearDown();
        
        // Create result
        TestPartResultType result_type = current_test_failed_ ? kFatalFailure : kSuccess;
        test_info->result_ = new TestPartResult(result_type, "", 0, "");
        
        if (current_test_failed_) {
            failed_count_++;
        } else {
            passed_count_++;
        }
        
        // Notify listeners that test is ending
        for (auto listener : listeners_.listeners_) {
            listener->OnTestEnd(*test_info);
        }
    }
    
    // Notify listeners that program is ending
    for (auto listener : listeners_.listeners_) {
        listener->OnTestProgramEnd(*this);
    }
    
    return failed_count_;
}

void UnitTest::AddFailure(const char* file, int line, const char* message) {
    current_test_failed_ = true;
    
    TestPartResult result(kNonFatalFailure, file, line, message);
    
    // Notify listeners
    for (auto listener : listeners_.listeners_) {
        listener->OnTestPartResult(result);
    }
}

// TestEventListeners implementation
void TestEventListeners::Append(TestEventListener* listener) {
    listeners_.push_back(listener);
}

TestEventListener* TestEventListeners::Release(TestEventListener* listener) {
    auto it = std::find(listeners_.begin(), listeners_.end(), listener);
    if (it != listeners_.end()) {
        listeners_.erase(it);
        return listener;
    }
    return nullptr;
}

// Global state for current test failure
bool UnitTest::current_test_failed_ = false;
std::vector<Test*> UnitTest::test_instances_;

} // namespace testing