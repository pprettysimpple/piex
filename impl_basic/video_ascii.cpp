#include "video_ascii.h"

#include <iostream>
#include <sstream>


namespace chip8 {

void video_system_ascii_t::render(const video_memory_t& video_memory) {
    std::stringstream frame;
    frame << "\033[2J\033[1;1H";

    frame << "\n";
    
    for (size_t i = 0; i < VIDEO_HEIGHT; ++i) {
        for (size_t j = 0; j < VIDEO_WIDTH; ++j) {
            frame << (video_memory[i][j] ? '#' : '.');
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
