#include <iostream>
#include <sstream>

#include <platform_impl/video_ascii.h>


namespace chip8 {

namespace {

void video_to_ascii(const video_system_abstract_t::memory_t& video_memory, std::stringstream& result) {
    for (size_t i = 0; i < VIDEO_HEIGHT; ++i) {
        for (size_t j = 0; j < VIDEO_WIDTH; ++j) {
            result << (video_memory[i][j] ? '#' : ' ');
        }
        result << '\n';
    }
}

} // namespace

video_system_ascii_t::video_system_ascii_t() {
    clear_screen();
}

void video_system_ascii_t::render() {
    std::stringstream frame;
    frame << "\033[2J\033[1;1H";

    frame << "\n";
    video_to_ascii(memory, frame);
    frame << "\n\n";

    frame << "info:\n";
    static size_t frame_counter = 0;
    frame << "frame: " << frame_counter++ << '\n';

    std::cout << frame.str() << std::flush;
}

} // namespace chip8
