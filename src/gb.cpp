#include "gb.hpp"
#include "fmt/format.h"

void gb::step() {
    u8 opcode = 0;
    int old_cycles = Cpu.frame_cycles;
    opcode = !Cpu.halted ? Cpu.Fetch8() : 0;
    Cpu.execute(opcode);
    Cpu.frame_cycles += Cpu.halted ? 4 : 0;
    int passed = Cpu.frame_cycles - old_cycles;
    Ppu.Tick(passed);
    Cpu.div_cycles += (passed);
    Cpu.tima_cycles += (passed);
}

void gb::runFrame() {
    framesPassed++;
    while(Cpu.frame_cycles < 70224) {
        step();
    }
    Cpu.frame_cycles = 0;
}