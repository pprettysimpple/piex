#include <iostream>
#include <sstream>

#include <platform_impl/video_abstract.h>


namespace chip8 {

bool video_system_abstract_t::draw_sprite(uint8_t start_col, uint8_t start_row, const bytes_view sprite) {
    bool collision = false;
    for (size_t row = 0; row < sprite.size(); ++row) {
        uint8_t sprite_byte = sprite[row];
        for (size_t col = 0; col < 8; ++col) {
            // if (start_col + col >= VIDEO_WIDTH /* || start_row + row >= VIDEO_HEIGHT */) {
            //     continue;
            // }    

            bool sprite_pixel = (sprite_byte >> (7 - col)) & 0x01;

            auto r = (start_row + row) % VIDEO_HEIGHT;
            auto c = (start_col + col) % VIDEO_WIDTH;
            bool& screen_pixel = memory[r][c];

            if (sprite_pixel && screen_pixel) {
                collision = true;
            }
            screen_pixel = (screen_pixel != sprite_pixel);
        }
    }

    render();
    return collision;
}

void video_system_abstract_t::clear_screen() {
    for (auto& row : memory) {
        std::fill(row.begin(), row.end(), false);
    }

    render();
}

} // namespace chip8
