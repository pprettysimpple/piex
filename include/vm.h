#pragma once

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#include <common.h>
#include <platform_iface/keyboard.h>
#include <platform_iface/timers.h>
#include <platform_iface/video.h>


namespace chip8 {

struct vm_t {
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


    explicit vm_t(
        keyboard_system_iface_t& keyboard_system,
        timers_system_iface_t& timers_system,
        video_system_iface_t& video_system
    ) noexcept;

    vm_t(const vm_t&) = delete;
    vm_t(vm_t&&) noexcept = delete;
    vm_t& operator=(const vm_t&) = delete;
    vm_t& operator=(vm_t&&) noexcept = delete;

    void next_instruction() noexcept;

    void emulate_one();

    void emulate(const uint64_t hz, uint64_t max_cycles);

    void load_data(const bytes_view data, const size_t offset) noexcept;

    std::optional<std::reference_wrapper<const vm_t::instruction_t>> decode_instruction(const vm_t::opcode_t& opcode_bytes) const;

    decoded_instruction_t execute_instruction_pc();
    void execute_instruction_decoded(const decoded_instruction_t& decoded_instruction);
};

} // namespace chip8
