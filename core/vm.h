#pragma once

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdint>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <string_view>

#include <core/common.h>
#include <core/iface/keyboard.h>
#include <core/iface/random.h>
#include <core/iface/sound.h>
#include <core/iface/timers.h>
#include <core/iface/video.h>


namespace chip8 {

struct vm_t {
    static inline constexpr auto OP_DURATION = std::chrono::milliseconds(2);
    static inline constexpr auto TIMER_DURATION = std::chrono::nanoseconds(16666667);

    struct settings_t {
        enum emulator_type_t {
            CHIP_8,
            SCHIP1_1,
            SCHIP_C,
            XO_CHIP,
        };

        emulator_type_t emulator_type = CHIP_8;
    };

    struct instruction_t {
        std::string_view name;

        using executor_t = void (*)(vm_t&, const opcode_t&);
        const executor_t executor;
    };

    using decoded_instruction_t = std::tuple<std::reference_wrapper<const vm_t::instruction_t>, opcode_t>;

    // settings
    settings_t settings;

    // registers
    std::array<uint8_t, REGISTERS_SIZE> V;
    uint16_t I;
    uint16_t pc;
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;

    // memory
    std::array<uint16_t, STACK_SIZE> stack;
    std::array<uint8_t, MEMORY_SIZE> memory;

    // peripherals
    keyboard_system_iface_t& keyboard_system;
    timers_system_iface_t& timers_system;
    video_system_iface_t& video_system;
    random_system_iface_t& random_system;
    sound_system_iface_t& sound_system;

    std::chrono::nanoseconds sum_exec_duration_mod_timer = std::chrono::nanoseconds::zero();

    explicit vm_t(
        settings_t&& settings,
        keyboard_system_iface_t& keyboard_system,
        timers_system_iface_t& timers_system,
        video_system_iface_t& video_system,
        random_system_iface_t& random_system,
        sound_system_iface_t& sound_system
    ) noexcept;

    vm_t(const vm_t&) = delete;
    vm_t(vm_t&&) noexcept = delete;
    vm_t& operator=(const vm_t&) = delete;
    vm_t& operator=(vm_t&&) noexcept = delete;

    void emulate_one_instruction();

    void emulate_duration(std::chrono::nanoseconds duration = std::chrono::nanoseconds::max());

    void load_data(const bytes_view data, const size_t offset) noexcept;

    void next_instruction() noexcept;

    std::optional<std::reference_wrapper<const vm_t::instruction_t>> decode_instruction(const opcode_t& opcode_bytes) const;

    decoded_instruction_t execute_instruction_pc();
    void execute_instruction_decoded(const decoded_instruction_t& decoded_instruction);
};

} // namespace chip8
