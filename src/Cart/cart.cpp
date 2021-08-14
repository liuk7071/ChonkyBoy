#include "cart.hpp"

cart::cart(const char* dir) {
    rom = Helpers::loadROM(dir);
    switch(rom[0x147]) {
    case 0x00: mbc = NoMBC; break;
    case 0x01: mbc = NoMBC; break;
    case 0x13: mbc = MBC3; break;
    default: Helpers::panic("Unhandled MBC cart: {:02X}\n", rom[0x147]);
    }
}

u8 cart::read(u16 addr) {
    if(addr >= 0x0000 && addr <= 0x3fff) 
        return rom[addr];
    if(addr >= 0x4000 && addr <= 0x7fff) {
        switch(mbc) {
        case NoMBC: {
            return rom[addr];
        }
        case MBC3: {
            u16 actual_address = (addr - 0x4000) + (RomBank * 0x4000);
            return rom[actual_address];
        }
        }
    }
    if(addr >= 0xa000 && addr <= 0xbfff) {
        u16 actual_address = (addr - 0xa0000) + (RamBank * 0x2000);
        return RamEnable ? extram[actual_address] : 0xff;
    }
    Helpers::panic("Unhandled cart read {:02X}\n", addr);
}
void cart::write(u16 addr, u8 data) {
    if(addr >= 0x0000 && addr <= 0x1fff) {
        RamEnable = (data & 0x0f) == 0x0a;
        return;
    }
    if(addr >= 0x2000 && addr <= 0x3fff) {
        RomBank = data & 0x7f;
        if(RomBank == 0) RomBank = 1;
        return;
    }
    if(addr >= 0x4000 && addr <= 0x5fff) { 
        RamBank = data & 0x7f;
        if(RamBank == 0) RamBank = 1;
        return;
    }
    if(addr >= 0x6000 && addr <= 0x7fff) {  // RTC stuff
        return;
    }
    if(addr >= 0xa000 && addr <= 0xbfff) {
        if(RamEnable) {
            u16 actual_address = (addr - 0xa0000) + (RamBank * 0x2000);
            extram[actual_address] = data;
        }
        return;
    }
    Helpers::panic("Unhandled cart write {:02X}\n", addr);
}