#pragma once

#include <cstdlib>

#include <platform_iface/keyboard.h>


namespace chip8 {

struct keyboard_system_fake_t : keyboard_system_iface_t {
    bool is_pressed(keyboard_key_t key) const noexcept override;
    keyboard_key_t wait_for_keypress() const noexcept override;
};

} // namespace chip8
