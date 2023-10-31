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

inline constexpr auto CLS = instruction_t{
    "CLS",
    [](vm_t& vm, const opcode_t&) {
        for (auto& row : vm.video_memory) {
            std::fill(row.begin(), row.end(), false);
        }
        vm.video_system.render(vm.video_memory);
        
        vm.next_instruction();
    }
};

inline constexpr auto RET = instruction_t{
    "RET",
    [](vm_t& vm, const opcode_t&) {
        if (vm.sp == 0) {
            throw std::runtime_error("Stack underflow");
        }
        vm.pc = vm.stack[--vm.sp];
        vm.next_instruction();
    }
};

inline constexpr auto JP_ADDR = instruction_t{
    "JP_ADDR",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.pc = opcode.get_nnn();
    }
};

inline constexpr auto CALL_ADDR = instruction_t{
    "CALL_ADDR",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.sp == vm.stack.size()) {
            throw std::runtime_error("Stack overflow");
        }
        vm.stack[vm.sp++] = vm.pc;
        vm.pc = opcode.get_nnn();
    }
};

inline constexpr auto SE_VX_BYTE = instruction_t{
    "SE_VX_BYTE",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.V[opcode.get_x()] == opcode.get_kk()) {
            vm.next_instruction(); // skip next instruction
        }
        vm.next_instruction();
    }
};

inline constexpr auto SNE_VX_BYTE = instruction_t{
    "SNE_VX_BYTE",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.V[opcode.get_x()] != opcode.get_kk()) {
            vm.next_instruction();
        }
        vm.next_instruction();
    }
};

inline constexpr auto SE_VX_VY = instruction_t{
    "SE_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.V[opcode.get_x()] == vm.V[opcode.get_y()]) {
            vm.next_instruction();
        }
        vm.next_instruction();
    }
};

inline constexpr auto LD_VX_BYTE = instruction_t{
    "LD_VX_BYTE",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] = opcode.get_kk();
        vm.next_instruction();
    }
};

inline constexpr auto ADD_VX_BYTE = instruction_t{
    "ADD_VX_BYTE",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] += opcode.get_kk();
        vm.next_instruction();
    }
};

inline constexpr auto LD_VX_VY = instruction_t{
    "LD_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] = vm.V[opcode.get_y()];
        vm.next_instruction();
    }
};

inline constexpr auto OR_VX_VY = instruction_t{
    "OR_VX_VY",
    
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] |= vm.V[opcode.get_y()];

        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.V[0xF] = 0;
        }

        vm.next_instruction();
    }
};

inline constexpr auto AND_VX_VY = instruction_t{
    "AND_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] &= vm.V[opcode.get_y()];

        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.V[0xF] = 0;
        }

        vm.next_instruction();
    }
};

inline constexpr auto XOR_VX_VY = instruction_t{
    "XOR_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] ^= vm.V[opcode.get_y()];

        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.V[0xF] = 0;
        }

        vm.next_instruction();
    }
};

inline constexpr auto ADD_VX_VY = instruction_t{
    "ADD_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        // check overflow
        auto& target = vm.V[opcode.get_x()];
        auto value = vm.V[opcode.get_y()];
        uint16_t result = static_cast<uint16_t>(target) + static_cast<uint16_t>(value);
        target = result;
        if (result > 0xFF) {
            vm.V[0xF] = 1;
        } else {
            vm.V[0xF] = 0;
        }
        vm.next_instruction();
    }
};

inline constexpr auto SUB_VX_VY = instruction_t{
    "SUB_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        // check underflow
        auto& target = vm.V[opcode.get_x()];
        auto value = vm.V[opcode.get_y()];
        int16_t result = static_cast<int16_t>(target) - static_cast<int16_t>(value);
        target = static_cast<uint8_t>(result);
        if (result > 0) {
            vm.V[0xF] = 1;
        } else {
            vm.V[0xF] = 0;
        }
        vm.next_instruction();
    }
};

inline constexpr auto SHR_VX_VY = instruction_t{
    "SHR_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        uint8_t value = vm.V[opcode.get_y()];
        auto carry = value & 0x1;
        vm.V[opcode.get_x()] = value >> 1;
        vm.V[0xF] = carry;
        vm.next_instruction();
    }
};

inline constexpr auto SUBN_VX_VY = instruction_t{
    "SUBN_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        // check underflow
        auto& target = vm.V[opcode.get_x()];
        auto value = vm.V[opcode.get_y()];
        int16_t result = static_cast<int16_t>(value) - static_cast<int16_t>(target);
        target = static_cast<uint8_t>(result);
        if (result > 0) {
            vm.V[0xF] = 1;
        } else {
            vm.V[0xF] = 0;
        }
        vm.next_instruction();
    }
};

