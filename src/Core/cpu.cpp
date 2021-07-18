#include "cpu.hpp"

cpu::cpu(memory* memptr) {
    Memory = memptr;

    // Initialize registers
    a = 0x01;
    f = 0xb0;
    b = 0x0;
    c = 0x13;
    d = 0x0;
    e = 0xd8;
    h = 0x01;
    l = 0x4d;

    pc = 0x100;
    sp = 0xfffe;

}

void cpu::execute(u16 address) {
    u8 opcode = Memory->read(address);

    switch(opcode) {
    default:
        std::stringstream err; err << "Unhandled opcode 0x" << std::hex << opcode << "\n";
        Helpers::panic(err.str().c_str());
    }
}

