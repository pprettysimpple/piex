#pragma once

#include <core/instructions.h>


namespace chip8 {

inline std::optional<std::reference_wrapper<const instruction_t>> decode_instruction(const opcode_t opcode) {
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

}