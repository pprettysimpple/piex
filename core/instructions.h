#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>

#include <iostream>

#include <core/common.h>
#include <core/vm.h>


namespace chip8 {

struct instruction_t {
        std::string_view name;

        using executor_t = void (*)(vm_t&, const opcode_t&);
        const executor_t executor;
    };

using decoded_instruction_t = std::tuple<std::reference_wrapper<const instruction_t>, opcode_t>;

namespace instructions {

// some boilerplate to make instruction declaration more readable
namespace detail {
struct InstructionDeclarationHelper {
    template <typename F>
    constexpr instruction_t operator+(F&& executor) {
        return instruction_t{
            .name = name,
            .executor = executor,
        };
    }

    std::string_view name;
};
} // namespace detail

/**
 * Each instruction is a lambda that takes a vm_t reference and an opcode_t reference.
 * Intruction mutates the vm_t and advances the program counter.
 */
#define PIEX_INSTRUCTION(instruction_name)\
inline constexpr auto instruction_name = detail::InstructionDeclarationHelper{.name = #instruction_name} + [](vm_t& vm, const opcode_t& opcode)

PIEX_INSTRUCTION(CLS) {
    for (auto& row : vm.video_memory) {
        std::fill(row.begin(), row.end(), false);
    }
    vm.video_system.render(vm.video_memory);

    vm.next_instruction();
};

PIEX_INSTRUCTION(RET) {
    if (vm.sp == 0) {
        throw std::runtime_error("Stack underflow");
    }
    vm.pc = vm.stack[--vm.sp];
    vm.next_instruction();
};

PIEX_INSTRUCTION(JP_ADDR) {
    vm.pc = opcode.get_nnn();
};

PIEX_INSTRUCTION(CALL_ADDR) {
    if (vm.sp == vm.stack.size()) {
        throw std::runtime_error("Stack overflow");
    }
    vm.stack[vm.sp++] = vm.pc;
    vm.pc = opcode.get_nnn();
};

PIEX_INSTRUCTION(SE_VX_BYTE) {
    if (vm.V[opcode.get_x()] == opcode.get_kk()) {
        vm.next_instruction(); // skip next instruction
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SNE_VX_BYTE) {
    if (vm.V[opcode.get_x()] != opcode.get_kk()) {
        vm.next_instruction();
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SE_VX_VY) {
    if (vm.V[opcode.get_x()] == vm.V[opcode.get_y()]) {
        vm.next_instruction();
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_VX_BYTE) {
    vm.V[opcode.get_x()] = opcode.get_kk();
    vm.next_instruction();
};

PIEX_INSTRUCTION(ADD_VX_BYTE) {
    vm.V[opcode.get_x()] += opcode.get_kk();
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_VX_VY) {
    vm.V[opcode.get_x()] = vm.V[opcode.get_y()];
    vm.next_instruction();
};

PIEX_INSTRUCTION(OR_VX_VY) {
    vm.V[opcode.get_x()] |= vm.V[opcode.get_y()];

    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.V[0xF] = 0;
    }

    vm.next_instruction();
};

PIEX_INSTRUCTION(AND_VX_VY) {
    vm.V[opcode.get_x()] &= vm.V[opcode.get_y()];

    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.V[0xF] = 0;
    }

    vm.next_instruction();
};

PIEX_INSTRUCTION(XOR_VX_VY) {
    vm.V[opcode.get_x()] ^= vm.V[opcode.get_y()];

    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.V[0xF] = 0;
    }

    vm.next_instruction();
};

PIEX_INSTRUCTION(ADD_VX_VY) {
    // check overflow
    auto& target = vm.V[opcode.get_x()];
    auto value = vm.V[opcode.get_y()];
    uint16_t result = static_cast<uint16_t>(target) + static_cast<uint16_t>(value);
    target = static_cast<uint8_t>(result);
    if (result > 0xFF) {
        vm.V[0xF] = 1;
    }
    else {
        vm.V[0xF] = 0;
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SUB_VX_VY) {
    // check underflow
    auto& target = vm.V[opcode.get_x()];
    auto value = vm.V[opcode.get_y()];
    int16_t result = static_cast<int16_t>(target) - static_cast<int16_t>(value);
    target = static_cast<uint8_t>(result);
    if (result > 0) {
        vm.V[0xF] = 1;
    }
    else {
        vm.V[0xF] = 0;
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SHR_VX_VY) {
    uint8_t value = vm.V[opcode.get_y()];
    auto carry = value & 0x1;
    vm.V[opcode.get_x()] = value >> 1;
    vm.V[0xF] = carry;
    vm.next_instruction();
};

PIEX_INSTRUCTION(SUBN_VX_VY) {
    // check underflow
    auto& target = vm.V[opcode.get_x()];
    auto value = vm.V[opcode.get_y()];
    int16_t result = static_cast<int16_t>(value) - static_cast<int16_t>(target);
    target = static_cast<uint8_t>(result);
    if (result > 0) {
        vm.V[0xF] = 1;
    }
    else {
        vm.V[0xF] = 0;
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SHL_VX_VY) {
    uint8_t value = vm.V[opcode.get_y()];
    auto carry = value >> 7;
    vm.V[opcode.get_x()] = value << 1;
    vm.V[0xF] = carry;
    vm.next_instruction();
};

PIEX_INSTRUCTION(SNE_VX_VY) {
    if (vm.V[opcode.get_x()] != vm.V[opcode.get_y()]) {
        vm.next_instruction();
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_I_ADDR) {
    vm.I = opcode.get_nnn();
    vm.next_instruction();
};

PIEX_INSTRUCTION(JP_V0_ADDR) {
    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.pc = opcode.get_nnn() + vm.V[0];
    }
    else {
        vm.pc = opcode.get_nnn() + vm.V[opcode.get_x()];
    }
};

PIEX_INSTRUCTION(RND_VX_BYTE) {
    vm.V[opcode.get_x()] = vm.random_system.get_random_byte() & opcode.get_kk();
    vm.next_instruction();
};

PIEX_INSTRUCTION(DRW_VX_VY_N) {
    if (static_cast<size_t>(opcode.get_n() + vm.I) > vm.memory.size()) {
        throw std::runtime_error("DRW_VX_VY_N: sprite out of bounds");
    }

    const auto sprite = bytes_view(vm.memory.data() + vm.I, opcode.get_n());

    auto start_col = vm.V[opcode.get_x()] % VIDEO_WIDTH;
    auto start_row = vm.V[opcode.get_y()] % VIDEO_HEIGHT;

    bool collision = false;
    for (size_t row = 0; row < sprite.size(); ++row) {
        uint8_t sprite_byte = sprite[row];
        for (size_t col = 0; col < 8; ++col) {
            if (start_col + col >= VIDEO_WIDTH || start_row + row >= VIDEO_HEIGHT) {
                continue;
            }

            bool sprite_pixel = (sprite_byte >> (7 - col)) & 0x01;

            auto r = (start_row + row) % VIDEO_HEIGHT;
            auto c = (start_col + col) % VIDEO_WIDTH;
            bool& screen_pixel = vm.video_memory[r][c];

            if (sprite_pixel && screen_pixel) {
                collision = true;
            }
            screen_pixel ^= sprite_pixel;
        }
    }

    vm.V[0xF] = collision ? 1 : 0;

    vm.video_system.render(vm.video_memory);

    vm.next_instruction();
};

PIEX_INSTRUCTION(SKP_VX) {
    if (vm.keyboard_system.is_pressed(static_cast<keyboard_key_t>(vm.V[opcode.get_x()]))) {
        vm.next_instruction(); // skip next instruction
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(SKNP_VX) {
    if (!vm.keyboard_system.is_pressed(static_cast<keyboard_key_t>(vm.V[opcode.get_x()]))) {
        vm.next_instruction(); // skip next instruction
    }
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_VX_DT) {
    vm.V[opcode.get_x()] = vm.delay_timer;
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_VX_K) {
    vm.V[opcode.get_x()] = static_cast<uint8_t>(vm.keyboard_system.wait_for_keypress());
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_DT_VX) {
    vm.delay_timer = vm.V[opcode.get_x()];
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_ST_VX) {
    vm.sound_timer = vm.V[opcode.get_x()];
    vm.next_instruction();
};

PIEX_INSTRUCTION(ADD_I_VX) {
    vm.I += vm.V[opcode.get_x()];
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_F_VX) {
    // TODO: make setting for this quirk
    vm.I = vm.V[opcode.get_x()] * 5;
    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_B_VX) {
    uint8_t value = vm.V[opcode.get_x()];
    vm.memory[vm.I] = value / 100;
    vm.memory[static_cast<size_t>(vm.I + 1)] = (value / 10) % 10;
    vm.memory[static_cast<size_t>(vm.I + 2)] = value % 10;

    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_I_VX) {
    uint8_t size = opcode.get_x();
    for (uint8_t i = 0; i <= size; ++i) {
        vm.memory[static_cast<size_t>(vm.I + i)] = vm.V[i];
    }

    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.I += size + 1;
    }

    vm.next_instruction();
};

PIEX_INSTRUCTION(LD_VX_I) {
    uint8_t size = opcode.get_x();
    for (uint8_t i = 0; i <= size; ++i) {
        vm.V[i] = vm.memory[static_cast<size_t>(vm.I + i)];
    }

    if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
        vm.I += size + 1;
    }

    vm.next_instruction();
};

} // namespace instructions
} // namespace chip8