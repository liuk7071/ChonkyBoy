#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = Cpu.Fetch8();
    int old_cycles = Cpu.frame_cycles; 
    Cpu.execute(opcode);
    Ppu.Tick(Cpu.frame_cycles - old_cycles);
}

void gb::runFrame() {
    framesPassed++;
    while(Cpu.frame_cycles < 70224) {
        step();
    }
    Cpu.frame_cycles = 0;
}