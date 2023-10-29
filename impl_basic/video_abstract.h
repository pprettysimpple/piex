#pragma once

#include <core/common.h>
#include <core/iface/video.h>


namespace chip8 {

struct video_system_abstract_t : video_system_iface_t {
    using memory_t = std::array<std::array<bool, VIDEO_WIDTH>, VIDEO_HEIGHT>;

    virtual bool draw_sprite(uint8_t x, uint8_t y, const bytes_view sprite) override;

    virtual void clear_screen() override;

    virtual void render() override;

    virtual ~video_system_abstract_t() override = default;

protected:
    virtual void render_impl() = 0;

    memory_t memory;
};

} // namespace chip8