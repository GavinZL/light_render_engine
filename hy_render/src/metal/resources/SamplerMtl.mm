#include "SamplerMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

SamplerMtl::SamplerMtl(const SamplerDesc& desc, void* device) : mDesc(desc) {
    std::cout << "[SamplerMtl] 创建Metal采样器" << std::endl;
    mSamplerState = reinterpret_cast<void*>(0x400);
}

void SamplerMtl::bind(uint32_t unit) {
    std::cout << "[SamplerMtl] 绑定采样器到单元 " << unit << std::endl;
}

ResourceHandle SamplerMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mSamplerState;
    return handle;
}

} // namespace render
} // namespace hyengine
