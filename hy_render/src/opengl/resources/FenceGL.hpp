#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL同步栅栏对象实现
 * 
 * OpenGL中使用glFenceSync创建同步对象，用于GPU-CPU同步或GPU-GPU同步。
 * 典型使用场景：
 * 1. 等待GPU完成渲染命令
 * 2. 多线程渲染同步
 * 3. 双缓冲/三缓冲同步
 */
class FenceGL : public HYFence {
public:
    FenceGL();
    virtual ~FenceGL();

    /**
     * @brief 等待栅栏信号
     * 阻塞当前线程，直到GPU完成栅栏之前的所有命令
     */
    void wait() override;

    /**
     * @brief 重置栅栏
     * 删除旧的同步对象并创建新的
     */
    void reset() override;

    /**
     * @brief 查询是否已触发
     * @return true 如果GPU已完成栅栏之前的所有命令
     */
    bool isSignaled() const override;

    ResourceHandle getResourceHandle() const override;

private:
    void* mSync = nullptr; // GLsync对象（void*是因为不想包含OpenGL头文件）
    mutable bool mSignaled = false;
};

} // namespace render
} // namespace hyengine
