#pragma once

#include <core/common.h>

#include <impl_basic/video_abstract.h>


namespace chip8 {

struct video_system_ascii_t : video_system_abstract_t {
    video_system_ascii_t();

    virtual void render_impl() override;

    virtual ~video_system_ascii_t() override = default;
};

} // namespace chip8
