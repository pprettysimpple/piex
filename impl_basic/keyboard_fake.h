#pragma once

#include <cstdlib>

#include <core/iface/keyboard.h>


namespace chip8 {

struct keyboard_system_fake_t : keyboard_system_iface_t {
    bool is_pressed(keyboard_key_t key) override;
    keyboard_key_t wait_for_keypress() override;
};

} // namespace chip8
