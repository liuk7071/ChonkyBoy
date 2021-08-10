#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <fstream>
#include <sstream>
#include "utils.hpp"
#include "cart.hpp"
#include "ppu.hpp"
#include "BitField.hpp"

class memory {
public:
    memory(cart* cartptr, ppu* ppuptr);
    const char* bootrom_path = "H:\\bootrom.gb";
    cart* Cart;
    ppu* PPU;
    
    void Write(u16 addr, u8 data);
    void Write16(u16 addr, u16 data);
    u8 Read(u16 addr);
    u16 Read16(u16 addr);

    u8 ReadBootrom(u16 addr);
    bool bootrom_enabled = true;

    u8 bootrom[256];
    u8 wram[8192];
    u8 hram[127];
    u8 IE;
    u8 IF;
    u8 TAC;
    u8 TMA;

    u8 joyp;
    void HandleJOYP();
};