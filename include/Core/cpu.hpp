#pragma once
#include <array>
#include <string>
#include <sstream>
#include <cstdarg>
#include "utils.hpp"
#include "BitField.hpp"
#include "memory.hpp"

class cpu {
public:
    cpu(memory* memptr);
    bool halted = false;
    memory* Memory;
    bool debug_cpu = true;
    bool skip_bootrom = false;
    void debug(const char* fmt, ...) {
        if (debug_cpu) {
		std::va_list args;
		va_start(args, fmt);
		std::vprintf(fmt, args);
		va_end(args);
	    }
    }

    // Registers
    union {     // AF
        u16 af;
        BitField <8, 8, u16> a;
        BitField <7, 1, u16> Z;
        BitField <6, 1, u16> N;
        BitField <5, 1, u16> HC;
        BitField <4, 1, u16> C;
        BitField <0, 8, u16> f;
    };
    union {     // BC
        u16 bc;
        BitField <0, 8, u16> c;
        BitField <8, 8, u16> b;
    };
    union {     // DE
        u16 de;
        BitField <0, 8, u16> e;
        BitField <8, 8, u16> d;
    };
    union {     // hl
        u16 hl;
        BitField <0, 8, u16> l;
        BitField <8, 8, u16> h;
    };
    
    u16 pc = 0;
    u16 sp;

    bool ime = false;

    u8 GetR8(int number) {
        switch(number) {
            case 0: return b.Value();
            case 1: return c.Value();
            case 2: return d.Value();
            case 3: return e.Value();
            case 4: return h.Value();
            case 5: return l.Value();
            case 6: return Memory->Read(hl);
            case 7: return a.Value();
        }
    }
    void SetR8(int number, u8 value) {
        switch(number) {
            case 0: b = value; break;
            case 1: c = value; break;
            case 2: d = value; break;
            case 3: e = value; break;
            case 4: h = value; break;
            case 5: l = value; break;
            case 6: Memory->Write(hl, value); break;
            case 7: a = value; break;
        }
        return;
    }
    template<int group>
    u16 GetR16(int number) {
        if (group == 1) {
            switch (number) {
            case 0:	// BC
                return bc;
            case 1:	// DE
                return de;
            case 2: // HL
                return hl;
            case 3: // SP
                return sp;
            }
        }
        else if (group == 2) {
            switch (number) {
            case 0:	// BC
                return bc;
            case 1:	// DE
                return de;
            case 2: // HL+
                return hl++;
            case 3: // HL-
                return hl--;
            }
        }
        else if (group == 3) {
            switch (number) {
            case 0:	// BC
                return bc;
            case 1:	// DE
                return de;
            case 2: // HL
                return hl;
            case 3: // AF
                return af;
            }
        }
        else {
            Helpers::panic("Bad R16 group\n");
        }
    }
    template<int group>
    void SetR16(int number, u16 value) {
        if (group == 1) {
            switch (number) {
            case 0:	// BC
                bc = value;
                break;
            case 1:	// DE
                de = value;
                break;
            case 2: // HL
                hl = value;
                break;
            case 3: // SP
                sp = value;
                break;
            }
        }
        else if (group == 3) {
            switch (number) {
            case 0:	// BC
                bc = value;
                break;
            case 1:	// DE
                de = value;
                break;
            case 2: // HL
                hl = value;
                break;
            case 3: // AF
                value &= ~0b1111;   // Low 4 bits of F are always 0
                af = value;
                break;
            }
        }
        else {
            Helpers::panic("Bad R16 group\n");
        }
        return;
    }
	bool CheckForCondition(uint8_t opcode) {
        if (opcode & 1) // There is no condition
            return true;

        switch ((opcode >> 3) & 3) {
        case 0: {	// NZ
            return !Z;
        }
        case 1: {	// Z
            return Z;
        }
        case 2: {	// NC
            return !C;
        }
        case 3: {	// C
            return C;
        }
        }
    }
    u8 Fetch8();
    u16 Fetch16();

    void Push(u16 data) {
        sp -= 2;
        Memory->Write16(sp, data); 
        frame_cycles += 8;
        return;
    }
    u16 Pop() {
        u16 data = Memory->Read16(sp);
        sp += 2;
        frame_cycles += 8;
        return data;
    }

    void execute(u8 opcode);
    int frame_cycles = 0;
    int div_cycles = 0, tima_cycles = 0;
    int tima_clock_speeds[4] = {1024, 16, 64, 256};
};