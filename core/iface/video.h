#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <core/common.h>

namespace chip8 {

struct video_system_iface_t {
    virtual ~video_system_iface_t() = default;

    virtual bool draw_sprite(uint8_t x, uint8_t y, const bytes_view sprite) = 0;

    virtual void clear_screen() = 0;

    virtual void render() = 0;
};

using video_system_ptr = std::unique_ptr<video_system_iface_t>;

} // namespace chip8
