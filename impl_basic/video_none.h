#pragma once

#include <core/common.h>

#include <impl_basic/video_abstract.h>


namespace chip8 {

struct video_system_none_t : video_system_abstract_t {
    void render_impl() override;
};

} // namespace chip8
