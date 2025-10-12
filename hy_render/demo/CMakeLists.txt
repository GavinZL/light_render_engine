cmake_minimum_required(VERSION 3.15)

# 查找GLFW
find_package(glfw3 QUIET)

if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found, demo will not be built")
    return()
endif()

# Demo可执行文件
add_executable(hy_render_demo
    main.cpp
)

target_link_libraries(hy_render_demo
    PRIVATE
        hy_render
        glfw
)

# 复制shader文件到构建目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
