#pragma once

#include <chrono>

#include <core/common.h>
#include <core/iface/timers.h>


namespace chip8 {

struct timers_system_instant_t : timers_system_iface_t {
    void tick(std::chrono::nanoseconds) override;
};

} // namespace chip8
