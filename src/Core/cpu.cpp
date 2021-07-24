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

u8 cpu::Fetch8() {
    frame_cycles += 4;
    return Memory->Read(pc++);
}

u16 cpu::Fetch16() {
    return Fetch8() | (Fetch8() << 8);
}

void cpu::execute(u8 opcode) {
    switch(opcode) {
    case 0x00: debug("0x%04x | 0x%02x | NOP\n", pc-1, opcode); frame_cycles += 4; break;
    case 0xc2: case 0xd2: case 0xca: case 0xda: case 0xc3: {    // JP Condition u16
        debug("0x%04x | 0x%02x | JP Cond u16\n", pc-1, opcode);
        u16 addr = Fetch16();
        if(CheckForCondition(opcode)) {
            pc = addr;
            frame_cycles += 4;
        }
        break;
    }

    default:
        Helpers::panic("Unhandled opcode 0x{:02X}\n", opcode);
    }
}