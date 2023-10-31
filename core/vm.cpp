#include <cstdio>
#include <functional>
#include <optional>
#include <sstream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include <core/common.h>
#include <core/iface/keyboard.h>
#include <core/iface/random.h>
#include <core/iface/timers.h>
#include <core/iface/video.h>
#include <core/instructions.h>
#include <core/instruction_decoder.h>
#include <core/vm.h>


namespace chip8 {

namespace {

void wrap_instruction_execution(vm_t& vm, const decoded_instruction_t& decoded_instruction) {
    const auto& [instruction_ref, opcode] = decoded_instruction;
    try {
        instruction_ref.get().executor(vm, opcode);
    } catch (const std::exception& e) {
        std::stringstream error;
        error << "error while executing instruction: " << instruction_ref.get().name << std::endl;
        error << "error: " << e.what();
        
        error << "debug info:" << std::endl;
        error << "sp: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(vm.sp) << std::endl;
        error << "pc: 0x" << std::hex << std::setw(4) << std::setfill('0') << vm.pc << std::endl;
        error << "opcode: 0x" << std::hex << std::setw(4) << std::setfill('0') << opcode.bytes << std::endl;
        error << "I: 0x" << std::hex << std::setw(4) << std::setfill('0') << vm.I << std::endl;
        error << "V: ";
        for (const auto& v : vm.V) {
            error << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(v) << " ";
        }
        error << std::endl;
        throw;
    }
}

}


vm_t::vm_t(
    settings_t&& settings,
    keyboard_system_iface_t& keyboard_system,
    timers_system_iface_t& timers_system,
    video_system_iface_t& video_system,
    random_system_iface_t& random_system,
    sound_system_iface_t& sound_system
) noexcept
    : settings(std::move(settings))
    , keyboard_system(keyboard_system)
    , timers_system(timers_system)
    , video_system(video_system)
    , random_system(random_system)
    , sound_system(sound_system)
{
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(V.begin(), V.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);

    video_system.clear_screen();
}

void vm_t::emulate_one_instruction() {
    uint16_t opcode_bytes = memory[pc] << 8 | memory[pc + 1];
    auto opcode = opcode_t{opcode_bytes};
    auto instruction_opt = decode_instruction(opcode);

    if (!instruction_opt) {
        std::stringstream error;
        error << "unknown opcode: 0x" << std::hex << std::setw(4) << std::setfill('0') << opcode_bytes;
        throw std::runtime_error(error.str());
    }

    const auto& instruction = instruction_opt.value().get();

    wrap_instruction_execution(*this, {instruction, opcode});

    timers_duration += settings.op_duration;

    auto play_sound_duration = std::chrono::nanoseconds::zero();
    while (timers_duration >= settings.timer_duration) {
        timers_duration -= settings.timer_duration;
        play_sound_duration += settings.timer_duration;

        delay_timer = delay_timer > 0 ? delay_timer - 1 : 0;
        sound_timer = sound_timer > 0 ? sound_timer - 1 : 0;
        timers_system.tick(settings.timer_duration);
    }

    sound_system.play_sound(play_sound_duration);
}

void vm_t::emulate_duration(std::chrono::nanoseconds target_duration) {
    while (target_duration >= settings.op_duration) {
        target_duration -= settings.op_duration;
        emulate_one_instruction();
    }
}

void vm_t::load_data(const bytes_view data, const size_t offset) noexcept {
    std::copy(data.begin(), data.end(), memory.begin() + offset);
}

void vm_t::next_instruction() noexcept {
    pc += 2;
    pc %= MEMORY_SIZE;
}

} // namespace chip8
