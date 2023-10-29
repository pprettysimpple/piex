#include <iostream>
#include <fstream>
#include <limits>
#include <memory>
#include <sstream>
#include <string_view>

#include <instructions.h>
#include <platform_impl/keyboard_fake.h>
#include <platform_impl/timers_basic.h>
#include <platform_impl/video_ascii.h>
#include <platform_impl/video_none.h>
#include <vm.h>

#include <sdl_platform.h>


// filename of rom to load and optional max cycles to emulate
int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <rom>" << std::endl;
        return 1;
    }

    std::string_view rom_filename(argv[1]);

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
        .emulator_type = chip8::vm_t::settings_t::CHIP_8,
        .hz = 500,
        .max_cycles = std::numeric_limits<uint64_t>::max(),
    };

#if ENABLE_SDL
    auto sdl_impl = std::make_unique<chip8::sdl::sdl_system_facade_t>();

    auto vm = std::make_unique<chip8::vm_t>(
        std::move(settings),
        *sdl_impl,
        *sdl_impl,
        *sdl_impl
    );
#else
    auto keyboard_system = std::make_unique<chip8::keyboard_system_fake_t>();
    auto timers_system = std::make_unique<chip8::timers_system_basic_t>();
    auto video_system = std::make_unique<chip8::video_system_ascii_t>();

    auto vm = std::make_unique<chip8::vm_t>(
        std::move(settings),
        *keyboard_system,
        *timers_system,
        *video_system
    );
#endif

    vm->load_data(rom, chip8::ROM_OFFSET);
    vm->load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);

    vm->emulate();

    return 0;
}
