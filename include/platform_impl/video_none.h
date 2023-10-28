#pragma once

#include <common.h>

#include <platform_impl/video_abstract.h>


namespace chip8 {

struct video_system_none_t : video_system_abstract_t {
    void render() override;
};

} // namespace chip8
