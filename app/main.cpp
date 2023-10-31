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


int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "usage: " << argv[0] << " <sdl|ascii> <ch8|sch|xoch> <rom>" << std::endl;
        return 1;
    }

    std::string_view rom_filename(argv[3]);

    // read rom from file
    chip8::bytes_owned rom = std::invoke([rom_filename]() {
        std::ifstream file(rom_filename.data(), std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file");
        }

        chip8::bytes_owned buffer(std::istreambuf_iterator<char>(file), {});
        return buffer;
    });

    auto settings = chip8::vm_t::settings_t{
        .emulator_type = std::invoke([argv]() {
            if (std::string_view(argv[2]) == "ch8") {
                return chip8::vm_t::settings_t::CHIP_8;
            } else if (std::string_view(argv[2]) == "sch") {
                return chip8::vm_t::settings_t::SCHIP1_1;
            } else if (std::string_view(argv[2]) == "xoch") {
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

    if (std::string_view(argv[1]) == "sdl") {
        run_with_sdl();
    } else if (std::string_view(argv[1]) == "ascii") {
        run_with_ascii();
    } else {
        std::cerr << "invalid frontend type" << std::endl;
        return 1;
    }

    return 0;
}
