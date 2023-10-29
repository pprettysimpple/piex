#include <iostream>
#include <sstream>

#include <platform_impl/video_ascii.h>


namespace chip8 {

video_system_ascii_t::video_system_ascii_t() {
    clear_screen();
}

void video_system_ascii_t::render_impl() {
    std::stringstream frame;
    frame << "\033[2J\033[1;1H";

    frame << "\n";
    
    for (size_t i = 0; i < VIDEO_HEIGHT; ++i) {
        for (size_t j = 0; j < VIDEO_WIDTH; ++j) {
            frame << (memory[i][j] ? '#' : '.');
        }
        frame << '\n';
    }

    frame << "\n\n";

    frame << "info:\n";
    static size_t frame_counter = 0;
    frame << "frame: " << frame_counter++ << '\n';

    std::cout << frame.str() << std::flush;
}

} // namespace chip8
