#include "memory.hpp"

memory::memory(cart* cartptr, ppu* ppuptr) {
    Cart = cartptr;
    PPU = ppuptr;
    //bootrom = fopen(bootrom_path, "r");

    std::ifstream file{bootrom_path, std::ios::binary};
    if (!file.is_open()) {
        printf("Failed to open %s\n", bootrom_path);
        exit(1);
    }
    file.read((char*)bootrom, 256);
    file.close();
}

void memory::Write(u16 addr, u8 data) {
    if(addr >= 0x0000 && addr <= 0x3fff) {  // ROM
        return;
    }
    if(addr >= 0xfe00 && addr <= 0xfe9f) {  // OAM
        return;
    }
    if(addr >= 0xfea0 && addr <= 0xfeff) {
        return;
    }
    if(addr == 0xff7f) {
        return;
    }
    if(addr >= 0x8000 && addr <= 0x9fff) {  // VRAM
        PPU->vram[addr & 0x1fff] = data; return;
    }
    if(addr >= 0xc000 && addr <= 0xfdff) {
        wram[addr & 0x1fff] = data; return;
    }
    if(addr >= 0xff80 && addr <= 0xfffe) {
        hram[addr & 0x7f] = data; return;
    }
    if(addr == 0xff01) {
        printf("%c", data); return;
    }
    if(addr == 0xff02) {
        return;
    }
    if(addr == 0xff07) {
        TAC = data; return;
    }
    if(addr == 0xff0f) {
        IF = data; return;
    }
    if(addr == 0xff11) {
        return;
    }
    if(addr == 0xff12) {
        return;
    }
    if(addr == 0xff13) {
        return;
    }
    if(addr == 0xff14) {
        return;
    }
    if(addr == 0xff24) {
        return;            
    }
    if(addr == 0xff25) {
        return;            
    }
    if(addr == 0xff26) {
        return;           
    }
    if(addr == 0xff40) {    // LCDC
        PPU->lcdc = data; return;
    }
    if(addr == 0xff41) {    // STAT
        return;
    }
    if(addr == 0xff42) {    // SCY
        PPU->scy = data; return;
    }
    if(addr == 0xff43) {    // SCX
        PPU->scx = data; return;
    }
    if(addr == 0xff47) {    // BGP
        PPU->bgp = data; return;
    }
    if(addr == 0xff48) {    // OBP0
        return;
    }
    if(addr == 0xff49) {    // OBP1
        return;
    }
    if(addr == 0xff50) {
        bootrom_enabled = false;
        return;
    }
    if(addr == 0xffff) {
        IE = data; return;
    }

    Helpers::panic("Unhandled memory write: 0x{:04X}\n", addr);
}
void memory::Write16(u16 addr, u16 data) {
    Write(addr + 1, (data & 0xff00) >> 8);
    Write(addr, data & 0xff);
    return;
}

u8 memory::Read(u16 addr) {
    if(addr >= 0x0000 && addr < 0x0100) {
        if (bootrom_enabled) {
            return bootrom[addr];
        }
        else {
            return Cart->read(addr);
        }
    }
    if(addr >= 0x0100 && addr <= 0x3fff) {
        return Cart->read(addr);
    }
    if(addr >= 0xc000 && addr <= 0xfdff) {
        return wram[addr & 0x1fff];
    }
    if(addr >= 0xff80 && addr <= 0xfffe) {
        return hram[addr & 0x7f];
    }
    if(addr >= 0x4000 && addr <= 0x7fff) {
        return Cart->read(addr);
    }
    if(addr == 0xff42) {
        return PPU->scy;
    }
    if(addr == 0xff44) {    // LY
        return PPU->ly;
    }

    Helpers::panic("Unhandled memory read: 0x{:04X}\n", addr);
}
u16 memory::Read16(u16 addr) {
    u8 byte1 = Read(addr);
    u8 byte2 = Read(addr + 1);
    return byte1 | (byte2 << 8);
}


