#pragma once

#include <cstdint>


namespace chip8 {

struct random_system_iface_t {
    virtual ~random_system_iface_t() = default;

    virtual uint8_t get_random_byte() = 0;
};

} // namespace chip8
