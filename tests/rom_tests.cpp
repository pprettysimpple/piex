#include <chrono>
#include <cstdio>
#include <filesystem>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <core/common.h>
#include <core/instructions.h>
#include <core/vm.h>

#include <impl_basic/random_crand.h>
#include <impl_basic/timers_basic.h>
#include <impl_basic/video_none.h>



struct keyboard_mock_t : chip8::keyboard_system_iface_t {
    bool is_pressed(chip8::keyboard_key_t key) override {
        return pressed_keys[static_cast<size_t>(key)];
    }

    chip8::keyboard_key_t wait_for_keypress() override {
        return chip8::keyboard_key_t::KEY_0; // FIXME: implement
    }

    std::bitset<16> pressed_keys;
};

struct video_system_mock_t : chip8::video_system_none_t {
    std::string render_for_test() {
        std::stringstream frame;
        
        for (size_t i = 0; i < chip8::VIDEO_HEIGHT; ++i) {
            for (size_t j = 0; j < chip8::VIDEO_WIDTH; ++j) {
                frame << (memory[i][j] ? '#' : '.');
            }
            frame << '\n';
        }

        return frame.str();
    }

    const memory_t& get_memory() const noexcept {
        return memory;
    }
};

struct env_t {
    std::unique_ptr<keyboard_mock_t> keyboard_system = std::make_unique<keyboard_mock_t>();
    std::unique_ptr<chip8::timers_system_basic_t> timers_system = std::make_unique<chip8::timers_system_basic_t>();
    std::unique_ptr<video_system_mock_t> video_system = std::make_unique<video_system_mock_t>();
    std::unique_ptr<chip8::random_system_crand_t> random_system = std::make_unique<chip8::random_system_crand_t>();

    chip8::vm_t vm;

    env_t(chip8::vm_t::settings_t settings = {}) 
        : vm(
            std::move(settings),
            *keyboard_system,
            *timers_system,
            *video_system,
            *random_system
        )
    {
        vm.load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);
    }

    video_system_mock_t& get_video_system() noexcept {
        return *video_system;
    }

    keyboard_mock_t& get_keyboard() noexcept {
        return *keyboard_system;
    }
};


chip8::bytes_owned load_rom(std::string_view filename) {
    std::ifstream file(filename.data(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::stringstream err;
        err << "failed to open file: " << filename;
        err << " (" << std::filesystem::current_path() << ")";

        throw std::runtime_error(err.str());
    }

    chip8::bytes_owned buffer(std::istreambuf_iterator<char>(file), {});
    return buffer;
}

void launch_rom(std::string_view filename, std::string expected, std::chrono::nanoseconds duration) {
    auto rom_bytes = load_rom(filename);

    env_t env({
        .emulator_type = chip8::vm_t::settings_t::CHIP_8,
    });
    env.vm.load_data(rom_bytes, chip8::ROM_OFFSET);

    env.vm.emulate_duration(duration);

    auto actual = env.get_video_system().render_for_test();

    ASSERT_EQ(expected, actual);
}


TEST(RomTests, Chip8Logo) {
    std::string expected_image = R"(
................................................................
............#####.#....................#..........##............
..............#.....##.#...##..###...###.#..#..##..#............
..............#...#.#.#.#.#..#.#..#.#..#.#..#.#.................
..............#...#.#...#.####.#..#.#..#.#..#..#................
..............#...#.#...#.#....#..#.#..#.#..#...#...............
..............#...#.#...#..###.#..#..###..###.##................
................................................................
................................................................
...........#####...##.......##..#####...........#######.........
..........#######.###......###.#######.........###...###........
.........###...##.###......###.###..###.......###.....##........
........###.......###..........###...##.......###.....##........
........###..#.#..###.......##.###...##.......###.....##........
........###.......######...###.###...##........###...##.........
........###.#...#.#######..###.###...##.####....######..........
........###..###..###..###.###.###..###.####...###..###.........
........###.......###...##.###.#######........###....###........
........###.......###...##.###.######........###......##........
........###.......###...##.###.###...........###......##........
........###.......###...##.###.###.#.#...###.###......##........
.........###...##.###...##.###.###.###...#.#.####....###........
..........#######.###...##.###.###...#...#.#..#########.........
...........#####..###...##.###.###...#.#.###...#######..........
................................................................
................................................................
.............###..##...##.#.......##......#.#....##.............
..............#..#..#.#...###....#...#..#...###.#..#............
..............#..####..#..#.......#..#..#.#.#...####............
..............#..#......#.#........#.#..#.#.#...#...............
..............#...###.##...##....##...###.#..##..###............
................................................................
)";
    expected_image = expected_image.substr(1); // remove first newline

    launch_rom("tests/data/1-chip8-logo.ch8", expected_image, std::chrono::milliseconds(100));
}

