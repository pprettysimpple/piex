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
#include <core/iface/timers.h>
#include <core/iface/video.h>
#include <core/instructions.h>
#include <core/vm.h>


namespace chip8 {

vm_t::vm_t(
    settings_t&& settings,
    keyboard_system_iface_t& keyboard_system,
    timers_system_iface_t& timers_system,
    video_system_iface_t& video_system
) noexcept
    : settings(std::move(settings))
    , I(0)
    , pc(ROM_OFFSET)
    , sp(0)
    , keyboard_system(keyboard_system)
    , timers_system(timers_system)
    , video_system(video_system)
{
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(V.begin(), V.end(), 0);
    std::fill(stack.begin(), stack.end(), 0);
}

void vm_t::emulate_one() {
    auto pc_copy = pc;
    const auto& [instruction_ref, opcode] = execute_instruction_pc();

    timers_system.update_timers(settings.hz);

    // std::printf("[addr][name][code] 0x%04X %s 0x%04X\n", pc_copy, instruction_ref.get().name.data(), opcode.bytes);
}

void vm_t::emulate() {
    auto target_duration = std::chrono::nanoseconds(1'000'000'000 / settings.hz);

    while (settings.max_cycles-- > 0) {
        auto start_exec_timestamp = std::chrono::high_resolution_clock::now();

        emulate_one();

        // system timers
        auto end_exec_timestamp = std::chrono::high_resolution_clock::now();
        auto exec_duration = end_exec_timestamp - start_exec_timestamp;

        auto extra_sleep_duration = target_duration - exec_duration;

        std::this_thread::sleep_for(extra_sleep_duration);
    }
}

void vm_t::load_data(const bytes_view data, const size_t offset) noexcept {
    std::copy(data.begin(), data.end(), memory.begin() + offset);
}

std::optional<std::reference_wrapper<const vm_t::instruction_t>> vm_t::decode_instruction(const vm_t::opcode_t& opcode) const {
    switch (opcode.get_nibble<3>()) {
        case 0x0: {
            switch (opcode.get_kk()) {
                case 0xE0: {
                    return std::cref(instructions::CLS);
                }
                case 0xEE: {
                    return std::cref(instructions::RET);
                }
                default: {
                    return std::nullopt;
                }
            }
        }
        case 0x1: {
            return std::cref(instructions::JP_ADDR);
        }
        case 0x2: {
            return std::cref(instructions::CALL_ADDR);
        }
        case 0x3: {
            return std::cref(instructions::SE_VX_BYTE);
        }
        case 0x4: {
            return std::cref(instructions::SNE_VX_BYTE);
        }
        case 0x5: {
            return std::cref(instructions::SE_VX_VY);
        }
        case 0x6: {
            return std::cref(instructions::LD_VX_BYTE);
        }
        case 0x7: {
            return std::cref(instructions::ADD_VX_BYTE);
        }
        case 0x8: {
            switch (opcode.get_nibble<0>()) {
                case 0x0: {
                    return std::cref(instructions::LD_VX_VY);
                }
                case 0x1: {
                    return std::cref(instructions::OR_VX_VY);
                }
                case 0x2: {
                    return std::cref(instructions::AND_VX_VY);
                }
                case 0x3: {
                    return std::cref(instructions::XOR_VX_VY);
                }
                case 0x4: {
                    return std::cref(instructions::ADD_VX_VY);
                }
                case 0x5: {
                    return std::cref(instructions::SUB_VX_VY);
                }
                case 0x6: {
                    return std::cref(instructions::SHR_VX_VY);
                }
                case 0x7: {
                    return std::cref(instructions::SUBN_VX_VY);
                }
                case 0xE: {
                    return std::cref(instructions::SHL_VX_VY);
                }
                default: {
                    return std::nullopt;
                }
            }
        }
        case 0x9: {
            return std::cref(instructions::SNE_VX_VY);
        }
        case 0xA: {
            return std::cref(instructions::LD_I_ADDR);
        }
        case 0xB: {
            return std::cref(instructions::JP_V0_ADDR);
        }
        case 0xC: {
            return std::cref(instructions::RND_VX_BYTE);
        }
        case 0xD: {
            return std::cref(instructions::DRW_VX_VY_N);
        }
        case 0xE: {
            switch (opcode.get_kk()) {
                case 0x9E: {
                    return std::cref(instructions::SKP_VX);
                }
                case 0xA1: {
                    return std::cref(instructions::SKNP_VX);
                }
                default: {
                    return std::nullopt;
                }
            }
        }
        case 0xF: {
            switch (opcode.get_kk()) {
                case 0x07: {
                    return std::cref(instructions::LD_VX_DT);
                }
                case 0x0A: {
                    return std::cref(instructions::LD_VX_K);
                }
                case 0x15: {
                    return std::cref(instructions::LD_DT_VX);
                }
                case 0x18: {
                    return std::cref(instructions::LD_ST_VX);
                }
                case 0x1E: {
                    return std::cref(instructions::ADD_I_VX);
                }
                case 0x29: {
                    return std::cref(instructions::LD_F_VX);
                }
                case 0x33: {
                    return std::cref(instructions::LD_B_VX);
                }
                case 0x55: {
                    return std::cref(instructions::LD_I_VX);
                }
                case 0x65: {
                    return std::cref(instructions::LD_VX_I);
                }
                default: {
                    return std::nullopt;
                }
            }
        }
        default: {
            return std::nullopt;
        }
    }
}

void vm_t::next_instruction() noexcept {
    pc += 2;
    pc %= MEMORY_SIZE;
}

std::tuple<std::reference_wrapper<const vm_t::instruction_t>, vm_t::opcode_t> vm_t::execute_instruction_pc() {
    uint16_t opcode_bytes = memory[pc] << 8 | memory[pc + 1];
    auto opcode = opcode_t{opcode_bytes};
    auto instruction_opt = decode_instruction(opcode);

    if (!instruction_opt) {
        std::stringstream error;
        error << "unknown opcode: 0x" << std::hex << std::setw(4) << std::setfill('0') << opcode_bytes;
        throw std::runtime_error(error.str());
    }

    const auto& instruction = instruction_opt.value().get();

    execute_instruction_decoded({instruction, opcode});

    return {std::cref(instruction), opcode};
}

void vm_t::execute_instruction_decoded(const decoded_instruction_t& decoded_instruction) {
    const auto& [instruction_ref, opcode] = decoded_instruction;
    try {
        instruction_ref.get().executor(*this, opcode);
    } catch (const std::exception& e) {
        std::stringstream error;
        error << "error while executing instruction: " << instruction_ref.get().name << std::endl;
        error << "error: " << e.what();
        
        error << "debug info:" << std::endl;
        error << "sp: 0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(sp) << std::endl;
        error << "pc: 0x" << std::hex << std::setw(4) << std::setfill('0') << pc << std::endl;
        error << "opcode: 0x" << std::hex << std::setw(4) << std::setfill('0') << opcode.bytes << std::endl;
        error << "I: 0x" << std::hex << std::setw(4) << std::setfill('0') << I << std::endl;
        error << "V: ";
        for (const auto& v : V) {
            error << std::hex << std::setw(2) << std::setfill('0') << static_cast<uint16_t>(v) << " ";
        }
        error << std::endl;
        throw;
    }
}

} // namespace chip8
