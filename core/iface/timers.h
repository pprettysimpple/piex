#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

#include <core/common.h>


namespace chip8 {

struct timers_system_iface_t {
    virtual ~timers_system_iface_t() = default;

    virtual void tick(std::chrono::nanoseconds duration) = 0;
};

using timers_system_ptr = std::unique_ptr<timers_system_iface_t>;

} // namespace chip8
