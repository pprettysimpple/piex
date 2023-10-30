#pragma once

#include <chrono>

#include <core/iface/sound.h>


namespace chip8 {

struct sound_system_none_t : sound_system_iface_t {
    void play_sound(std::chrono::nanoseconds duration) override;
};

} // namespace chip8
