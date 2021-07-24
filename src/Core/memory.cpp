#include "memory.hpp"

memory::memory(cart* cartptr) {
    Cart = cartptr;
}

void memory::Write(u16 addr, u8 data) {
    switch(addr) {
    default:
        Helpers::panic("Unhandled memory write: 0x{:04X}\n", addr);
    }
}

u8 memory::Read(u16 addr) {
    if(addr >= 0x0000 && addr <= 0x3fff) {
        return Cart->read(addr);
    }

    Helpers::panic("Unhandled memory read: 0x{:04X}\n", addr);
}
