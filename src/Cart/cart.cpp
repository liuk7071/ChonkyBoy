#include "cart.hpp"

cart::cart(const char* dir) {
    data = fopen(dir, "r");
}

u8 cart::read(u16 addr) {
    fseek(data, addr, SEEK_SET);
    u8 byte;
    fread(&byte, sizeof(u8), sizeof(u8)*1, data);
    return byte;
}