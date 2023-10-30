#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

#include <core/common.h>


namespace chip8 {

struct sound_system_iface_t {
    virtual ~sound_system_iface_t() = default;

    virtual void play_sound(std::chrono::nanoseconds duration) = 0;
};

using sound_system_ptr = std::unique_ptr<sound_system_iface_t>;

} // namespace chip8