TEST(RomTests, IBMLogo) {
    std::string expected_image = R"(
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
............########.#########...#####.........#####..#.#.......
......................................................#.#.......
............########.###########.######.......######...#........
................................................................
..............####.....###...###...#####.....#####....#.#.......
......................................................###.......
..............####.....#######.....#######.#######......#.......
........................................................#.......
..............####.....#######.....###.#######.###..............
.......................................................#........
..............####.....###...###...###..#####..###..............
......................................................###.......
............########.###########.#####...###...#####..#.#.......
......................................................#.#.......
............########.#########...#####....#....#####..###.......
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
................................................................
)";
    expected_image = expected_image.substr(1); // remove first newline

    launch_rom("tests/data/2-ibm-logo.ch8", expected_image, std::chrono::milliseconds(100));
}

TEST(RomTests, Corax) {
    std::string expected_image = R"(
................................................................
..###.#.#.........###.#.#.........###.#.#.........###.###.......
...##..#...#.#......#..#...#.#....###.###..#.#....#...##...#.#..
....#.#.#..##.....##..#.#..##.....#.#...#..##.....##....#..##...
..###.#.#..#......###.#.#..#......###...#..#......#...##...#....
................................................................
..#.#.#.#.........###.###.........###.###.........###.###.......
..###..#...#.#....#.#.##...#.#....###.##...#.#....#....##..#.#..
....#.#.#..##.....#.#.#....##.....#.#...#..##.....##....#..##...
....#.#.#..#......###.###..#......###.##...#......#...###..#....
................................................................
..###.#.#.........###.###.........###.###.........###.###.......
..##...#...#.#....###.#.#..#.#....###...#..#.#....#...##...#.#..
....#.#.#..##.....#.#.#.#..##.....#.#..#...##.....##..#....##...
..##..#.#..#......###.###..#......###..#...#......#...###..#....
................................................................
..###.#.#.........###.##..........###..##.............#.#.......
....#..#...#.#....###..#...#.#....###.#....#.#....#.#..#...#.#..
...#..#.#..##.....#.#..#...##.....#.#.###..##.....#.#.#.#..##...
...#..#.#..#......###.###..#......###.###..#.......#..#.#..#....
................................................................
..###.#.#.........###.###.........###.###.......................
..###..#...#.#....###...#..#.#....###.##...#.#..................
....#.#.#..##.....#.#.##...##.....#.#.#....##...................
..##..#.#..#......###.###..#......###.###..#....................
................................................................
..##..#.#.........###.###.........###..##.............#.#...###.
...#...#...#.#....###..##..#.#....#...#....#.#....#.#.###...#.#.
...#..#.#..##.....#.#...#..##.....##..###..##.....#.#...#...#.#.
..###.#.#..#......###.###..#......#...###..#.......#....#.#.###.
................................................................
................................................................
)";
    expected_image = expected_image.substr(1); // remove first newline

    launch_rom("tests/data/3-corax+.ch8", expected_image, std::chrono::seconds(1));
}