inline constexpr auto SHL_VX_VY = instruction_t{
    "SHL_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        uint8_t value = vm.V[opcode.get_y()];
        auto carry = value >> 7;
        vm.V[opcode.get_x()] = value << 1;
        vm.V[0xF] = carry;
        vm.next_instruction();
    }
};

inline constexpr auto SNE_VX_VY = instruction_t{
    "SNE_VX_VY",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.V[opcode.get_x()] != vm.V[opcode.get_y()]) {
            vm.next_instruction();
        }
        vm.next_instruction();
    }
};

inline constexpr auto LD_I_ADDR = instruction_t{
    "LD_I_ADDR",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.I = opcode.get_nnn();
        vm.next_instruction();
    }
};

inline constexpr auto JP_V0_ADDR = instruction_t{
    "JP_V0_ADDR",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.pc = opcode.get_nnn() + vm.V[0];
        } else {
            vm.pc = opcode.get_nnn() + vm.V[opcode.get_x()];
        }
    }
};

inline constexpr auto RND_VX_BYTE = instruction_t{
    "RND_VX_BYTE",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] = vm.random_system.get_random_byte() & opcode.get_kk();
        vm.next_instruction();
    }
};

inline constexpr auto DRW_VX_VY_N = instruction_t{
    "DRW_VX_VY_N",
    [](vm_t& vm, const opcode_t& opcode) {
        if (opcode.get_n() + vm.I > vm.memory.size()) {
            throw std::runtime_error("DRW_VX_VY_N: sprite out of bounds");
        }

        const auto sprite = bytes_view(vm.memory.begin() + vm.I, opcode.get_n());

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
    }
};

inline constexpr auto SKP_VX = instruction_t{
    "SKP_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        if (vm.keyboard_system.is_pressed(static_cast<keyboard_key_t>(vm.V[opcode.get_x()]))){
            vm.next_instruction(); // skip next instruction
        }
        vm.next_instruction();
    }
};

inline constexpr auto SKNP_VX = instruction_t{
    "SKNP_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        if (!vm.keyboard_system.is_pressed(static_cast<keyboard_key_t>(vm.V[opcode.get_x()]))) {
            vm.next_instruction(); // skip next instruction
        }
        vm.next_instruction();
    }
};

inline constexpr auto LD_VX_DT = instruction_t{
    "LD_VX_DT",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] = vm.delay_timer;
        vm.next_instruction();
    }
};

inline constexpr auto LD_VX_K = instruction_t{
    "LD_VX_K",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.V[opcode.get_x()] = static_cast<uint8_t>(vm.keyboard_system.wait_for_keypress());
        vm.next_instruction();
    }
};

inline constexpr auto LD_DT_VX = instruction_t{
    "LD_DT_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.delay_timer = vm.V[opcode.get_x()];
        vm.next_instruction();
    }
};

inline constexpr auto LD_ST_VX = instruction_t{
    "LD_ST_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.sound_timer = vm.V[opcode.get_x()];
        vm.next_instruction();
    }
};

inline constexpr auto ADD_I_VX = instruction_t{
    "ADD_I_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        vm.I += vm.V[opcode.get_x()];
        vm.next_instruction();
    }
};

inline constexpr auto LD_F_VX = instruction_t{
    "LD_F_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        // TODO: make setting for this quirk
        vm.I = vm.V[opcode.get_x()] * 5;
        vm.next_instruction();
    }
};

inline constexpr auto LD_B_VX = instruction_t{
    "LD_B_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        uint8_t value = vm.V[opcode.get_x()];
        vm.memory[vm.I] = value / 100;
        vm.memory[vm.I + 1] = (value / 10) % 10;
        vm.memory[vm.I + 2] = value % 10;

        vm.next_instruction();
    }
};

inline constexpr auto LD_I_VX = instruction_t{
    "LD_I_VX",
    [](vm_t& vm, const opcode_t& opcode) {
        uint8_t size = opcode.get_x();
        for (uint8_t i = 0; i <= size; ++i) {
            vm.memory[vm.I + i] = vm.V[i];
        }

        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.I += size + 1;
        }

        vm.next_instruction();
    }
};

inline constexpr auto LD_VX_I = instruction_t{
    "LD_VX_I",
    [](vm_t& vm, const opcode_t& opcode) {
        uint8_t size = opcode.get_x();
        for (uint8_t i = 0; i <= size; ++i) {
            vm.V[i] = vm.memory[vm.I + i];
        }
        
        if (vm.settings.emulator_type == vm_t::settings_t::emulator_type_t::CHIP_8) {
            vm.I += size + 1;
        }
        
        vm.next_instruction();
    }
};

} // namespace instructions
} // namespace chip8