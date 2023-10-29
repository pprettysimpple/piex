#include <bits/chrono.h>
#include <iostream>
#include <sstream>

#include <platform_impl/timers_basic.h>


namespace chip8 {

void timers_system_basic_t::set_delay_timer(uint8_t value) {
    delay_timer_ = value;
}

uint8_t timers_system_basic_t::get_delay_timer() noexcept {
    return delay_timer_;
}

void timers_system_basic_t::set_sound_timer(uint8_t value) {
    sound_timer_ = value;
}

bool timers_system_basic_t::is_sound_active() {
    return sound_timer_ > 0;
}

void timers_system_basic_t::update_timers(uint64_t vm_hz) {
    if (ticks_ % (vm_hz / TARGET_HZ) != 0) {
        ticks_++;
        return;
    }

    ticks_++;

    if (delay_timer_ > 0) {
        delay_timer_--;
    }

    if (sound_timer_ > 0) {
        sound_timer_--;
    }
}


} // namespace chip8
