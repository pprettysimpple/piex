#include <cstdio>

#include <common.h>
#include <platform_impl/keyboard_fake.h>


namespace chip8 {

bool keyboard_system_fake_t::is_pressed(keyboard_key_t key) {
    return false; // todo: implement
}

keyboard_key_t keyboard_system_fake_t::wait_for_keypress() {
    return KEY_0; // todo: implement
}

} // namespace chip8
