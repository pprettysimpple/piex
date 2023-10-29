#pragma once

#include <core/iface/random.h>


namespace chip8 {

struct random_system_crand_t : public random_system_iface_t {
    random_system_crand_t();

    uint8_t get_random_byte() override;

    ~random_system_crand_t() override = default;
};

} // namespace chip8
