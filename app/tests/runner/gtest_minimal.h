/**
 * @file gtest_minimal.h
 * @brief Minimal Google Test implementation for embedded systems
 * 
 * This header provides a lightweight implementation of Google Test
 * macros and classes suitable for resource-constrained embedded systems.
 * It maintains API compatibility with Google Test while minimizing
 * memory footprint and removing features unsuitable for embedded use.
 * 
 * Supported Features:
 * - TEST() macro for defining tests
 * - EXPECT_ and ASSERT_ assertion macros
 * - Test fixtures via TEST_F()
 * - Custom test event listeners
 * - Test filtering (basic)
 * 
 * Removed Features:
 * - Death tests
 * - Type-parameterized tests
 * - Value-parameterized tests
 * - Exception handling
 * - Threading support
 */

#ifndef GTEST_MINIMAL_H
#define GTEST_MINIMAL_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>

// Forward declarations
namespace testing {
    class TestInfo;
    class TestSuite;
    class UnitTest;
    class TestEventListener;
    class TestEventListeners;
    class TestPartResult;
    class Test;
}

// Test result types
namespace testing {
    enum TestPartResultType {
        kSuccess,
        kNonFatalFailure,
        kFatalFailure
    };
}

// Main test macros
#define TEST(test_suite_name, test_name) \
    class test_suite_name##_##test_name##_Test : public ::testing::Test { \
    public: \
        test_suite_name##_##test_name##_Test() {} \
        void TestBody() override; \
        static ::testing::TestInfo* const test_info_; \
    }; \
    ::testing::TestInfo* const test_suite_name##_##test_name##_Test::test_info_ = \
        ::testing::UnitTest::GetInstance()->RegisterTest( \
            #test_suite_name, #test_name, \
            new test_suite_name##_##test_name##_Test()); \
    void test_suite_name##_##test_name##_Test::TestBody()

// Test fixture macro
#define TEST_F(test_fixture, test_name) \
    class test_fixture##_##test_name##_Test : public test_fixture { \
    public: \
        test_fixture##_##test_name##_Test() {} \
        void TestBody() override; \
        static ::testing::TestInfo* const test_info_; \
    }; \
    ::testing::TestInfo* const test_fixture##_##test_name##_Test::test_info_ = \
        ::testing::UnitTest::GetInstance()->RegisterTest( \
            #test_fixture, #test_name, \
            new test_fixture##_##test_name##_Test()); \
    void test_fixture##_##test_name##_Test::TestBody()

// Assertion macros
#define EXPECT_TRUE(condition) \
    if (!(condition)) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected: " #condition " is true"); \
    }

#define EXPECT_FALSE(condition) \
    if (condition) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected: " #condition " is false"); \
    }

#define EXPECT_EQ(expected, actual) \
    if (!((expected) == (actual))) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected equality"); \
    }

#define EXPECT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected inequality"); \
    }

#define EXPECT_LT(val1, val2) \
    if (!((val1) < (val2))) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected less than"); \
    }

#define EXPECT_GT(val1, val2) \
    if (!((val1) > (val2))) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Expected greater than"); \
    }

// Fatal assertions (stop test execution)
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Assertion failed: " #condition); \
        return; \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Assertion failed: " #condition " should be false"); \
        return; \
    }

#define ASSERT_EQ(expected, actual) \
    if (!((expected) == (actual))) { \
        ::testing::UnitTest::GetInstance()->AddFailure( \
            __FILE__, __LINE__, "Assertion failed: equality"); \
        return; \
    }

// Test runner macro
#define RUN_ALL_TESTS() ::testing::UnitTest::GetInstance()->Run()

// Test initialization
namespace testing {
    void InitGoogleTest(int* argc, char** argv);
}

// Core testing classes (minimal implementation)
namespace testing {
    
    /**
     * @class TestPartResult
     * @brief Represents a single test assertion result
     */
    class TestPartResult {
    public:
        TestPartResult(TestPartResultType type, const char* file, int line, const char* message)
            : type_(type), file_(file), line_(line), message_(message) {}
        
