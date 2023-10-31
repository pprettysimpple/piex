#include "platform.h"

#include <SDL2/SDL_events.h>

#include <iostream>
#include <optional>
#include <sstream>

#include <core/common.h>


namespace chip8::sdl {

sdl_system_facade_t::sdl_system_facade_t() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, VIDEO_WIDTH * PIXEL_SIZE, VIDEO_HEIGHT * PIXEL_SIZE, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::stringstream error;
        error << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error(error.str());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::stringstream error;
        error << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        throw std::runtime_error(error.str());
    }

    ui_thread = std::thread(&sdl_system_facade_t::ui_thread_func, this);
}

void sdl_system_facade_t::render(const video_memory_t& video_memory) {
    for (size_t i = 0; i < VIDEO_HEIGHT; ++i) {
        for (size_t j = 0; j < VIDEO_WIDTH; ++j) {
            SDL_Rect rect;
            rect.x = j * PIXEL_SIZE;
            rect.y = i * PIXEL_SIZE;
            rect.w = PIXEL_SIZE;
            rect.h = PIXEL_SIZE;

            if (video_memory[i][j]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }
            auto err_code = SDL_RenderFillRect(renderer, &rect);

            if (err_code != 0) {
                std::stringstream error;
                error << "SDL_RenderFillRect error: " << SDL_GetError() << std::endl;
                throw std::runtime_error(error.str());
            }
        }
    }

    SDL_RenderPresent(renderer);
}

bool sdl_system_facade_t::is_pressed(keyboard_key_t key) {
    return key_pressed[key].load();
}

keyboard_key_t sdl_system_facade_t::wait_for_keypress() {
    last_key_pressed.store(std::nullopt);
    last_key_pressed.wait(std::nullopt);

    auto key = last_key_pressed.load().value();

    key_pressed[key].wait(true);
    return static_cast<keyboard_key_t>(key);
}

void sdl_system_facade_t::ui_thread_func() {
    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
                case SDL_QUIT:
                    std::exit(EXIT_SUCCESS);
                    break;
                case SDL_KEYDOWN: {
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        std::exit(EXIT_SUCCESS);
                    }
                    auto key_opt = sdl_key_to_chip8_key(e.key.keysym.sym);
                    if (key_opt.has_value()) {
                        auto key = key_opt.value();
                        key_pressed[key].store(true);
                        key_pressed[key].notify_one();

                        last_key_pressed.store(key);
                        last_key_pressed.notify_one();
                    }
                    break;
                }
                case SDL_KEYUP: {
                    auto key_opt = sdl_key_to_chip8_key(e.key.keysym.sym);
                    if (key_opt.has_value()) {
                        auto key = key_opt.value();
                        key_pressed[key].store(false);
                        key_pressed[key].notify_one();
                    }
                    break;
                }
                default:
                    continue;
            }
        }
    }
}


} // namespace chip8::sdl
