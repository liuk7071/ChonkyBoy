#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = 0;
    int old_cycles = Cpu.frame_cycles; 
    if(!Cpu.halted) {
        opcode = Cpu.Fetch8();
        Cpu.execute(opcode);
        Ppu.Tick(Cpu.frame_cycles - old_cycles);
    } else {
        Cpu.execute(opcode);
        Ppu.Tick(4);
    }
    Cpu.div_cycles += Cpu.frame_cycles - old_cycles;
    Cpu.tima_cycles += Cpu.frame_cycles - old_cycles;
}

void gb::runFrame() {
    framesPassed++;
    while(Cpu.frame_cycles < 70224) {
        step();
    }
    Cpu.frame_cycles = 0;
}