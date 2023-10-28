#include "vm.h"
#include <gtest/gtest.h>

#include <instructions.h>
#include <memory>


// namespace {

// template <typename T>
// struct counting_callback_mock_t {};

// template <typename Ret, typename... Args>
// struct counting_callback_mock_t<std::function<Ret(Args...)>> {
//     std::shared_ptr<size_t> call_count = 0;

//     Ret operator()(Args&&... args) {
//         ++*call_count;
//         return Ret();
//     }
// };

// }


// struct keyboard_mock_t : chip8::keyboard_system_iface_t {
//     keyboard_mock_t(std::function<bool(chip8::keyboard_key_t)>* is_pressed_mock,
//                     std::function<chip8::keyboard_key_t()>* wait_for_keypress_mock) :
//         is_pressed_mock(is_pressed_mock),
//         wait_for_keypress_mock(wait_for_keypress_mock) {}

//     bool is_pressed(chip8::keyboard_key_t key) const noexcept override {
//         return (*is_pressed_mock)(key);
//     }

//     chip8::keyboard_key_t wait_for_keypress() const noexcept override {
//         return (*wait_for_keypress_mock)();
//     }

//     std::function<bool(chip8::keyboard_key_t)>* is_pressed_mock;
//     std::function<chip8::keyboard_key_t()>* wait_for_keypress_mock;
// };

// struct video_mock_t : chip8::video_system_iface_t {
//     video_mock_t(std::function<bool(uint8_t, uint8_t, const chip8::bytes_view)>* draw_sprite_mock,
//                  std::function<void()>* clear_screen_mock,
//                  std::function<void()>* render_mock) :
//         draw_sprite_mock(draw_sprite_mock),
//         clear_screen_mock(clear_screen_mock),
//         render_mock(render_mock) {}

//     bool draw_sprite(uint8_t x, uint8_t y, const chip8::bytes_view sprite) override {
//         return (*draw_sprite_mock)(x, y, sprite);
//     }

//     void clear_screen() override {
//         return (*clear_screen_mock)();
//     }

//     void render() override {
//         return (*render_mock)();
//     }

//     std::function<bool(uint8_t, uint8_t, const chip8::bytes_view)>* draw_sprite_mock;
//     std::function<void()>* clear_screen_mock;
//     std::function<void()>* render_mock;
// };

// struct env_t {
//     std::function<bool(chip8::keyboard_key_t)> is_pressed_mock;
//     std::function<chip8::keyboard_key_t()> wait_for_keypress_mock;

//     std::function<bool(uint8_t, uint8_t, const chip8::bytes_view)> draw_sprite_mock;
//     std::function<void()> clear_screen_mock;
//     std::function<void()> render_mock;

//     chip8::vm_t vm;

//     env_t() :
//         is_pressed_mock(counting_callback_mock_t<decltype(is_pressed_mock)>()),
//         wait_for_keypress_mock(counting_callback_mock_t<decltype(wait_for_keypress_mock)>()),
//         draw_sprite_mock(counting_callback_mock_t<decltype(draw_sprite_mock)>()),
//         clear_screen_mock(counting_callback_mock_t<decltype(clear_screen_mock)>()),
//         render_mock(counting_callback_mock_t<decltype(render_mock)>()),
//         vm(std::make_unique<keyboard_mock_t>(keyboard_mock_t(&is_pressed_mock, &wait_for_keypress_mock)),
//            std::make_unique<video_mock_t>(video_mock_t(&draw_sprite_mock, &clear_screen_mock, &render_mock))) {}
// };


// TEST(InstructionsSet, CLS) {
//     env_t env;

//     counting_callback_mock_t<decltype(env.clear_screen_mock)> clear_screen_mock;
//     env.clear_screen_mock = clear_screen_mock;

//     chip8::vm_t::decoded_instruction_t instruction{chip8::instructions::CLS, chip8::vm_t::opcode_t{0x00E0}};
//     env.vm.execute_instruction_decoded(instruction);

//     ASSERT_EQ(*(clear_screen_mock.call_count), 1);
// }
