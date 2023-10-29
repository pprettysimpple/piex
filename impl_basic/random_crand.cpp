#include "random_crand.h"

#include <cstdint>
#include <cstdlib>
#include <ctime>

namespace chip8 {

random_system_crand_t::random_system_crand_t() {
    srand(time(nullptr));
}

uint8_t random_system_crand_t::get_random_byte() {
    return static_cast<uint8_t>(rand() & 0xFF); // todo: implement
}

} // namespace chip8
