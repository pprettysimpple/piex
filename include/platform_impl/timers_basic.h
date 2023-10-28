#pragma once

#include <chrono>

#include <common.h>

#include <platform_iface/timers.h>


namespace chip8 {

struct timers_system_basic_t : timers_system_iface_t {
    void set_delay_timer(uint8_t value) override;
    uint8_t get_delay_timer() noexcept override;
    void set_sound_timer(uint8_t value) override;

private:
    uint8_t delay_timer_ = 0;
    uint8_t sound_timer_ = 0;

    std::chrono::time_point<std::chrono::steady_clock> delay_timer_last_change;
};

} // namespace chip8
