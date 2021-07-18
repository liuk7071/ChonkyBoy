#include "memory.hpp"

memory::memory() {
    
}

void memory::write(u16 addr, u8 data) {
    switch(addr) {
    default:
        std::stringstream err; err << "Unhandled memory write: 0x" << std::hex << addr << "\n";
        Helpers::panic(err.str().c_str());
    }
}

u8 memory::read(u16 addr) {
    switch(addr) {
    default:
        std::stringstream err; err << "Unhandled memory read: 0x" << std::hex << addr << "\n";
        Helpers::panic(err.str().c_str());
    }
}