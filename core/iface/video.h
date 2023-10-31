#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <core/common.h>

namespace chip8 {

struct video_system_iface_t {
    virtual ~video_system_iface_t() = default;

    virtual void render(const video_memory_t&) = 0;
};

using video_system_ptr = std::unique_ptr<video_system_iface_t>;

} // namespace chip8
