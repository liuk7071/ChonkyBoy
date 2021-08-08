#include "ppu.hpp"

ppu::ppu() {
    framebuffer.fill (0xff);
    lcdc = 0x91;
    ly = 0x91;
    bgp = 0xfc;
}

u16 ppu::Read16(u16 addr) {
    u8 byte1 = vram[addr & 0x1fff];
    u16 byte2 = vram[addr + 1 & 0x1fff];
    return byte1 | (byte2 << 8);
}
void ppu::SetPixel(int x, int y, u8 r, u8 g, u8 b, u8 a) {
    int pixel = (y * 4) * 160 + (x * 4);
    framebuffer[pixel] = r;
    framebuffer[pixel+1] = g;
    framebuffer[pixel+2] = b;
    framebuffer[pixel+3] = a;
}

void ppu::Tick(int cycles) {
    if(!enabled.Value()) {
        return;
    }

    current_cycles += cycles;

    switch(mode) {
        case OAM: {
            if(current_cycles >= 80) {
                mode = Drawing;
            }
            break;
        }
        case Drawing: {
            if(current_cycles >= 172) {
                RenderBGLine();
                mode = HBlank;
            }
            break;
        }
        case HBlank: {
            if(current_cycles >= 204) {
                ly++;
                current_cycles = 0;
                mode = (ly == 144) ? VBlank : OAM;
            }
            break;
        }
        case VBlank: {
            if(current_cycles >= 456) {
                ly++;
                current_cycles = 0;
            }
            if(ly == 154) {
                mode = OAM;
                ly = 0;
            }
            break;
        }
    }
}

// Drawing
void ppu::RenderBGLine() {
    u16 bgmap = (!bg_tilemap.Value()) ? 0x9800 : 0x9c00;
    u16 tiledata = (!bgwin_tiledata.Value()) ? 0x8800 : 0x8000;

    for(int x = 0; x < 160; x++) {
        u8 scrolledx = scx + x;
        u8 scrolledy = scy + ly;
        u16 tileline;
        u8 tile_index;

        tile_index = vram[(bgmap + ((scrolledy / 8) * 32) + (scrolledx / 8) & 0x1fff)];
        tileline = Read16((tiledata == 0x8000) ? (tiledata + (tile_index * 16) + ((scrolledy & 7) * 2)) : (0x9000 + s16(s8(tile_index)) * 16 + ((scrolledy & 7) * 2)));
        u8 high = (tileline >> 8);
        high >>= (7 - (scrolledx & 7));
        high &= 1;
        u8 low = (tileline & 0xff);
        low >>= (7 - (scrolledx & 7));
        low &= 1;
        u8 pixel = (high << 1) | low;

        u8 color_index = (bgp >> (pixel << 1)) & 3;
        color_index *= 4;
        SetPixel(scrolledx, ly, palette[color_index], palette[color_index + 1], palette[color_index + 2], palette[color_index + 3]);
    }
}