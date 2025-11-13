# HY Render Engine Tests

# 数学库基础测试
add_executable(test_math_basic test_math_basic.cpp)
target_link_libraries(test_math_basic hy_render)

# 添加测试
add_test(NAME MathBasicTest COMMAND test_math_basic)
