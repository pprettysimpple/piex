#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <common.h>


namespace chip8 {

struct keyboard_system_iface_t {
    virtual ~keyboard_system_iface_t() = default;

    virtual bool is_pressed(keyboard_key_t key) const noexcept = 0;
    virtual keyboard_key_t wait_for_keypress() const noexcept = 0;
};

using keyboard_system_ptr = std::unique_ptr<keyboard_system_iface_t>;

} // namespace chip8
