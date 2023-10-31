#pragma once

#include <core/common.h>

#include <core/iface/video.h>


namespace chip8 {

struct video_system_none_t : video_system_iface_t {
    void render(const video_memory_t&) override;
};

} // namespace chip8
