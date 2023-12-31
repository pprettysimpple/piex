#include "timers_basic.h"

#include <iostream>
#include <sstream>
#include <thread>

#include <core/vm.h>


namespace chip8 {

void timers_system_basic_t::tick(std::chrono::nanoseconds duration) {
    std::this_thread::sleep_for(duration);
}

} // namespace chip8
