#pragma once
#include <array>
#include <sstream>
#include "utils.hpp"
#include "BitField.hpp"

class memory {
public:
    memory();
    void write(u16 addr, u8 data);
    u8 read(u16 addr);

    // Memory map
    u8 bank0[4096];
    u8 wram[4096];
};