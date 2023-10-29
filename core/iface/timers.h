#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <core/common.h>


namespace chip8 {

struct timers_system_iface_t {
    virtual ~timers_system_iface_t() = default;

    virtual void set_delay_timer(uint8_t value) = 0;
    virtual uint8_t get_delay_timer() noexcept = 0;
    virtual void set_sound_timer(uint8_t value) = 0;

    // Indicates that sound timer should be played
    virtual bool is_sound_active() = 0;

    virtual void update_timers(uint64_t vm_hz) = 0;
};

using timers_system_ptr = std::unique_ptr<timers_system_iface_t>;

} // namespace chip8