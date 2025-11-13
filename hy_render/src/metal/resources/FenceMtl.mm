#include "FenceMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

FenceMtl::FenceMtl(void* device) : mDevice(device) {
    std::cout << "[FenceMtl] 创建Metal栅栏" << std::endl;
    mEvent = reinterpret_cast<void*>(0x700);
}

void FenceMtl::wait() {
    std::cout << "[FenceMtl] 等待栅栏" << std::endl;
    mSignaled = true;
}

void FenceMtl::reset() {
    std::cout << "[FenceMtl] 重置栅栏" << std::endl;
    mSignaled = false;
}

bool FenceMtl::isSignaled() const {
    return mSignaled;
}

ResourceHandle FenceMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mEvent;
    return handle;
}

} // namespace render
} // namespace hyengine
