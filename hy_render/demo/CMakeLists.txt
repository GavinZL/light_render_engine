cmake_minimum_required(VERSION 3.15)

# Demo源文件
set(DEMO_SOURCES
    main.cpp
)

# 创建可执行文件
add_executable(hy_render_demo ${DEMO_SOURCES})

# 链接渲染引擎库
target_link_libraries(hy_render_demo PRIVATE hy_render)

# 针对不同平台的配置
if(HY_RENDER_DEMO_USE_GLFW AND glfw3_FOUND)
    # 使用GLFW
    target_link_libraries(hy_render_demo PRIVATE glfw)
    target_compile_definitions(hy_render_demo PRIVATE USE_GLFW=1)
    message(STATUS "Demo will use GLFW for window management")
elseif(HY_RENDER_DEMO_USE_GLFW AND GLFW_FOUND)
    # 使用pkg-config找到的GLFW
    target_link_libraries(hy_render_demo PRIVATE ${GLFW_LIBRARIES})
    target_include_directories(hy_render_demo PRIVATE ${GLFW_INCLUDE_DIRS})
    target_compile_definitions(hy_render_demo PRIVATE USE_GLFW=1)
    message(STATUS "Demo will use GLFW (via pkg-config) for window management")
else()
    # 不使用GLFW，只做简单的控制台输出
    target_compile_definitions(hy_render_demo PRIVATE USE_GLFW=0)
    message(STATUS "Demo will run without window (console output only)")
endif()

# macOS特定配置
if(APPLE)
    # 使用Cocoa框架
    find_library(COCOA_LIBRARY Cocoa)
    if(COCOA_LIBRARY)
        target_link_libraries(hy_render_demo PRIVATE ${COCOA_LIBRARY})
    endif()
    
    # 使用IOKit框架
    find_library(IOKIT_LIBRARY IOKit)
    if(IOKIT_LIBRARY)
        target_link_libraries(hy_render_demo PRIVATE ${IOKIT_LIBRARY})
    endif()
endif()

# 设置输出目录
set_target_properties(hy_render_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(WIN32)
    # Windows特定配置
    set_target_properties(hy_render_demo PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 添加场景图演示
add_executable(scene_graph_demo scene_graph_demo.cpp)
target_link_libraries(scene_graph_demo PRIVATE hy_render)
set_target_properties(scene_graph_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加调试工具演示
add_executable(debug_tools_demo debug_tools_demo.cpp)
target_link_libraries(debug_tools_demo PRIVATE hy_render)
set_target_properties(debug_tools_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 添加高级场景管理演示
add_executable(advanced_scene_demo advanced_scene_demo.cpp)
target_link_libraries(advanced_scene_demo PRIVATE hy_render)
set_target_properties(advanced_scene_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# macOS Metal立方体演示
if(APPLE AND HY_RENDER_ENABLE_METAL)
    add_executable(metal_cube_demo metal_cube_demo.mm)
    target_link_libraries(metal_cube_demo PRIVATE hy_render)
    
    # 链接macOS框架
    find_library(COCOA_LIBRARY Cocoa)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    
    if(COCOA_LIBRARY AND METAL_LIBRARY AND QUARTZCORE_LIBRARY)
        target_link_libraries(metal_cube_demo PRIVATE 
            ${COCOA_LIBRARY}
            ${METAL_LIBRARY}
            ${QUARTZCORE_LIBRARY}
        )
    endif()
    
    set_target_properties(metal_cube_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    message(STATUS "Metal Cube Demo will be built")
endif()
