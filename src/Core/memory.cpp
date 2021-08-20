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
        Cart->write(addr, data); return;
    }
    if(addr >= 0x4000 && addr <= 0x7fff) {  // ROM
        Cart->write(addr, data); return;
    }
    if(addr >= 0xfe00 && addr <= 0xfe9f) {  // OAM
        //if((PPU->mode != ppu::OAM) && (PPU->mode != ppu::Drawing)) PPU->oam[addr & 0xff] = data; return;
        PPU->oam[addr & 0xff] = data; return;
    }
    if(addr >= 0xfea0 && addr <= 0xfeff) {
        return;
    }
    if(addr == 0xff7f) {    // idk tetris writes here
        return;
    }
    if(addr >= 0x8000 && addr <= 0x9fff) {  // VRAM
        //if(PPU->mode != ppu::Drawing) PPU->vram[addr & 0x1fff] = data; return;
        PPU->vram[addr & 0x1fff] = data; return;
    }
    if(addr >= 0xa000 && addr <= 0xbfff) {
        Cart->write(addr, data); 
        return;
    }
    if(addr >= 0xc000 && addr <= 0xfdff) {
        wram[addr & 0x1fff] = data; return;
    }
    if(addr >= 0xff80 && addr <= 0xfffe) {
        hram[addr & 0x7f] = data; return;
    }
    if(addr == 0xff00) {    // JOYP
        data &= ~0b1111; 
        joyp &= ~0b110000;
        joyp |= data;
        //printf("\n%x\n", joyp);
        return;
    }
    if(addr == 0xff01) {
        //printf("%c", data); 
        return;
    }
    if(addr == 0xff02) {
        return;
    }
    if(addr == 0xff03) {
        Helpers::warn("Write to ff03h\n");
        return;
    }
    if(addr == 0xff04) {    // DIV
        div = 0; return;
    }
    if(addr == 0xff05) {    // TIMA
        tima = data; return;
    }
    if(addr == 0xff06) {
        TMA = data; return;
    }
    if(addr == 0xff07) {
        TAC = data; return;
    }
    if(addr == 0xff0f) {
        IF = data; return;
    }
    if(addr == 0xff10) {
        return;
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
    if(addr == 0xff16) {
        return;
    }
    if(addr == 0xff17) {
        return;
    }
    if(addr == 0xff18) {
        return;
    }
    if(addr == 0xff19) {
        return;
    }
    if(addr == 0xff1a) {
        return;
    }
    if(addr == 0xff1b) {
        return;
    }
    if(addr == 0xff1c) {
        return;
    }
    if(addr == 0xff1d) {
        return;
    }
    if(addr == 0xff1e) {
        return;
    }
    if(addr == 0xff20) {
        return;
    }
    if(addr == 0xff21) {
        return;
    }
    if(addr == 0xff22) {
        return;
    }
    if(addr == 0xff23) {
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
    if(addr >= 0xff30 && addr <= 0xff3f) {  // Wave pattern ram
        return;
    }
    if(addr == 0xff40) {    // LCDC
        PPU->lcdc = data; return;
    }
    if(addr == 0xff41) {    // STAT
        data &= ~0b11;
        PPU->stat &= 0b11111100;
        PPU->stat |= data; 
        printf("%x\n", PPU->stat);
        return;
    }
    if(addr == 0xff42) {    // SCY
        PPU->scy = data; return;
    }
    if(addr == 0xff43) {    // SCX
        PPU->scx = data; return;
    }
    if(addr == 0xff46) {    // OAM DMA
        u16 start_address = u16(data << 8);
        for(u8 i = 0; i < 0xa0; i++) {
            PPU->oam[i] = Read(start_address | i);
        }
        return;
    }
    if(addr == 0xff47) {    // BGP
        PPU->bgp = data; return;
    }
    if(addr == 0xff48) {    // OBP0
        PPU->obp0 = data; return;
    }
    if(addr == 0xff49) {    // OBP1
        PPU->obp1 = data; return;
    }
    if(addr == 0xff4a) {
        PPU->wy = data; return;
    }
    if(addr == 0xff4b) {
        PPU->wx = data; return;
    }
    if(addr == 0xff50) {
        bootrom_enabled = false;
        return;
    }
    if(addr == 0xffff) {
        IE = data; return;
    }

    // GBC regs
    if(addr == 0xff56) {
        return;
    }

    Helpers::panic("Unhandled memory write: 0x{:04X}\n", addr);
}
void memory::Write16(u16 addr, u16 data) {
    Write(addr, data & 0xff);
    Write(addr + 1, (data & 0xff00) >> 8);
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
    if(addr >= 0x8000 && addr <= 0x9fff) {  // VRAM
        //return !(PPU->mode == ppu::Drawing) ? 0xff : PPU->vram[addr & 0x1fff];
        return PPU->vram[addr & 0x1fff];
    }
    if(addr >= 0xa000 && addr <= 0xbfff) {
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
    if(addr >= 0xfe00 && addr <= 0xfe9f) {  // OAM
        //if((PPU->mode != ppu::OAM) && (PPU->mode != ppu::Drawing)) return PPU->oam[addr & 0xff];
        //else return 0xff;
        return PPU->oam[addr & 0xff];
    }
    if(addr == 0xff00) {    // JOYP
        HandleJOYP();
        return joyp;
    }
    if(addr == 0xff04) {    // DIV
        return div;
    }
    if(addr == 0xff0f) {
        return IF;
    }
    if(addr == 0xff10) {
        return 0;
    }
    if(addr == 0xff11) {
        return 0;
    }
    if(addr == 0xff12) {
        return 0;
    }
    if(addr == 0xff13) {
        return 0;
    }
    if(addr == 0xff14) {
        return 0;
    }
    if(addr == 0xff16) {
        return 0;
    }
    if(addr == 0xff17) {
        return 0;
    }
    if(addr == 0xff18) {
        return 0;
    }
    if(addr == 0xff19) {
        return 0;
    }
    if(addr == 0xff1a) {
        return 0;
    }
    if(addr == 0xff1b) {
        return 0;
    }
    if(addr == 0xff1c) {
        return 0;
    }
    if(addr == 0xff1d) {
        return 0;
    }
    if(addr == 0xff1e) {
        return 0;
    }
    if(addr == 0xff20) {
        return 0;
    }
    if(addr == 0xff21) {
        return 0;
    }
    if(addr == 0xff22) {
        return 0;
    }
    if(addr == 0xff23) {
        return 0;
    }
    if(addr == 0xff24) {
        return 0;            
    }
    if(addr == 0xff25) {
        return 0;            
    }
    if(addr == 0xff26) {
        return 0;           
    }
    if(addr >= 0xff30 && addr <= 0xff3f) {  // Wave pattern ram
        return 0xff;
    }
    if(addr == 0xff40) {
        return PPU->lcdc;
    }
    if(addr == 0xff41) {
        return PPU->stat;
    }
    if(addr == 0xff42) {
        return PPU->scy;
    }
    if(addr == 0xff44) {    // LY
        return PPU->ly;
    }
    if(addr == 0xff48) {
        return PPU->obp0;
    }
    if(addr == 0xff49) {
        return PPU->obp1;
    }
    if(addr == 0xffff) {
        return IE;
    }

    // GBC regs

    Helpers::panic("Unhandled memory read: 0x{:04X}\n", addr);
}
u16 memory::Read16(u16 addr) {
    u8 byte1 = Read(addr);
    u8 byte2 = Read(addr + 1);
    return byte1 | (byte2 << 8);
}

const sf::Keyboard::Key ActionButtonKeyMappings[] = {
    sf::Keyboard::Z,    // A
    sf::Keyboard::X,    // B
    sf::Keyboard::C,    // Select
    sf::Keyboard::Enter // Start
};
const sf::Keyboard::Key DirectionButtonKeyMappings[] = {
    sf::Keyboard::Right,// Right
    sf::Keyboard::Left, // Left
    sf::Keyboard::Up,   // Up
    sf::Keyboard::Down  // Down
};
void memory::HandleJOYP() {
    joyp &= ~0b1111;
    u8 direction = ((joyp >> 4) & 1);
    u8 action = ((joyp >> 5) & 1);
    u8 buttons = (direction << 1) | action;
    u8 keys = 0b1111;

    if (buttons == 0b01) {
        keys &= ~(sf::Keyboard::isKeyPressed(DirectionButtonKeyMappings[0]) << 0);
        keys &= ~(sf::Keyboard::isKeyPressed(DirectionButtonKeyMappings[1]) << 1);
        keys &= ~(sf::Keyboard::isKeyPressed(DirectionButtonKeyMappings[2]) << 2);
        keys &= ~(sf::Keyboard::isKeyPressed(DirectionButtonKeyMappings[3]) << 3);
    } else if (buttons == 0b10) {
        keys &= ~(sf::Keyboard::isKeyPressed(ActionButtonKeyMappings[0]) << 0);
        keys &= ~(sf::Keyboard::isKeyPressed(ActionButtonKeyMappings[1]) << 1);
        keys &= ~(sf::Keyboard::isKeyPressed(ActionButtonKeyMappings[2]) << 2);
        keys &= ~(sf::Keyboard::isKeyPressed(ActionButtonKeyMappings[3]) << 3);
    }
    joyp |= window->hasFocus() ? keys : 0xf;
}


