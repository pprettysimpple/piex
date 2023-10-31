#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include <core/common.h>
#include <core/iface/keyboard.h>
#include <core/iface/video.h>

#include <impl_basic/random_crand.h>
#include <impl_basic/timers_basic.h>
#include <impl_basic/sound_none.h>


namespace chip8::sdl {

struct sdl_system_facade_t : video_system_iface_t,
                             keyboard_system_iface_t,
                             timers_system_basic_t,
                             random_system_crand_t,
                             sound_system_none_t {
    static inline constexpr int PIXEL_SIZE = 16;

    // keymap
    // 1 2 3 4
    // q w e r
    // a s d f
    // z x c v
    // maps to
    // 1 2 3 C
    // 4 5 6 D
    // 7 8 9 E
    // A 0 B F

    static std::optional<keyboard_key_t> sdl_key_to_chip8_key(SDL_Keycode key) {
        switch (key) {
            case SDLK_1: return KEY_1;
            case SDLK_2: return KEY_2;
            case SDLK_3: return KEY_3;
            case SDLK_4: return KEY_C;
            case SDLK_q: return KEY_4;
            case SDLK_w: return KEY_5;
            case SDLK_e: return KEY_6;
            case SDLK_r: return KEY_D;
            case SDLK_a: return KEY_7;
            case SDLK_s: return KEY_8;
            case SDLK_d: return KEY_9;
            case SDLK_f: return KEY_E;
            case SDLK_z: return KEY_A;
            case SDLK_x: return KEY_0;
            case SDLK_c: return KEY_B;
            case SDLK_v: return KEY_F;
            default: return std::nullopt;
        }
    }

    static SDL_Keycode chip8_key_to_sdl_key(keyboard_key_t key) {
        switch (key) {
            case KEY_1: return SDLK_1;
            case KEY_2: return SDLK_2;
            case KEY_3: return SDLK_3;
            case KEY_C: return SDLK_4;
            case KEY_4: return SDLK_q;
            case KEY_5: return SDLK_w;
            case KEY_6: return SDLK_e;
            case KEY_D: return SDLK_r;
            case KEY_7: return SDLK_a;
            case KEY_8: return SDLK_s;
            case KEY_9: return SDLK_d;
            case KEY_E: return SDLK_f;
            case KEY_A: return SDLK_z;
            case KEY_0: return SDLK_x;
            case KEY_B: return SDLK_c;
            case KEY_F: return SDLK_v;
        }
        
        throw std::runtime_error("invalid key");
    }

    sdl_system_facade_t();

    virtual ~sdl_system_facade_t() override {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    virtual void render(const video_memory_t& video_memory) override;

    virtual bool is_pressed(keyboard_key_t key) override;

    virtual keyboard_key_t wait_for_keypress() override;

    void ui_thread_func();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::thread ui_thread;

    std::array<std::atomic<bool>, KEYPAD_SIZE> key_pressed;
    std::atomic<std::optional<keyboard_key_t>> last_key_pressed;
};

} // namespace chip8::sdl
