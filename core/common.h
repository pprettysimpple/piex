#pragma once

#include <cstdint>
#include <string_view>


namespace chip8 {

using bytes_view = std::basic_string_view<uint8_t>;
using bytes_owned = std::basic_string<uint8_t>;
using bytes_stream = std::basic_stringstream<uint8_t>;

inline constexpr size_t MEMORY_SIZE = 4096;
inline constexpr size_t ROM_OFFSET = 0x200;
inline constexpr size_t VIDEO_WIDTH = 64;
inline constexpr size_t VIDEO_HEIGHT = 32;
inline constexpr size_t VIDEO_SIZE = VIDEO_WIDTH * VIDEO_HEIGHT;
inline constexpr size_t STACK_SIZE = 16;
inline constexpr size_t KEYPAD_SIZE = 16;
inline constexpr size_t REGISTERS_SIZE = 16;
inline constexpr size_t KEYBOARD_SIZE = 16;

inline constexpr size_t FONTSET_SIZE = 80;

inline constexpr uint8_t CHIP8_STANDARD_FONTSET[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

inline constexpr auto CHIP8_STANDARD_FONTSET_VIEW = bytes_view(CHIP8_STANDARD_FONTSET, FONTSET_SIZE);

enum keyboard_key_t : uint8_t {
    KEY_0 = 0x0,
    KEY_1 = 0x1,
    KEY_2 = 0x2,
    KEY_3 = 0x3,
    KEY_4 = 0x4,
    KEY_5 = 0x5,
    KEY_6 = 0x6,
    KEY_7 = 0x7,
    KEY_8 = 0x8,
    KEY_9 = 0x9,
    KEY_A = 0xA,
    KEY_B = 0xB,
    KEY_C = 0xC,
    KEY_D = 0xD,
    KEY_E = 0xE,
    KEY_F = 0xF
};

struct opcode_t {
    uint16_t bytes;

    template <size_t Nibble>
    uint8_t get_nibble() const noexcept {
        static_assert(Nibble < 4, "Nimble must be in range [0, 3]");

        return (bytes >> (Nibble * 4)) & 0xF;
    }

    uint8_t get_x() const noexcept {
        return get_nibble<2>();
    }

    uint8_t get_y() const noexcept {
        return get_nibble<1>();
    }

    uint8_t get_z() const noexcept {
        return get_nibble<0>();
    }

    uint8_t get_n() const noexcept {
        return get_nibble<0>();
    }

    uint8_t get_kk() const noexcept {
        return bytes & 0xFF;
    }

    uint16_t get_nnn() const noexcept {
        return bytes & 0xFFF;
    }
};

} // namespace chip8
