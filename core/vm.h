#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string_view>

#include <core/common.h>
#include <core/iface/keyboard.h>
#include <core/iface/random.h>
#include <core/iface/timers.h>
#include <core/iface/video.h>


namespace chip8 {

struct vm_t {
    struct settings_t {
        enum emulator_type_t {
            CHIP_8,
            SCHIP1_1,
            SCHIP_C,
            XO_CHIP,
        };

        emulator_type_t emulator_type = CHIP_8;
        uint64_t hz = 500;
        uint64_t max_cycles = std::numeric_limits<uint64_t>::max();      
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

    struct instruction_t {
        std::string_view name;

        using executor_t = void (*)(vm_t&, const vm_t::opcode_t&);
        const executor_t executor;
    };

    using decoded_instruction_t = std::tuple<std::reference_wrapper<const vm_t::instruction_t>, vm_t::opcode_t>;

    // settings
    settings_t settings;

    // registers
    std::array<uint8_t, REGISTERS_SIZE> V;
    uint16_t I;
    uint16_t pc;
    uint8_t sp;

    // memory
    std::array<uint16_t, STACK_SIZE> stack;
    std::array<uint8_t, MEMORY_SIZE> memory;

    // peripherals
    keyboard_system_iface_t& keyboard_system;
    timers_system_iface_t& timers_system;
    video_system_iface_t& video_system;
    random_system_iface_t& random_system;


    explicit vm_t(
        settings_t&& settings,
        keyboard_system_iface_t& keyboard_system,
        timers_system_iface_t& timers_system,
        video_system_iface_t& video_system,
        random_system_iface_t& random_system
    ) noexcept;

    vm_t(const vm_t&) = delete;
    vm_t(vm_t&&) noexcept = delete;
    vm_t& operator=(const vm_t&) = delete;
    vm_t& operator=(vm_t&&) noexcept = delete;

    void next_instruction() noexcept;

    void emulate_one();

    void emulate();

    void load_data(const bytes_view data, const size_t offset) noexcept;

    std::optional<std::reference_wrapper<const vm_t::instruction_t>> decode_instruction(const vm_t::opcode_t& opcode_bytes) const;

    decoded_instruction_t execute_instruction_pc();
    void execute_instruction_decoded(const decoded_instruction_t& decoded_instruction);
};

} // namespace chip8
