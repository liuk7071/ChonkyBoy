#include "cart.hpp"

cart::cart(const char* dir) {
    rom = Helpers::loadROM(dir);
}

u8 cart::read(u16 addr) {
    return rom[addr];
}