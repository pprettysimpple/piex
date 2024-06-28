#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>

#include <core/vm.h>

#include <impl_basic/keyboard_fake.h>
#include <impl_basic/timers_basic.h>
#include <impl_basic/video_ascii.h>
#include <impl_basic/sound_none.h>

#include <impl_sdl/platform.h>

#undef main
int main(int argc, char** argv)
{
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " <sdl|ascii> <ch8|sch|xoch> <rom>" << std::endl;
        return 1;
    }

    auto renderer = std::string_view(argv[1]);
    auto emulator_type = std::string_view(argv[2]);
    auto rom_filename = std::string_view(argv[3]);

    // read rom from file
    auto rom = std::invoke([rom_filename]() {
        std::ifstream file(rom_filename.data(), std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file");
        }

        return chip8::bytes_owned(std::istreambuf_iterator<char>(file), {});
    });

    auto settings = chip8::vm_t::settings_t{
        .emulator_type = std::invoke([emulator_type]() {
            if (emulator_type == "ch8") {
                return chip8::vm_t::settings_t::CHIP_8;
            } else if (emulator_type == "sch") {
                return chip8::vm_t::settings_t::SCHIP1_1;
            } else if (emulator_type == "xoch") {
                return chip8::vm_t::settings_t::XO_CHIP;
            } else {
                std::cerr << "invalid emulator type" << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }),
    };

    auto run_with_sdl = [settings, &rom]() mutable {
        auto sdl_impl = std::make_unique<chip8::sdl::sdl_system_facade_t>();

        auto vm = std::make_unique<chip8::vm_t>(
            std::move(settings),
            *sdl_impl,
            *sdl_impl,
            *sdl_impl,
            *sdl_impl,
            *sdl_impl
        );

        vm->load_data(rom, chip8::ROM_OFFSET);
        vm->load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);

        vm->emulate_duration();
    };

    auto run_with_ascii = [settings, &rom]() mutable {
        auto keyboard_system = std::make_unique<chip8::keyboard_system_fake_t>();
        auto timers_system = std::make_unique<chip8::timers_system_basic_t>();
        auto video_system = std::make_unique<chip8::video_system_ascii_t>();
        auto random_system = std::make_unique<chip8::random_system_crand_t>();
        auto sound_system = std::make_unique<chip8::sound_system_none_t>();

        auto vm = std::make_unique<chip8::vm_t>(
            std::move(settings),
            *keyboard_system,
            *timers_system,
            *video_system,
            *random_system,
            *sound_system
        );

        vm->load_data(rom, chip8::ROM_OFFSET);
        vm->load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);

        vm->emulate_duration();
    };

    if (renderer == "sdl") {
        run_with_sdl();
    } else if (renderer == "ascii") {
        run_with_ascii();
    } else {
        std::cerr << "invalid frontend type" << std::endl;
        return 1;
    }

    return 0;
}
