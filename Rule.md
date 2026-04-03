1. 所有的修改都只在hy_render目录的文件， display目录只是作为参考
2. 在回答问题或解决问题之前，都需要熟悉整个工程，知道公共组件或工具(如数学库math， 通用工具如 GLUtils等)
3. 在需要定义常量、枚举 、 全局结构体 以及 全局方法工具时，需要放在公共文件中，形成模块化
4. 整个项目的命名风格 采用 驼峰命名方式
5. 整个项目在代码设计编写中 遵循SOLID原则，必要时采用设计模式进行设计
6. 采用c++17 标准进行, 在内存管理上统一采用智能指针
7. 创建新文件时 都需要 添加文件注释， 说明文件的作用 ， 如：HYMath.hpp文件
    /**
    * @file HYMath.hpp
    * @brief HY引擎数学库主头文件
    * 
    * 包含向量、矩阵、四元数及相关数学工具函数
    * 支持模板化设计，可用于float和double类型
    */
8. 头文件包含，统一由hy_render作为根目录， 如 RenderContextGL.hpp 中应该这样包含 #include "src/gl_common/RenderContextGLBase.hpp" 头文件
