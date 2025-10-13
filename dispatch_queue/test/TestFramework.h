// 简单测试框架
#ifndef __TEST_FRAMEWORK_H__
#define __TEST_FRAMEWORK_H__

#include <functional>
#include <string>
#include <chrono>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <exception>

namespace test
{

// 测试结果
struct TestResult {
    std::string testName;
    bool passed;
    std::string errorMessage;
    std::chrono::milliseconds duration;
};

// 测试统计
struct TestStatistics {
    int totalTests = 0;
    int passedTests = 0;
    int failedTests = 0;
    std::chrono::milliseconds totalDuration{0};
    std::vector<TestResult> results;
};

// 测试框架类
class TestFramework {
public:
    static TestFramework& getInstance();
    
    // 基础断言
    static void ASSERT_TRUE(bool condition, const std::string& message = "");
    static void ASSERT_FALSE(bool condition, const std::string& message = "");
    static void ASSERT_EQ(int expected, int actual, const std::string& message = "");
    static void ASSERT_EQ(size_t expected, size_t actual, const std::string& message = "");
    static void ASSERT_EQ(const std::string& expected, const std::string& actual, const std::string& message = "");
    static void ASSERT_NEAR(double expected, double actual, double tolerance, const std::string& message = "");
    
    // 超时测试
    static void ASSERT_TIMEOUT(std::function<void()> func, std::chrono::milliseconds timeout, const std::string& message = "");
    
    // 异常测试
    template<typename ExceptionType>
    static void ASSERT_THROWS(std::function<void()> func, const std::string& message = "") {
        try {
            func();
            throwAssertionError("Expected exception but none was thrown: " + message);
        } catch (const ExceptionType& e) {
            // 预期的异常类型，测试通过
        } catch (const std::exception& e) {
            throwAssertionError("Expected different exception type: " + std::string(e.what()) + ", " + message);
        } catch (...) {
            throwAssertionError("Expected different exception type: " + message);
        }
    }
    
    // 性能测试
    static void BENCHMARK(const std::string& name, std::function<void()> func, int iterations = 1000);
    
    // 并发测试
    static void CONCURRENT_TEST(const std::string& name, std::function<void()> func, int threadCount = 10);
    
    // 测试用例管理
    void addTest(const std::string& name, std::function<void()> testFunc);
    void runTest(const std::string& name, std::function<void()> testFunc);
    void runAllTests();
    
    // 获取测试统计
    const TestStatistics& getStatistics() const { return mStats; }
    void printSummary() const;
    
    // 清除统计
    void reset();

private:
    TestFramework() = default;
    ~TestFramework() = default;
    
    // 禁用拷贝和赋值
    TestFramework(const TestFramework&) = delete;
    TestFramework& operator=(const TestFramework&) = delete;
    
    static void throwAssertionError(const std::string& message);
    
private:
    TestStatistics mStats;
    std::vector<std::pair<std::string, std::function<void()>>> mTests;
};

// 异常类
class AssertionError : public std::runtime_error {
public:
    explicit AssertionError(const std::string& message) : std::runtime_error(message) {}
};

// 便利宏定义
#define TEST_CASE(name) \
    void test_##name(); \
    struct TestRegistrar_##name { \
        TestRegistrar_##name() { \
            TestFramework::getInstance().addTest(#name, test_##name); \
        } \
    }; \
    static TestRegistrar_##name registrar_##name; \
    void test_##name()

} // namespace test

#endif // __TEST_FRAMEWORK_H__