TEST(RomTests, Flags) {
    std::string expected_image = R"(
#.#..#..##..##..#.#...##....................###.................
###.#.#.#.#.#.#.#.#....#...#.#.#.#.#.#........#..#.#.#.#.#.#....
#.#.###.##..##...#.....#...##..##..##.......##...##..##..##.....
#.#.#.#.#...#....#....###..#...#...#........###..#...#...#......
................................................................
###...................#.#...................###.................
.##..#.#.#.#.#.#......###..#.#.#.#.#.#.#.#..##...#.#.#.#.#.#.#.#
..#..##..##..##.........#..##..##..##..##.....#..##..##..##..##.
###..#...#...#..........#..#...#...#...#....##...#...#...#...#..
................................................................
###...................###...................###.................
#....#.#.#.#.#.#........#..#.#.#.#.#.#.#.#..##...#.#.#.#.#.#....
###..##..##..##.........#..##..##..##..##...#....##..##..##.....
###..#...#...#..........#..#...#...#...#....###..#...#...#......
................................................................
................................................................
###..#..##..##..#.#...#.#...................###.................
#...#.#.#.#.#.#.#.#...###..#.#.#.#.#.#.#.#..##...#.#.#.#.#.#.#.#
#...###.##..##...#......#..##..##..##..##.....#..##..##..##..##.
###.#.#.#.#.#.#..#......#..#...#...#...#....##...#...#...#...#..
................................................................
###...................###...................###.................
#....#.#.#.#.#.#........#..#.#.#.#.#.#.#.#..##...#.#.#.#.#.#....
###..##..##..##.........#..##..##..##..##...#....##..##..##.....
###..#...#...#..........#..#...#...#...#....###..#...#...#......
................................................................
................................................................
###.###.#.#.###.##....###.###.........................#.#...###.
#.#..#..###.##..#.#...#...##...#.#.#.#............#.#.###...#.#.
#.#..#..#.#.#...##....##..#....##..##.............#.#...#...#.#.
###..#..#.#.###.#.#...#...###..#...#...............#....#.#.###.
................................................................
)";
    expected_image = expected_image.substr(1); // remove first newline

    launch_rom("tests/data/4-flags.ch8", expected_image, std::chrono::seconds(2));
}

TEST(RomTests, Quirks) {
    std::string expected_image = R"(
................................................................
.#.#.###.....##..###..##.###.###............###.##..............
.#.#.#.......#.#.##..##..##...#.............#.#.#.#........#.#..
.#.#.##......##..#.....#.#....#.............#.#.#.#........##...
..#..#.......#.#.###.##..###..#.............###.#.#........#....
................................................................
.###.###.###.###.##..#.#....................###.##..............
.###.##..###.#.#.#.#.#.#....................#.#.#.#........#.#..
.#.#.#...#.#.#.#.##...#.....................#.#.#.#........##...
.#.#.###.#.#.###.#.#..#.....................###.#.#........#....
................................................................
.##..###..##.##......#.#..#..###.###........###.##..............
.#.#..#..##..#.#.....#.#.#.#..#...#.........#.#.#.#........#.#..
.#.#..#....#.##......###.###..#...#.........#.#.#.#........##...
.##..###.##..#....#..###.#.#.###..#.........###.#.#........#....
................................................................
.###.#...###.##..##..###.##...##............###.##..............
.#...#....#..#.#.#.#..#..#.#.#..............#.#.#.#........#.#..
.#...#....#..##..##...#..#.#.#.#............#.#.#.#........##...
.###.###.###.#...#...###.#.#..##............###.#.#........#....
................................................................
..##.#.#.###.###.###.###.##...##............###.###.###.........
.##..###..#..#....#...#..#.#.#..............#.#.#...#......#.#..
...#.#.#..#..##...#...#..#.#.#.#............#.#.##..##.....##...
.##..#.#.###.#....#..###.#.#..##............###.#...#......#....
................................................................
..##.#.#.###.##..###.##...##................###.###.###.........
...#.#.#.###.#.#..#..#.#.#..................#.#.#...#......#.#..
...#.#.#.#.#.##...#..#.#.#.#................#.#.##..##.....##...
.##...##.#.#.#...###.#.#..##................###.#...#......#....
................................................................
................................................................
)";
    expected_image = expected_image.substr(1); // remove first newline

    auto rom_bytes = load_rom("tests/data/5-quirks.ch8");

    env_t env({
        .emulator_type = chip8::vm_t::settings_t::CHIP_8,
    });
    env.vm.load_data(rom_bytes, chip8::ROM_OFFSET);

    env.get_keyboard().pressed_keys[chip8::keyboard_key_t::KEY_1] = true;
    env.vm.emulate_duration(std::chrono::seconds(1));

    env.get_keyboard().pressed_keys[chip8::keyboard_key_t::KEY_1] = false;
    env.vm.emulate_duration(std::chrono::seconds(5));

    auto actual = env.get_video_system().render_for_test();

    printf("actual:\n%s\n", actual.data());

    ASSERT_EQ(expected_image, actual);
}
