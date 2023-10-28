#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>
#include <string_view>

#include <instructions.h>
#include <platform_impl/keyboard_fake.h>
#include <platform_impl/timers_basic.h>
#include <platform_impl/video_ascii.h>
#include <platform_impl/video_none.h>
#include <vm.h>


// filename of rom to load
int main(int argc, char** argv)
{
    if (argc != 2) {
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

    auto vm = std::make_unique<chip8::vm_t>(
        std::make_unique<chip8::keyboard_system_fake_t>(),
        std::make_unique<chip8::timers_system_basic_t>(),
        std::make_unique<chip8::video_system_none_t>()
    );

    vm->load_data(rom, chip8::ROM_OFFSET);
    vm->load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);

    vm->emulate(500);

    return 0;
}