        TestPartResultType type() const { return type_; }
        const char* file_name() const { return file_; }
        int line_number() const { return line_; }
        const char* message() const { return message_; }
        bool failed() const { return type_ != kSuccess; }
        
    private:
        TestPartResultType type_;
        const char* file_;
        int line_;
        const char* message_;
    };
    
    /**
     * @class TestInfo
     * @brief Information about a single test
     */
    class TestInfo {
    public:
        const char* test_suite_name() const { return suite_name_; }
        const char* name() const { return test_name_; }
        bool should_run() const { return should_run_; }
        const TestPartResult* result() const { return result_; }
        
    private:
        friend class UnitTest;
        const char* suite_name_;
        const char* test_name_;
        bool should_run_;
        TestPartResult* result_;
    };
    
    /**
     * @class TestSuite
     * @brief Collection of related tests
     */
    class TestSuite {
    public:
        const char* name() const { return name_; }
        int test_to_run_count() const { return test_count_; }
        int successful_test_count() const { return passed_; }
        int failed_test_count() const { return failed_; }
        
    private:
        friend class UnitTest;
        const char* name_;
        int test_count_;
        int passed_;
        int failed_;
    };
    
    /**
     * @class Test
     * @brief Base class for all tests
     */
    class Test {
    public:
        virtual ~Test() {}
        virtual void TestBody() = 0;
        virtual void SetUp() {}
        virtual void TearDown() {}
    };
    
    /**
     * @class TestEventListener
     * @brief Interface for receiving test events
     */
    class TestEventListener {
    public:
        virtual ~TestEventListener() {}
        
        virtual void OnTestProgramStart(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnTestProgramEnd(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnTestIterationStart(const UnitTest& unit_test, int iteration) { (void)unit_test; (void)iteration; }
        virtual void OnTestIterationEnd(const UnitTest& unit_test, int iteration) { (void)unit_test; (void)iteration; }
        virtual void OnEnvironmentsSetUpStart(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnEnvironmentsSetUpEnd(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnEnvironmentsTearDownStart(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnEnvironmentsTearDownEnd(const UnitTest& unit_test) { (void)unit_test; }
        virtual void OnTestSuiteStart(const TestSuite& test_suite) { (void)test_suite; }
        virtual void OnTestSuiteEnd(const TestSuite& test_suite) { (void)test_suite; }
        virtual void OnTestStart(const TestInfo& test_info) { (void)test_info; }
        virtual void OnTestEnd(const TestInfo& test_info) { (void)test_info; }
        virtual void OnTestPartResult(const TestPartResult& result) { (void)result; }
    };
    
    /**
     * @class TestEventListeners
     * @brief Manages test event listeners
     */
    class TestEventListeners {
    public:
        void Append(TestEventListener* listener);
        TestEventListener* Release(TestEventListener* listener);
        TestEventListener* default_result_printer() { return default_printer_; }
        
    private:
        friend class UnitTest;
        std::vector<TestEventListener*> listeners_;
        TestEventListener* default_printer_;
    };
    
    /**
     * @class UnitTest
     * @brief Singleton test manager
     */
    class UnitTest {
    public:
        static UnitTest* GetInstance();
        
        TestInfo* RegisterTest(const char* suite_name, const char* test_name, Test* test);
        int Run();
        void AddFailure(const char* file, int line, const char* message);
        
        TestEventListeners* listeners() { return &listeners_; }
        int successful_test_count() const { return passed_count_; }
        int failed_test_count() const { return failed_count_; }
        int total_test_count() const { return total_count_; }
        
    private:
        UnitTest();
        static UnitTest* instance_;
        TestEventListeners listeners_;
        int total_count_;
        int passed_count_;
        int failed_count_;
        std::vector<TestInfo*> tests_;
        static std::vector<Test*> test_instances_;
        static bool current_test_failed_;
    };
}

#endif // GTEST_MINIMAL_H