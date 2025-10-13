#include "TestFramework.h"
#include <algorithm>
#include <sstream>
#include <future>

namespace test
{

TestFramework& TestFramework::getInstance() {
    static TestFramework instance;
    return instance;
}

void TestFramework::ASSERT_TRUE(bool condition, const std::string& message) {
    if (!condition) {
        throwAssertionError("Expected true but got false: " + message);
    }
}

void TestFramework::ASSERT_FALSE(bool condition, const std::string& message) {
    if (condition) {
        throwAssertionError("Expected false but got true: " + message);
    }
}

void TestFramework::ASSERT_EQ(int expected, int actual, const std::string& message) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << "Expected " << expected << " but got " << actual << ": " << message;
        throwAssertionError(oss.str());
    }
}

void TestFramework::ASSERT_EQ(size_t expected, size_t actual, const std::string& message) {
    if (expected != actual) {
        std::ostringstream oss;
        oss << "Expected " << expected << " but got " << actual << ": " << message;
        throwAssertionError(oss.str());
    }
}

void TestFramework::ASSERT_EQ(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        throwAssertionError("Expected '" + expected + "' but got '" + actual + "': " + message);
    }
}

void TestFramework::ASSERT_NEAR(double expected, double actual, double tolerance, const std::string& message) {
    double diff = std::abs(expected - actual);
    if (diff > tolerance) {
        std::ostringstream oss;
        oss << "Expected " << expected << " but got " << actual 
            << " (difference " << diff << " > tolerance " << tolerance << "): " << message;
        throwAssertionError(oss.str());
    }
}

void TestFramework::ASSERT_TIMEOUT(std::function<void()> func, std::chrono::milliseconds timeout, const std::string& message) {
    auto future = std::async(std::launch::async, func);
    auto status = future.wait_for(timeout);
    
    if (status == std::future_status::timeout) {
        throwAssertionError("Function timed out after " + std::to_string(timeout.count()) + "ms: " + message);
    }
    
    try {
        future.get(); // 检查是否有异常
    } catch (const std::exception& e) {
        throwAssertionError("Function threw exception: " + std::string(e.what()) + ", " + message);
    }
}

void TestFramework::BENCHMARK(const std::string& name, std::function<void()> func, int iterations) {
    std::cout << "Running benchmark: " << name << " (" << iterations << " iterations)" << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double avgTime = static_cast<double>(duration.count()) / iterations;
    std::cout << "Benchmark " << name << ": " 
              << "Total: " << duration.count() << "μs, "
              << "Average: " << avgTime << "μs per iteration" << std::endl;
}

void TestFramework::CONCURRENT_TEST(const std::string& name, std::function<void()> func, int threadCount) {
    std::cout << "Running concurrent test: " << name << " (" << threadCount << " threads)" << std::endl;
    
    std::vector<std::thread> threads;
    std::vector<std::exception_ptr> exceptions(threadCount);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&func, &exceptions, i]() {
            try {
                func();
            } catch (...) {
                exceptions[i] = std::current_exception();
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 检查是否有异常
    for (int i = 0; i < threadCount; ++i) {
        if (exceptions[i]) {
            try {
                std::rethrow_exception(exceptions[i]);
            } catch (const std::exception& e) {
                throwAssertionError("Thread " + std::to_string(i) + " threw exception: " + e.what());
            }
        }
    }
    
    std::cout << "Concurrent test " << name << " completed in " << duration.count() << "ms" << std::endl;
}

void TestFramework::addTest(const std::string& name, std::function<void()> testFunc) {
    mTests.emplace_back(name, testFunc);
}

void TestFramework::runTest(const std::string& name, std::function<void()> testFunc) {
    TestResult result;
    result.testName = name;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        testFunc();
        result.passed = true;
    } catch (const AssertionError& e) {
        result.passed = false;
        result.errorMessage = e.what();
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = "Unexpected exception: " + std::string(e.what());
    } catch (...) {
        result.passed = false;
        result.errorMessage = "Unknown exception";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    mStats.totalTests++;
    if (result.passed) {
        mStats.passedTests++;
        std::cout << "✓ " << name << " (" << result.duration.count() << "ms)" << std::endl;
    } else {
        mStats.failedTests++;
        std::cout << "✗ " << name << " (" << result.duration.count() << "ms): " << result.errorMessage << std::endl;
    }
    
    mStats.totalDuration += result.duration;
    mStats.results.push_back(result);
}

void TestFramework::runAllTests() {
    std::cout << "Running " << mTests.size() << " tests..." << std::endl;
    
    for (const auto& test : mTests) {
        runTest(test.first, test.second);
    }
    
    printSummary();
}

void TestFramework::printSummary() const {
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << mStats.totalTests << std::endl;
    std::cout << "Passed: " << mStats.passedTests << std::endl;
    std::cout << "Failed: " << mStats.failedTests << std::endl;
    std::cout << "Success rate: " << (mStats.totalTests > 0 ? (100.0 * mStats.passedTests / mStats.totalTests) : 0.0) << "%" << std::endl;
    std::cout << "Total time: " << mStats.totalDuration.count() << "ms" << std::endl;
    
    if (mStats.failedTests > 0) {
        std::cout << "\nFailed tests:" << std::endl;
        for (const auto& result : mStats.results) {
            if (!result.passed) {
                std::cout << "  - " << result.testName << ": " << result.errorMessage << std::endl;
            }
        }
    }
}

void TestFramework::reset() {
    mStats = TestStatistics();
    mTests.clear();
}

void TestFramework::throwAssertionError(const std::string& message) {
    throw AssertionError(message);
}

} // namespace test