#pragma once
#include <array>
#include <utils.hpp>

class cart {
public:
    cart(const char* dir);
    std::vector<u8> rom;

    enum MBC {
        NoMBC,
        MBC1,
        MBC3
    };
    MBC mbc;
    u8 extram[8192];

    bool RamEnable = true;
    u8 RomBank = 1;
    u8 RamBank = 0;
    u8 read(u16 addr);
    void write(u16 addr, u8 data);
};