#include <iostream>
#include <sstream>

#include <platform_impl/timers_basic.h>


namespace chip8 {

uint8_t timers_system_basic_t::get_delay_timer() noexcept {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - delay_timer_last_change);
    auto ticks_since_last = elapsed.count() / (1000 / 60);

    if (ticks_since_last > delay_timer_) {
        delay_timer_ = 0;
    } else {
        delay_timer_ -= ticks_since_last;
    }

    delay_timer_last_change = now;

    return delay_timer_;
}

void timers_system_basic_t::set_delay_timer(uint8_t value) {
    delay_timer_ = value;
    delay_timer_last_change = std::chrono::steady_clock::now();
}

void timers_system_basic_t::set_sound_timer(uint8_t value) {
    sound_timer_ = value; // TODO: implement
}

} // namespace chip8
