#!/bin/bash

# 运行所有测试的脚本

echo "Running all dispatch_queue tests..."

# 获取脚本所在目录
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# 查找所有测试可执行文件
TEST_EXECUTABLES=($(find "$SCRIPT_DIR" -maxdepth 1 -type f -executable -name "Test*"))

if [ ${#TEST_EXECUTABLES[@]} -eq 0 ]; then
    echo "No test executables found!"
    exit 1
fi

echo "Found ${#TEST_EXECUTABLES[@]} test executables:"
for test_exe in "${TEST_EXECUTABLES[@]}"; do
    echo "  - $(basename "$test_exe")"
done

# 运行所有测试
FAILED_TESTS=0
PASSED_TESTS=0

for test_exe in "${TEST_EXECUTABLES[@]}"; do
    test_name=$(basename "$test_exe")
    echo ""
    echo "========================================"
    echo "Running $test_name"
    echo "========================================"
    
    # 运行测试
    "$test_exe"
    exit_code=$?
    
    if [ $exit_code -eq 0 ]; then
        echo "✓ $test_name PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "✗ $test_name FAILED (exit code: $exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
done

echo ""
echo "========================================"
echo "Test Summary:"
echo "  Passed: $PASSED_TESTS"
echo "  Failed: $FAILED_TESTS"
echo "  Total:  ${#TEST_EXECUTABLES[@]}"
echo "========================================"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "All tests passed! ✓"
    exit 0
else
    echo "Some tests failed! ✗"
    exit 1
fi