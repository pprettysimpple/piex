#pragma once

#include <chrono>

#include <core/common.h>
#include <core/iface/timers.h>


namespace chip8 {

struct timers_system_basic_t : timers_system_iface_t {
    static inline constexpr uint64_t TARGET_HZ = 60;

    void set_delay_timer(uint8_t value) override;
    uint8_t get_delay_timer() noexcept override;
    void set_sound_timer(uint8_t value) override;

    void tick() override;

private:
    uint8_t delay_timer_ = 0;
    uint8_t sound_timer_ = 0;
};

} // namespace chip8
