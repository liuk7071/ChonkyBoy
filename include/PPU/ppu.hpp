#pragma once
#include <array>
#include "utils.hpp"
#include "cart.hpp"
#include "BitField.hpp"

class ppu {
public:
    ppu();
    void Tick(int cycles);
    int current_cycles;

    std::array <u8, 160 * 144 * 4> framebuffer; // 160x144 RGBA framebuffer
    u32 palette[16] = { 0xFE, 0xD0, 0x18, 0xFF, 0xD3, 0x56, 0x00, 0xFF, 0x5E, 0x12, 0x10, 0xFF, 0x0D, 0x04, 0x05, 0xFF };
    void SetPixel(int x, int y, u8 r, u8 g, u8 b, u8 a);
    u8 vram[8192];
    u16 Read16(u16 addr);

    enum Mode {
        HBlank,
        VBlank,
        OAM,
        Drawing
    };
    Mode mode = OAM;
    void RenderBGLine();

    // IO
    union {
        u8 lcdc;
        BitField <0, 1, u8> bgwin_priority;
        BitField <1, 1, u8> obj_enable;
        BitField <2, 1, u8> obj_size;
        BitField <3, 1, u8> bg_tilemap;
        BitField <4, 1, u8> bgwin_tiledata;
        BitField <0, 1, u8> win_enable;
        BitField <6, 1, u8> win_tilemap;
        BitField <7, 1, u8> enabled;
    };
    u8 bgp, scx, scy;
    u8 ly;
};