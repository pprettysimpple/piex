#include "timers_basic.h"

#include <iostream>
#include <sstream>
#include <thread>

#include <core/vm.h>


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

void timers_system_basic_t::tick() {
    if (delay_timer_ > 0) {
        delay_timer_--;
    }

    if (sound_timer_ > 0) {
        sound_timer_--;
    }

    std::this_thread::sleep_for(vm_t::TIMER_DURATION);
}


} // namespace chip8
