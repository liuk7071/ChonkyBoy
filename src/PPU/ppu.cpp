#include "ppu.hpp"

ppu::ppu() {
    framebuffer.fill (0xff);
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
    //framebuffer[pixel+3] = a;
}

void ppu::LYC() {
    stat &= ~0b100;
    stat |= (lyc == ly) ? 0b100 : 0;
}
void ppu::StatINT() {
    if(stat & 0b1000000) {
        if(stat & 0b100) StatIRQ = true;
    }
}

void ppu::Tick(int cycles) {
    if(!enabled.Value()) {
        return;
    }

    current_cycles += cycles;

    switch(mode) {
        case OAM: {
            StatINT();
            if(current_cycles >= 80) {
                mode = Drawing;
                stat &= ~0b11;
                stat |= 3;
            }
            break;
        }
        case Drawing: {
            if(current_cycles >= 172) {
                RenderBGLine();
                RenderSpriteLine();
                mode = HBlank;
                stat &= ~0b11;
                stat |= 0;
            }
            break;
        }
        case HBlank: {
            if(current_cycles >= 204) {
                ly++;
                current_cycles -= 204;
                mode = (ly == 144) ? VBlank : OAM;
                stat &= ~0b11;
                stat |= (ly == 144) ? 1 : 2;
            }
            break;
        }
        case VBlank: {
            StatINT();
            if(ly == 144) VBlankIRQ = true;
            if(current_cycles >= 456) {
                ly++;
                current_cycles -= 456;
            }
            if(ly == 154) {
                mode = OAM;
                stat &= ~0b11;
                stat |= 2;
                ly = 0;
            }
            break;
        }
    }
    LYC();
}

// Drawing
void ppu::RenderBGLine() {
    u16 bgmap = (!bg_tilemap.Value()) ? 0x9800 : 0x9c00;
    u16 bgtiledata = (!bgwin_tiledata.Value()) ? 0x8800 : 0x8000;

    for(int x = 0; x < 160; x++) {
        u8 scrolledx = scx + x;
        u8 scrolledy = scy + ly;
        u16 tileline;
        u8 tile_index;

        tile_index = vram[(bgmap + ((scrolledy / 8) * 32) + (scrolledx / 8) & 0x1fff)];
        tileline = Read16((bgtiledata == 0x8000) ? (bgtiledata + (tile_index * 16) + ((scrolledy & 7) * 2)) : (0x9000 + s16(s8(tile_index)) * 16 + ((scrolledy & 7) * 2)));
        u8 high = (tileline >> 8);
        high >>= (7 - (scrolledx & 7));
        high &= 1;
        u8 low = (tileline & 0xff);
        low >>= (7 - (scrolledx & 7));
        low &= 1;
        u8 pixel = (high << 1) | low;

        u8 color_index = (bgp >> (pixel << 1)) & 3;
        color_index *= 4;
        SetPixel(x, ly, palette[color_index], palette[color_index + 1], palette[color_index + 2], palette[color_index + 3]);
    }
}
void ppu::FetchOAM() {
    sprite_count = 0;
    u8 sprite_height =  obj_size.Value() ? 16 : 8;

    for (int i = 0; i < 0xa0 && sprite_count < 10; i += 4) {
        Sprite sprite;
        sprite.ypos = (oam[i] - 16);
        sprite.xpos = (oam[i + 1] - 8);
        sprite.tilenum = (oam[i + 2]);
        sprite.flags = (oam[i + 3]);
        if (ly >= s16(sprite.ypos) && ly < (s16(sprite.ypos + sprite_height))) {
            sprites[sprite_count].ypos      = sprite.ypos;
            sprites[sprite_count].xpos      = sprite.xpos;
            sprites[sprite_count].tilenum   = sprite.tilenum;
            sprites[sprite_count].flags     = sprite.flags;
            sprite_count++;
        }
    }
}
void ppu::RenderSpriteLine() {
    if(!obj_enable.Value()) return;

    FetchOAM();
    for(int i = 0; i < sprite_count; i++) {
        u16 tiley = (sprites[i].yflip.Value()) ? (ly - sprites[i].ypos) ^ (obj_size.Value() ? 15 : 7) : ly - sprites[i].ypos;
        u8 obp = (sprites[i].palette) ? obp1 : obp0;
        u16 tileline = Read16(0x8000 + ((sprites[i].tilenum << 4) + (tiley << 1)));

        for(int x = 0; x < 8; x++) {
            u8 tilex = (sprites[i].xflip.Value()) ? 7 - x : x;
            u8 high = (tileline >> 8);
            high >>= (7 - tilex);
            high &= 1;
            u8 low = (tileline & 0xff);
            low >>= (7 - tilex);
            low &= 1;
            u8 pixel = (high << 1) | low;
            u8 color_index = (obp >> (pixel << 1)) & 3;
            color_index *= 4;
            if((framebuffer[(ly * 4) * 160 + ((sprites[i].xpos + tilex) * 4)] == 0xfa) && (sprites[i].xpos + tilex) < 160 && color_index != 0)
                SetPixel(sprites[i].xpos + tilex, ly, palette[color_index], palette[color_index + 1], palette[color_index + 2], palette[color_index + 3]);
        }
    }
}