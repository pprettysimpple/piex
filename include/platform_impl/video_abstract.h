#pragma once

#include <common.h>
#include <platform_iface/video.h>


namespace chip8 {

struct video_system_abstract_t : video_system_iface_t {
    using memory_t = std::array<std::array<bool, VIDEO_WIDTH>, VIDEO_HEIGHT>;

    virtual bool draw_sprite(uint8_t x, uint8_t y, const bytes_view sprite) override;

    virtual void clear_screen() override;

    virtual ~video_system_abstract_t() override = default;

    memory_t memory;
};

} // namespace chip8
