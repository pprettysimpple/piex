#include <cstdio>
#include <filesystem>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include <common.h>
#include <instructions.h>
#include <platform_impl/timers_basic.h>
#include <platform_impl/video_none.h>
#include <vm.h>



struct keyboard_mock_t : chip8::keyboard_system_iface_t {
    bool is_pressed(chip8::keyboard_key_t key) const noexcept override {
        return pressed_keys[static_cast<size_t>(key)];
    }

    chip8::keyboard_key_t wait_for_keypress() const noexcept override {
        return chip8::keyboard_key_t::KEY_0;
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
    chip8::vm_t vm;

    env_t() :
        vm(std::make_unique<keyboard_mock_t>(),
           std::make_unique<chip8::timers_system_basic_t>(),
           std::make_unique<video_system_mock_t>())
    {
        vm.load_data(chip8::CHIP8_STANDARD_FONTSET_VIEW, 0);
    }

    video_system_mock_t& get_video_system() noexcept {
        return *static_cast<video_system_mock_t*>(vm.video_system.get());
    }

    keyboard_mock_t& get_keyboard() noexcept {
        return *static_cast<keyboard_mock_t*>(vm.keyboard_system.get());
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

void launch_rom(std::string_view filename, std::string expected, uint64_t max_cycles) {
    auto rom_bytes = load_rom(filename);

    env_t env;
    env.vm.load_data(rom_bytes, chip8::ROM_OFFSET);

    env.vm.emulate(500, max_cycles);

    auto actual = env.get_video_system().render_for_test();

    ASSERT_STREQ(expected.data(), actual.data());
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

    launch_rom("../../tests/data/1-chip8-logo.ch8", expected_image, 100);
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

    launch_rom("../../tests/data/2-ibm-logo.ch8", expected_image, 100);
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

    launch_rom("../../tests/data/3-corax+.ch8", expected_image, 500);
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

    launch_rom("../../tests/data/4-flags.ch8", expected_image, 1000);
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

    auto rom_bytes = load_rom("../../tests/data/5-quirks.ch8");

    env_t env;
    env.vm.load_data(rom_bytes, chip8::ROM_OFFSET);

    env.get_keyboard().pressed_keys[chip8::keyboard_key_t::KEY_0] = true; 
    env.vm.emulate(500, 500);

    env.get_keyboard().pressed_keys[chip8::keyboard_key_t::KEY_0] = false;
    env.vm.emulate(500, 500 * 5);

    auto actual = env.get_video_system().render_for_test();

    ASSERT_STREQ(expected_image.data(), actual.data());
}
