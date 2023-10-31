#pragma once

#include <core/common.h>

#include <core/iface/video.h>


namespace chip8 {

struct video_system_ascii_t : video_system_iface_t {
    virtual void render(const video_memory_t& video_memory) override;

    virtual ~video_system_ascii_t() override = default;
};

} // namespace chip8
