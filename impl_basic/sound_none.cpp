#include "sound_none.h"

#include <iostream>
#include <sstream>
#include <thread>

#include <core/vm.h>


namespace chip8 {

void sound_system_none_t::play_sound(std::chrono::nanoseconds duration) {}

} // namespace chip8
